from tkinter import filedialog as fd
from pathlib import Path
from datetime import datetime
from datetime import timedelta
import tkinter as tk
import os
import shutil
import subprocess
import tools
import random

class Route:
    def __init__(self, map):

        # Application et carte
        self.app = map.app
        self.map = map

        # Variable locale pour nombre de clics
        self.nbclicks = 0
        
        #Variable pour le nombre de chemins dessinés
        self.nbPathes = 0
        
        #Variable pour stocker les lignes du fichier de routage
        self.Lines = []
        
        #Variable pour les couleurs des chemins de routage
        self.colors = []
        
        #Variables pour restrictions de temps/coordonees
        self.date_restriction_debut = None
        self.date_restriction_fin = None
        self.heure_restriction_debut = None
        self.latitude_restriction_depart = None
        self.longitude_restriction_depart = None
        self.latitude_restriction_arrivee = None
        self.longitude_restriction_arrivee = None

        # Initialisation
        self.init()

    """
    Initialisation du routage
    """
    def init(self):
        
        #Configuration du menu
        self.app.file_routage_menu.entryconfig(0, command=self.openPolar)
        self.app.file_routage_menu.entryconfig(1, command=self.openIsoc)
        
        # Configuration des boutons de l'application
        self.app.route_button.config(command=self.activate)
        self.app.change_date_button.config(command=self.change_date)
        self.app.route_valid_button.config(command=self.validate)

    """
    Active l'outil de routage.
    """
    def activate(self):
        self.app.editbar.grid_remove()
        self.app.routingbar.grid(row=0, column=0,rowspan=2, sticky="nsew")
        self.app.canvas.bind("<ButtonPress-1>", self.get_coordinates)
        
        #Curseur
        self.app.canvas.config(cursor="target")
        
        #Ajout du bouton de déplacement pour quitter le routage
        self.app.move_button.grid(row=0, column=3, padx=(0, 10), ipadx=5, ipady=5)
        
        #Bouton de validation activé
        self.app.route_valid_button.config(state=tk.NORMAL)
        
        #Message d'erreur vide
        self.app.route_valid_message.config(text="")
        
        #Récupération des valeurs pour les restrictions de temps/coordonees
        path_to_isoc = Path(f"{Path(__file__).parent.absolute()}/routage/meteo/out.isoc")
        path_to_polar = Path(f"{Path(__file__).parent.absolute()}/routage/polaire/polar.dat")
        if(os.path.isfile(path_to_isoc)):
            if(os.path.isfile(path_to_polar)):
                path_to_isoc = str(path_to_isoc)
                
                file_isoc = open(path_to_isoc, "r")
                entete = file_isoc.readline()
                file_isoc.close()
                entete_split = entete.split()
                
                #Partie date et heure
                date_format = entete_split[1]
                pas = int(entete_split[0])
                delta = int(entete_split[3])
                start_hour = entete_split[2]
                nbhours = pas * delta
                
                #Restriction des dates du calendrier a la période couverte par le fichier .isoc
                self.date_restriction_debut = date_format[6:8] + "/" + date_format[4:6] + "/" + date_format[0:4]
                self.date_restriction_debut = datetime.strptime(self.date_restriction_debut, "%d/%m/%Y")
                self.app.calendrier_date_depart.config(mindate=self.date_restriction_debut)
                self.date_restriction_fin = self.date_restriction_debut + timedelta(hours=nbhours)
                self.app.calendrier_date_depart.config(maxdate=self.date_restriction_fin)
                
                #coordonnees
                self.latitude_restriction_depart = float(entete_split[6])
                self.longitude_restriction_depart = float(entete_split[7])
                nb_pas_latitude = int(entete_split[4])
                nb_pas_longitude = int(entete_split[5])
                pas_latitude = float(entete_split[8])
                pas_longitude = float(entete_split[9])
                
                self.latitude_restriction_arrivee = self.latitude_restriction_depart + pas_latitude*(nb_pas_latitude-1)
                self.longitude_restriction_arrivee = self.longitude_restriction_depart + pas_longitude*(nb_pas_longitude-1)
                
                #heure
                if len(start_hour) == 3:
                    start_hour = "0" + start_hour
                self.heure_restriction_debut = start_hour
            else:
                self.app.route_valid_button.config(state=tk.DISABLED)
                self.app.route_valid_message.config(text="Pas de fichier de polaire utilisé pour le routage")
        else :
            self.app.route_valid_button.config(state=tk.DISABLED)
            self.app.route_valid_message.config(text="Pas de fichier météo utilisé pour le routage")
        
    """
    Changement de la date de départ
    """
    def change_date(self):
        date = self.app.calendrier_date_depart.get_date()
        self.app.date_depart_value.config(text=date)
        
    """
    Récupération des coordonnées
    """
    def get_coordinates(self,event):
        coordonees = self.app.coordinates.cget("text")
        coordonees = coordonees.split()
        
        latitude = round(float(coordonees[2]),5)
        longitude = round(float(coordonees[5]),5)
        
        if self.nbclicks%2 == 0:
            self.app.latitude_depart_value.config(text=latitude)
            self.app.longitude_depart_value.config(text=longitude)
        else :
            self.app.latitude_arrivee_value.config(text=latitude)
            self.app.longitude_arrivee_value.config(text=longitude)
        
        self.nbclicks = self.nbclicks + 1
        
    """
    Validation et calcul du routage
    """
    def validate(self):
        if self.app.latitude_arrivee_value.cget("text") == "" or self.app.longitude_arrivee_value.cget("text") == "" :
            self.app.route_valid_message.config(text="Veuillez choisir des coordonées d'arrivée")
            if self.app.latitude_depart_value.cget("text") == "" or self.app.longitude_depart_value.cget("text") == "" :
                self.app.route_valid_message.config(text="Veuillez choisir des coordonées de départ")
                return
            return
        
        else :
            #Récupération des dates et heures de début du routage
            date = self.app.date_depart_value.cget("text")
            date_tab = date.split("/")
            date_formatee = date_tab[2]+date_tab[1]+date_tab[0]
            date_datetime = datetime.strptime(date, "%d/%m/%Y")
            
            heure = self.app.heure_depart_picker.hours()
            minutes = self.app.heure_depart_picker.minutes()
            
            if heure < 10 :
                heure = "0" + str(heure)
            else :
                heure = str(heure)
                
            if minutes < 10:
                minutes = "0" + str(minutes)
            else :
                minutes = str(minutes)
                
            
            time = heure+minutes+"00"
            
            #Vérification que les horaires choisis sont bien couverts par le fichier .isoc
            check_date_hour = True
            if self.date_restriction_debut == date_datetime:
                heure_debut = int(self.heure_restriction_debut[0:2])
                minutes_debut = int(self.heure_restriction_debut[2:4])
                if (heure_debut == int(heure) and int(minutes)) < minutes_debut or (int(heure) < heure_debut):
                    check_date_hour = False
                    
            if check_date_hour == True:
                
                check_coords = False
                
                #Récupération des coordonnées pour le routage
                latitude_depart = str(self.app.latitude_depart_value.cget("text"))
                longitude_depart = str(self.app.longitude_depart_value.cget("text"))
                latitude_arrivee = str(self.app.latitude_arrivee_value.cget("text"))
                longitude_arrivee = str(self.app.longitude_arrivee_value.cget("text"))
                
                #Vérification que les coordonnées choisies sont bien comprises dans les coordonnées couvertes par le fichier .isoc
                if self.latitude_restriction_depart <= float(latitude_depart) and self.latitude_restriction_arrivee >= float(latitude_depart):
                    if self.latitude_restriction_depart <= float(latitude_arrivee) and self.latitude_restriction_arrivee >= float(latitude_arrivee):
                        if self.longitude_restriction_depart <= float(longitude_depart) and self.longitude_restriction_arrivee >= float(longitude_depart):
                            if self.longitude_restriction_depart <= float(longitude_arrivee) and self.longitude_restriction_arrivee >= float(longitude_arrivee):
                                check_coords = True
                
                if check_coords == True:
                    
                    #Création du fichier de paramètres      
                    file_params = open("routage/params","w")
                    
                    lignes = ["seed 5 \n", "start  "+latitude_depart+" "+longitude_depart+"\n", "finish "+latitude_arrivee+" "+longitude_arrivee+"\n",
                              "date "+date_formatee+" \n", "stime "+time+"\n", "polar " + str(Path(f"{Path(__file__).parent.absolute()}/routage/polaire/polar.dat"))+" \n", 
                              "weather "+str(Path(f"{Path(__file__).parent.absolute()}/routage/meteo/out.isoc"))+"\n", 
                              "noise 0.0 \n", "gens 7000 \n", "alpha 100 \n", "depth 4 \n", "outputfile res.out"]
                    
                    file_params.writelines(lignes)
                    
                    file_params.close()
                    
                    #Appel du programme noisypaes
                    path_to_noisypaes = Path(f"{Path(__file__).parent.absolute()}/../../Routage/Routage/Sources/NOISY_PAES/noisypaes")
                    path_to_params = Path(f"{Path(__file__).parent.absolute()}/routage/params")
                    
                    cmd = str(path_to_noisypaes) + " "+ str(path_to_params)
                    
                    subprocess.call(cmd, shell=True)
                    
                    #Remplacement du fichier utilisé pour le routage
                    actual_res_path = Path(f"{Path(__file__).parent.absolute()}/best_iso.dat")
                    new_res_path = Path(f"{Path(__file__).parent.absolute()}/routage/res.out")
                    
                    if os.path.exists(new_res_path):
                            os.remove(new_res_path)
                            
                    shutil.copyfile(actual_res_path, new_res_path)
                    
                    #Réinitialisation du répertoire de résultats
                    remove_dir = Path(f"{Path(__file__).parent.absolute()}/routage/resultats")
                    files_res = os.listdir(remove_dir)
                    for file in files_res:
                        os.remove(Path(f"{Path(__file__).parent.absolute()}/routage/resultats/"+file))
                    os.rmdir(remove_dir)
                    os.mkdir(remove_dir)
                    
                    #Déplacement des fichiers de résultats dans le bon répertoire
                    res_files = ["all_path_and_iso.dat","array.dat","best_iso.dat","noisy_archive0.dat","noisyfree_front.dat","noisy_front.dat","noisy_iso.dat","noisy_isofront.dat","noisy_isojournal.dat","noisy_isojournalwithsegments.dat","noisy_ps.dat","pareto_set_journal.dat"]
                    
                    for file in res_files:
                        shutil.move(Path(f"{Path(__file__).parent.absolute()}/"+file), Path(f"{Path(__file__).parent.absolute()}/routage/resultats/"+file))
                    
                    #Lecture du fichier de résultat
                    res_path = Path(f"{Path(__file__).parent.absolute()}/routage/res.out")
                    file1 = open(res_path, 'r')
                    self.Lines = file1.readlines()
                    file1.close()
                    
                    #Dessin des routes
                    self.drawPath()
                    
                    #Fin du routage
                    self.deactivate()
                    
                else :
                    message_coords_erreur = "Les coordonées choisies ne sont pas couvertes,\n latitude " + str(self.latitude_restriction_depart) + " jusqu'a " + str(self.latitude_restriction_arrivee) + "\n longitude " + str(self.longitude_restriction_depart) + " jusqu'a " + str(self.longitude_restriction_arrivee)
                    self.app.route_valid_message.config(text=message_coords_erreur)
        
            else :
                message_date_erreur = "L'heure choisie n'est pas couverte, début à partir de " + self.heure_restriction_debut[0:2]+"h"+self.heure_restriction_debut[2:4]
                self.app.route_valid_message.config(text=message_date_erreur)
            
    """
    Enregistrement d'un fichier de polaire de vitesse XML pour cartographie puis changement en fichier .dat
    """
    def openPolar(self):
        # Navigateur de fichiers
        file_types = [("XML file ", ".xml")]
        file_name = fd.askopenfilename(title="Ouvrir un fichier XML (polaire)", initialdir="/", filetypes=file_types)

        # On ouvre le fichier
        if file_name:
            remove_dir = Path(f"{Path(__file__).parent.absolute()}/routage/polaire")
            files_polar = os.listdir(remove_dir)
            for file in files_polar:
                os.remove(Path(f"{Path(__file__).parent.absolute()}/routage/polaire/"+file))
            os.rmdir(remove_dir)
            os.mkdir(remove_dir)

            file_path = Path(f"{Path(__file__).parent.absolute()}/routage/polaire/polaire.xml")
            shutil.copy(file_name, file_path)
            
            cmd = "./../../Routage/polar/xmlToDat "+str(file_path)
            subprocess.call(cmd, shell=True)
            actual_polar_path = Path(f"{Path(__file__).parent.absolute()}/polar.dat")
            new_polar_path = Path(f"{Path(__file__).parent.absolute()}/routage/polaire/")
            shutil.move(actual_polar_path, new_polar_path)
            
    """
    Enregistrement d'un fichier .isoc pour les données météo
    """
    def openIsoc(self):
        # Navigateur de fichiers
        file_types = [("Isoc file ", ".isoc")]
        file_name = fd.askopenfilename(title="Enregistrer un fichier isoc", initialdir="/", filetypes=file_types)

        # On ouvre le fichier
        if file_name:
            remove_dir = Path(f"{Path(__file__).parent.absolute()}/routage/meteo")
            files_polar = os.listdir(remove_dir)
            for file in files_polar:
                os.remove(Path(f"{Path(__file__).parent.absolute()}/routage/meteo/"+file))
            os.rmdir(remove_dir)
            os.mkdir(remove_dir)

            file_path = Path(f"{Path(__file__).parent.absolute()}/routage/meteo/out.isoc")
            shutil.copy(file_name, file_path)        
            
    """
    Dessin des chemins générés par le routage
    """
    def drawPath(self):        
        pathNumber = 0
        all_pathes = []
        path = []
        #Ajout des routes au tableau de routes
        for line in self.Lines:
            if line.strip():
                if line[0] != "#":
                    tabLine = line.split(" ")
                    (x,y) = tools.latlon_to_pixels(self.map.tiles_size, self.map.zoom_level, float(tabLine[8]), float(tabLine[9]))
                    path.append((x,y))
                else :
                    if len(path) != 0:
                        all_pathes.append(path)
                        path = []    
                        
        for path in all_pathes:
            pathName = "path"+str(pathNumber)
            
            if(self.nbPathes < len(all_pathes)):
                self.colors.append(self.random_color())
                
            self.app.canvas.create_line(path, tag = pathName, fill = self.colors[pathNumber])
            pathNumber += 1  
            
        self.nbPathes = pathNumber + 1
                
    
    """
    Effacement des chemins générés par le routage
    """
    def removePathes(self):
        for i in range(self.nbPathes):
            path = "path"+str(i)
            self.app.canvas.delete(path)
            
            
    """
    Fonction pour mettre fin au routage
    """
    def deactivate(self):
        self.app.latitude_depart_value.config(text="")
        self.app.longitude_depart_value.config(text="")
        self.app.latitude_arrivee_value.config(text="")
        self.app.longitude_arrivee_value.config(text="")
                    
        self.app.routingbar.grid_remove()
        
        self.map.move.activate()
        
    """
    Génère une couleur aléatoire 
    """
    def random_color(self):
        color="#"+''.join([random.choice('0123456789ABCDEF') for i in range(6)])
        return color
        