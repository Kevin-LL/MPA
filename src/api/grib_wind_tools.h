#ifndef grib_wind_tools_H
#define grib_wind_tools_H

#include <eccodes.h>
#define PI 3.14159265358979323846
#define KTS2MPS 0.51444444444444444444

/**
 * Convertit une date au format "dd/mm/yyyy" en long "yyyymmdd".
 * Ce format correspond à celui utilisé dans les fichiers GRIB.
 *
 * @param date        : la date au format "dd/mm/yyyy"
 * @return            la date en long "yyyymmdd"
 */
long convert_date_format(char *date);

/**
 * Convertit une heure au format "hh:mm" en long "hhmm".
 * Ce format correspond à celui utilisé dans les fichiers GRIB.
 * On notera qu'une heure au format "hh:mm:ss" est aussi acceptée mais les
 * secondes seront ignorées, car non utilisées dans les fichiers GRIB.
 *
 * @param time        : l'heure au format "hh:mm"
 * @return            l'heure en long "hhmm"
 */
long convert_time_format(char *time);

/**
 * Récupère un message précis à partir d'une date et d'une heure.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "yyyymmdd"
 * @param time        : l'heure au format "hhmm"
 * @param param       : "u" si on recherche un message contenant la composante u, sinon "v"
 * @return            l'handle d'un message
 */
codes_handle *get_handle_from_date(char *filename, long date, long time, char param);

/**
 * Prend une latitude et une longitude et renvoie le point le plus proche dans un tableau.
 * Le tableau est renvoyé au format "[latitude, longitude, valeur]".
 * Cette fonction permet de simplifier "codes_grib_nearest_find" d'ecCodes.
 *
 * @param h           : l'handle d'un message
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param res         : le point le plus proche au format "[latitude, longitude, valeur]"
 * @return            0 si OK, sinon code d'erreur
 */
int get_nearest_point(codes_handle *h, double lat, double lon, double *res);

/**
 * Modifie une valeur dans un message précis à partir de diverses informations.
 *
 * @param filename    : le chemin vers le fichier GRIB
 * @param date        : la date au format "yyyymmdd"
 * @param time        : l'heure au format "hhmm"
 * @param param       : "u" si on recherche un message contenant la composante u, sinon "v"
 * @param lat         : la latitude du point de grille
 * @param lon         : la longitude du point de grille
 * @param value       : la nouvelle valeur
 * @return            0 si OK, sinon code d'erreur
 */
int set_wind(char *filename, long date, long time, char param, double lat, double lon, double value);

/**
 * Calcule la vitesse du vent à partir des composantes u et v.
 *
 * @param u           : direction ouest/est (positif depuis l'ouest et négatif depuis l'est)
 * @param v           : direction sud/nord (positif depuis le sud et négatif depuis le nord)
 * @return            la vitesse du vent en mètres par seconde
 */
double calcul_wind_speed(double u, double v);

/**
 * Calcule la direction du vent à partir des composantes u et v.
 *
 * @param u           : direction ouest/est (positif depuis l'ouest et négatif depuis l'est)
 * @param v           : direction sud/nord (positif depuis le sud et négatif depuis le nord)
 * @return            la direction du vent en degrés
 */
double calcul_wind_angle(double u, double v);

/**
 * Transforme une valeur en noeuds en valeur en mètres par seconde.
 *
 * @param value       : la valeur en noeuds
 * @return            la valeur en mètres par seconde
 */
double knots_to_mps(double value);

#endif
