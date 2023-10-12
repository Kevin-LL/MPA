#ifndef grib_wind_H
#define grib_wind_H

/*
 * Enumeration qui définie les modifications possibles sur le vent
 * 0 = VAL_ANGLE
 * 1 = VAL_SPEED
 * 2 = VAL_POURC
 */
typedef enum val_type
{
	VAL_ANGLE,
	VAL_SPEED,
	VAL_POURC
} val_type_t;

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis remplace la valeur de la composante u du vent par "u_value".
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param u_value     : la nouvelle valeur de la composante u du vent
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_u(char *filename, char *date, char *time, double lat, double lon, double u_value);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis remplace la valeur de la composante v du vent par "v_value".
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param v_value     : la nouvelle valeur de la composante v du vent
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_v(char *filename, char *date, char *time, double lat, double lon, double v_value);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis remplace la valeur de la direction du vent par "value".
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param value       : la nouvelle valeur de la direction du vent en degrés
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_dir(char *filename, char *date, char *time, double lat, double lon, double value);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis ajoute "offset" à la direction du vent.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param offset      : le décalage à ajouter à la direction du vent en degrés
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_dir_offset(char *filename, char *date, char *time, double lat, double lon, double offset);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis remplace la valeur de la vitesse du vent par "value".
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param value       : la nouvelle valeur de la vitesse du vent en noeuds
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_speed(char *filename, char *date, char *time, double lat, double lon, double value);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis ajoute "offset" à la vitesse du vent.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param offset      : le décalage à ajouter à la vitesse du vent en noeuds
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_speed_offset(char *filename, char *date, char *time, double lat, double lon, double offset);

/**
 * Récupère le point de grille le plus proche de la latitude et de la longitude définies
 * puis ajoute "percentage" à la vitesse du vent.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param percentage  : le décalage à ajouter à la vitesse du vent en pourcentage
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_speed_percentage(char *filename, char *date, char *time, double lat, double lon, double percentage);

/**
 * Parcourt tous les points de grille dans une zone définie et applique une modification.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "dd/mm/yyyy"
 * @param time        : l'heure au format "hh:mm"
 * @param min_lat     : la latitude minimale de la zone
 * @param min_lon     : la longitude minimale de la zone
 * @param max_lat     : la latitude maximale de la zone
 * @param max_lon     : la longitude maximale de la zone
 * @param val_type    : le type de modification
 * - 0 pour modifier la direction du vent en degrés
 * - 1 pour modifier la vitesse du vent en noeuds
 * - 2 pour décaler la vitesse du vent en pourcentage
 * @param is_offset   : 0 si on applique un remplacement ou 1 si on applique un décalage
 * @param value       : la nouvelle valeur
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind_area_generic(char *filename, char *date, char *time, double min_lat, double min_lon, double max_lat, double max_lon, val_type_t val_type, int is_offset, double value);

#endif
