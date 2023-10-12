from platform import system
if system() == "Windows":
    from os import add_dll_directory
from pathlib import Path
from ctypes import CDLL, c_int, c_double, c_char_p

class Gribwind:
    def __init__(self):

        # Bibliothèque partagée
        self.libc = None

    """
    Chargement de la bibliothèque partagée en fonction du système.
    """
    def setup(self):
        api_path = Path(__file__).parent.absolute()
        if system() == "Windows":
            with open(api_path / "eccodes_dir.txt") as file:
                add_dll_directory(Path(file.readline()) / "bin")
                add_dll_directory(api_path)
            self.libc = CDLL("libgribwind.dll")
        else:
            self.libc = CDLL(api_path / "libgribwind.so")

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis remplace la valeur de la composante u du vent par "u_value".

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param u_value: la nouvelle valeur de la composante u du vent

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_u(self, filename, date, time, lat, lon, u_value):
        self.libc.set_wind_u.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_u.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_u(b_filename, b_date, b_time, lat, lon, u_value)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis remplace la valeur de la composante v du vent par "v_value".

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param v_value: la nouvelle valeur de la composante v du vent

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_v(self, filename, date, time, lat, lon, v_value):
        self.libc.set_wind_v.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_v.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_v(b_filename, b_date, b_time, lat, lon, v_value)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis remplace la valeur de la direction du vent par "value".

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param value: la nouvelle valeur de la direction du vent en degrés

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_dir(self, filename, date, time, lat, lon, value):
        self.libc.set_wind_dir.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_dir.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_dir(b_filename, b_date, b_time, lat, lon, value)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis ajoute "offset" à la direction du vent.

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param offset: le décalage à ajouter à la direction du vent en degrés

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_dir_offset(self, filename, date, time, lat, lon, offset):
        self.libc.set_wind_dir_offset.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_dir_offset.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_dir_offset(b_filename, b_date, b_time, lat, lon, offset)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis remplace la valeur de la vitesse du vent par "value".

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param value: la nouvelle valeur de la vitesse du vent en noeuds

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_speed(self, filename, date, time, lat, lon, value):
        self.libc.set_wind_speed.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_speed.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_speed(b_filename, b_date, b_time, lat, lon, value)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis ajoute "offset" à la vitesse du vent.

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param offset: le décalage à ajouter à la vitesse du vent en noeuds

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_speed_offset(self, filename, date, time, lat, lon, offset):
        self.libc.set_wind_speed_offset.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_speed_offset.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_speed_offset(b_filename, b_date, b_time, lat, lon, offset)

    """
    Récupère le point de grille le plus proche de la latitude et de la longitude définies
    puis ajoute "percentage" à la vitesse du vent.

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param lat: la latitude du point de grille
    @param lon: la longitude du point de grille
    @param percentage: le décalage à ajouter à la vitesse du vent en pourcentage

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_speed_percentage(self, filename, date, time, lat, lon, percentage):
        self.libc.set_wind_speed_percentage.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double]
        self.libc.set_wind_speed_percentage.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_speed_percentage(b_filename, b_date, b_time, lat, lon, percentage)

    """
    Parcourt tous les points de grille dans une zone définie et applique une modification.

    @param filename: le chemin vers le fichier GRIB
    @param date: la date au format "dd/mm/yyyy"
    @param time: l'heure au format "hh:mm"
    @param min_lat: la latitude minimale de la zone
    @param min_lon: la longitude minimale de la zone
    @param max_lat: la latitude maximale de la zone
    @param max_lon: la longitude maximale de la zone
    @param val_type: le type de modification
    - 0 pour modifier la direction du vent en degrés
    - 1 pour modifier la vitesse du vent en noeuds
    - 2 pour décaler la vitesse du vent en pourcentage
    @param is_offset: 0 si on applique un remplacement ou 1 si on applique un décalage
    @param value: la nouvelle valeur

    @return: 0 si OK, sinon code d'erreur
    """
    def set_wind_area_generic(self, filename, date, time, min_lat, min_lon, max_lat, max_lon, val_type, is_offset, value):
        self.libc.set_wind_area_generic.argtypes = [c_char_p, c_char_p, c_char_p, c_double, c_double, c_double, c_double, c_int, c_int, c_double]
        self.libc.set_wind_area_generic.restype = c_int
        b_filename = filename.encode()
        b_date = date.encode()
        b_time = time.encode()
        return self.libc.set_wind_area_generic(b_filename, b_date, b_time, min_lat, min_lon, max_lat, max_lon, val_type, is_offset, value)
