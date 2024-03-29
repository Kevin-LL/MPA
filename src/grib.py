import math
import xarray as xr
import numpy as np
from . import tools
from threading import Thread, Event
from pathlib import Path
from PIL import Image, ImageTk
from tkinter import filedialog as fd

class Grib:
    def __init__(self, map):

        # Application et carte
        self.app = map.app
        self.map = map

        # Attributs
        self.file_path = None
        self.data_time = None
        self.data_step = None
        self.data_u = None
        self.data_v = None
        self.data_latitudes = None
        self.data_longitudes = None
        self.barbs_ref = {}
        self.barbs_size = 64
        self.step = 0
        self.thread = Thread(target=self.draw_barbs, daemon=True)
        self.thread_run = Event()

        # Initialisation
        self.configuration()

        # Évènements
        self.app.timeline.bind("<ButtonRelease-1>", self.update_step)
        self.app.timeline.bind("<ButtonRelease-2>", self.update_step)
        self.app.timeline.bind("<ButtonRelease-3>", self.update_step)

    """
    Configuration de la classe.
    """
    def configuration(self):
        # Menu
        self.app.file_menu.entryconfig(0, command=self.open)
        self.app.file_menu.entryconfig(2, command=self.close)

        # Barre d'outils
        self.app.grib_button.config(command=self.open)

        # Ligne temporelle
        self.app.timeline.config(command=self.update_datetime)

    """
    Active les fonctionnalités du fichier GRIB.
    """
    def activate(self):
        # Application
        self.app.title(self.file_path.name)
        self.app.protocol("WM_DELETE_WINDOW", self.quit)

        # Menu
        self.app.file_menu.entryconfig(2, state="normal")
        self.app.file_menu.entryconfig(4, command=self.quit)

        # Barre d'outils
        grib_icon_open = Image.open(Path(__file__).parent.absolute() / "images" / "icons" / "grib_close.png").resize((32,32))
        self.app.grib_icon_image = ImageTk.PhotoImage(grib_icon_open)
        self.app.grib_button.config(command=self.close, image=self.app.grib_icon_image)
        self.app.save_button.grid(row=0, column=2, padx=(0, 10), ipadx=5, ipady=5)
        self.app.save_as_button.grid(row=0, column=3, padx=(0, 10), ipadx=5, ipady=5)
        self.app.move_button.grid(row=0, column=4, padx=(0, 10), ipadx=5, ipady=5)
        self.app.edit_button.grid(row=0, column=5, padx=(0, 10), ipadx=5, ipady=5)
        self.app.undo_button.grid(row=0, column=6, padx=(0, 10), ipadx=5, ipady=5)
        self.app.redo_button.grid(row=0, column=7, padx=(0, 10), ipadx=5, ipady=5)
        self.app.history_button.grid(row=0, column=8, ipadx=5, ipady=5)

        # Date et heure
        self.app.datetime.grid(row=0, column=1, sticky="nw", padx=(10, 0), pady=(10, 0), ipadx=5, ipady=5)
        self.update_datetime(0)

        # Ligne temporelle
        self.app.timeline.grid(row=2, column=0, sticky="ew")
        self.app.timeline.config(from_=0, to=self.data_step.size - 1)

    """
    Désactive les fonctionnalités du fichier GRIB.
    """
    def deactivate(self):
        # Application
        self.app.title("Éditeur de vents marins")
        self.app.protocol("WM_DELETE_WINDOW", self.app.destroy)

        # Menu
        self.app.file_menu.entryconfig(2, state="disabled")
        self.app.file_menu.entryconfig(4, command=self.app.destroy)

        # Barre d'outils
        grib_icon_open = Image.open(Path(__file__).parent.absolute() / "images" / "icons" / "grib_open.png").resize((32,32))
        self.app.grib_icon_image = ImageTk.PhotoImage(grib_icon_open)
        self.app.grib_button.config(command=self.open, image=self.app.grib_icon_image)
        self.app.save_button.grid_remove()
        self.app.save_as_button.grid_remove()
        self.app.move_button.grid_remove()
        self.app.edit_button.grid_remove()
        self.app.undo_button.grid_remove()
        self.app.redo_button.grid_remove()
        self.app.history_button.grid_remove()
        self.map.move.activate()

        # Barre d'édition
        self.app.min_latitude_value.config(text="")
        self.app.min_longitude_value.config(text="")
        self.app.max_latitude_value.config(text="")
        self.app.max_longitude_value.config(text="")
        self.app.edit_input_value.config(bg="#ffffff")
        self.app.edit_input_value.delete(0, "end")
        self.app.canvas.delete("edit")

        # Date et heure
        self.app.datetime.grid_remove()

        # Barre d'historique
        self.map.history.deactivate()

        # Ligne temporelle
        self.app.timeline.grid_remove()
        self.app.timeline.set(0)
        self.step = 0

    """
    Récupère les informations du fichier GRIB.

    @param file_path: chemin vers le fichier GRIB
    """
    def get_data(self, file_path):
        self.file_path = Path(file_path)
        grib_file = xr.load_dataset(file_path, engine="cfgrib", backend_kwargs={"indexpath": ""})
        self.data_time = grib_file.time.values
        self.data_step = grib_file.step.values
        if hasattr(grib_file, "u") and hasattr(grib_file, "v"):
            self.data_u = grib_file.u.values
            self.data_v = grib_file.v.values
        if hasattr(grib_file, "u10") and hasattr(grib_file, "v10"):
            self.data_u = grib_file.u10.values
            self.data_v = grib_file.v10.values
        self.data_latitudes = grib_file.latitude.values
        self.data_longitudes = grib_file.longitude.values

    """
    Réinitialise les informations du fichier GRIB.
    """
    def reset_data(self):
        self.file_path = None
        self.data_time = None
        self.data_step = None
        self.data_u = None
        self.data_v = None
        self.data_latitudes = None
        self.data_longitudes = None

    """
    Ouvre le fichier GRIB.

    @param file_path: chemin vers le fichier GRIB
    """
    def open(self, file_path = None):
        if file_path is None:
            # Navigateur de fichiers
            file_types = [("GRIB file", ".gb .grb .grib .gb2 .grb2 .grib2")]
            file_path = fd.askopenfilename(title="Ouvrir un fichier GRIB", filetypes=file_types)

        if file_path and self.close() is not None:
            # Obtention des informations du fichier
            self.get_data(file_path)

            # Historisation
            self.map.history.set()

            # Activation des fonctionnalités du fichier
            self.activate()

            # Démarrage du thread qui dessine les barbules
            if not self.thread.is_alive():
                self.thread_run.set()
                self.thread = Thread(target=self.draw_barbs, daemon=True)
                self.thread.start()

    """
    Ferme le fichier GRIB en fonction d'une popup de sauvegarde.

    @return: True si "Oui", False si "Non", None si "Annuler"
    """
    def close(self):
        # Obtention de la réponse de la popup
        answer = self.map.history.save_popup()

        if answer is not None:
            # Fermeture du thread qui dessine les barbules
            self.thread_run.clear()
            tools.wait(self.app, self.thread)

            # Réinitialisation des informations du fichier
            self.reset_data()

            # Historisation
            self.map.history.unset()

            # On désactive les fonctionnalités du fichier
            self.deactivate()

            # Suppression des images
            self.app.canvas.delete("barb")
            self.barbs_ref = {}

        return answer

    """
    Ferme le fichier GRIB et l'application en fonction d'une popup de sauvegarde.

    @return: True si "Oui", False si "Non", None si "Annuler"
    """
    def quit(self):
        # Obtention de la réponse de la popup
        answer = self.map.history.save_popup()

        if answer is not None:
            # Fermeture du thread qui dessine les barbules
            self.thread_run.clear()
            tools.wait(self.app, self.thread)

            # Fermeture de l'application
            self.app.destroy()

        return answer

    """
    Charge l'image d'une barbule via un chemin donné par le paramètre speed,
    on ajoute ensuite la barbule le canevas tout en conservant ses informations dans un dictionnaire.

    @param barb_bbox: boîte englobante de l'image
    @param size: taille de l'image
    @param speed: vitesse du vent en noeud
    @param direction: direction du vent en degré
    """
    def load_barb(self, barb_bbox, size, speed, direction):
        barb_path = Path(__file__).parent.absolute() / "images" / "barbs" / f"{speed}.png"

        # On vérifie si l'image existe avant d'ouvrir le fichier
        if barb_path.exists():
            barb_open = Image.open(barb_path)

            # Objet image
            barb_image = ImageTk.PhotoImage(barb_open.resize((size, size)).rotate(direction, expand=1))

            # Ajout de la barbule sur le canevas
            barb = self.app.canvas.create_image(barb_bbox[0] + size / 2, barb_bbox[1] + size / 2, image=barb_image, tag="barb")

            # Sauvegarde des informations de la barbule, on inclut l'objet image pour qu'il ne soit pas supprimé par le garbage collector
            self.barbs_ref[barb_bbox] = (barb, barb_image)

    """
    Dessine à partir du fichier GRIB toutes les barbules dans la zone visible.
    """
    def draw_barbs(self):
        while self.thread_run.is_set():
            # Gestion de la fréquence d'affichage des barbules
            data_avg_size = self.data_latitudes.size + self.data_longitudes.size / 2
            data_avg_spacing = np.mean(np.diff(np.sort(self.data_latitudes))) + np.mean(np.diff(np.sort(self.data_longitudes))) / 2
            data_frequency = data_avg_size / self.map.max_zoom_level / self.map.zoom_level / 2
            if data_avg_spacing < 1:
                data_frequency_max = data_avg_size / self.map.max_zoom_level / self.map.max_zoom_level / 2 / data_avg_spacing
                data_frequency = data_frequency / data_avg_spacing - data_frequency_max
            if data_frequency < 1:
                data_frequency = 1
            data_frequency = round(data_frequency)

            # Obtention de l'intervalle des données des latitudes et longitudes pour la zone visible
            visible_bbox = tools.get_visible_bbox(self.app.canvas, self.map.zoom_level)
            data_range = tools.get_data_range(visible_bbox, self.map.tiles_size, self.map.zoom_level, data_frequency, self.data_latitudes, self.data_longitudes)

            # Pour chaque indice visible on charge l'image associée selon une fréquence
            for index_latitude in range(data_range[0], data_range[1], data_frequency):
                for index_longitude in range(data_range[2], data_range[3], data_frequency):
                    if self.thread_run.is_set():
                        u = self.data_u[self.step][index_latitude][index_longitude]
                        v = self.data_v[self.step][index_latitude][index_longitude]

                        # On vérifie si les composantes u et v sont numériques
                        if not np.isnan(u) and not np.isnan(v):
                            latitude = self.data_latitudes[index_latitude]
                            longitude = self.data_longitudes[index_longitude]
                            x, y = tools.latlon_to_pixels(self.map.tiles_size, self.map.zoom_level, latitude, longitude)

                            # Gestion de la taille de l'image
                            size = self.barbs_size / data_frequency
                            if data_avg_spacing < 1:
                                size = size / self.map.max_zoom_level * self.map.zoom_level * data_frequency
                            if size < 1:
                                size = 1
                            size = round(size)

                            # Obtention de la boîte englobante de l'image
                            x1, y1 = x - size / 2, y - size / 2
                            x2, y2 = x1 + size, y1 + size
                            z = self.map.zoom_level
                            barb_bbox = (x1, y1, x2, y2, z)

                            if barb_bbox not in self.barbs_ref and tools.is_intersection(visible_bbox, barb_bbox):
                                # Définition de la direction
                                direction = tools.wind_uv_to_direction(u, v)

                                # Définition de la vitesse
                                speed_in_mps = tools.wind_uv_to_speed(u, v)
                                speed_in_knots = tools.mps_to_knots(speed_in_mps)
                                speed = 5 * round(speed_in_knots / 5) # Arrondi à un multiple de 5
                                if speed > 150:
                                    speed = 150

                                self.load_barb(barb_bbox, size, speed, -direction)

    """
    Met à jour les images des barbules dans une zone définie.

    @param step: pas de la ligne temporelle
    @param min_latitude: latitude minimale de la zone
    @param min_longitude: longitude minimale de la zone
    @param max_latitude: latitude maximale de la zone
    @param max_longitude: longitude maximale de la zone
    @param type: type de modification
    @param is_offset: True si ajout, False si remplacement
    @param input_value: valeur en entrée pour la modification
    """
    def update_barbs(self, step, min_latitude, min_longitude, max_latitude, max_longitude, type, is_offset, input_value):
        # On parcourt les données des latitudes et longitudes
        for index_latitude, latitude in enumerate(self.data_latitudes):
            for index_longitude, longitude in enumerate(self.data_longitudes):

                # On vérifie si on se trouve dans la zone d'édition
                if latitude >= min_latitude and latitude <= max_latitude and longitude >= min_longitude and longitude <= max_longitude:
                    u = self.data_u[step][index_latitude][index_longitude]
                    v = self.data_v[step][index_latitude][index_longitude]
                    speed_in_mps = tools.wind_uv_to_speed(u, v)

                    # Type de modification
                    match type:
                        case 0:
                            if is_offset:
                                new_angle = math.atan2(v, u) - float(input_value) * math.pi / 180
                            else:
                                new_angle = (270 - float(input_value)) * math.pi / 180
                            self.data_u[step][index_latitude][index_longitude] = speed_in_mps * math.cos(new_angle)
                            self.data_v[step][index_latitude][index_longitude] = speed_in_mps * math.sin(new_angle)
                        case 1:
                            if is_offset:
                                new_speed = tools.knots_to_mps(speed_in_mps + float(input_value))
                            else:
                                new_speed = tools.knots_to_mps(float(input_value))
                            if new_speed <= 0:
                                new_speed = tools.knots_to_mps(1)
                            self.data_u[step][index_latitude][index_longitude] *= new_speed / speed_in_mps
                            self.data_v[step][index_latitude][index_longitude] *= new_speed / speed_in_mps
                        case 2:
                            if is_offset:
                                new_speed = speed_in_mps + (speed_in_mps * float(input_value) / 100)
                                if new_speed <= 0:
                                    new_speed = tools.knots_to_mps(1)
                                self.data_u[step][index_latitude][index_longitude] *= new_speed / speed_in_mps
                                self.data_v[step][index_latitude][index_longitude] *= new_speed / speed_in_mps

        # Suppression des images
        self.app.canvas.delete("barb")
        self.barbs_ref = {}

    """
    Met à jour la date et l'heure en fonction du défilement de la ligne temporelle.

    @param value: pas de la ligne temporelle
    """
    def update_datetime(self, value):
        date, time = tools.datetime64_to_params(self.data_time + self.data_step[int(value)])
        self.app.datetime.config(text=f"{date} {time}")

    """
    Met à jour le pas après un relâchement du clique gauche sur la ligne temporelle.
    """
    def update_step(self, event):
        if self.step != self.app.timeline_step.get():
            self.step = self.app.timeline_step.get()

            # Suppression des images
            self.app.canvas.delete("barb")
            self.barbs_ref = {}
