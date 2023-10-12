#include "grib_wind.h"
#include "grib_wind_tools.h"

/*
 * Fonctions principales
 */

int set_wind_u(char *filename, char *date, char *time, double lat, double lon, double u_value)
{
	codes_handle *h = NULL;
	long ldate, ltime;
	double nearest[3] = {0,};

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention du message contenant u */
	h = get_handle_from_date(filename, ldate, ltime, 'u');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(h, lat, lon, nearest);

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest[0], nearest[1], u_value);

	/* Libération de la mémoire */
	codes_handle_delete(h);

	return 0;
}

int set_wind_v(char *filename, char *date, char *time, double lat, double lon, double v_value)
{
	codes_handle *h = NULL;
	long ldate, ltime;
	double nearest[3] = {0,};

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention du message contenant v */
	h = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(h, lat, lon, nearest);

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'v', nearest[0], nearest[1], v_value);

	/* Libération de la mémoire */
	codes_handle_delete(h);

	return 0;
}

int set_wind_dir(char *filename, char *date, char *time, double lat, double lon, double value)
{
	codes_handle *hu = NULL, *hv = NULL;
	long ldate, ltime;
	double nearest_u[3] = {0,};
	double nearest_v[3] = {0,};
	double speed, new_angle;

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention des messages contenant u et v */
	hu = get_handle_from_date(filename, ldate, ltime, 'u');
	hv = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(hu, lat, lon, nearest_u);
	get_nearest_point(hv, lat, lon, nearest_v);

	/* On récupère la vitesse du vent et on applique les modifications */
	speed = calcul_wind_speed(nearest_u[2], nearest_v[2]);
	new_angle = (270 - value) * PI / 180;
	nearest_u[2] = speed * cos(new_angle);
	nearest_v[2] = speed * sin(new_angle);

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest_u[0], nearest_u[1], nearest_u[2]);
	set_wind(filename, ldate, ltime, 'v', nearest_v[0], nearest_v[1], nearest_v[2]);

	/* Libération de la mémoire */
	codes_handle_delete(hu);
	codes_handle_delete(hv);

	return 0;
}

int set_wind_dir_offset(char *filename, char *date, char *time, double lat, double lon, double offset)
{
	codes_handle *hu = NULL, *hv = NULL;
	long ldate, ltime;
	double nearest_u[3] = {0,};
	double nearest_v[3] = {0,};
	double speed, new_angle;

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention des messages contenant u et v */
	hu = get_handle_from_date(filename, ldate, ltime, 'u');
	hv = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(hu, lat, lon, nearest_u);
	get_nearest_point(hv, lat, lon, nearest_v);

	/* On récupère la vitesse du vent et on applique les modifications */
	speed = calcul_wind_speed(nearest_u[2], nearest_v[2]);
	new_angle = atan2(nearest_v[2], nearest_u[2]) - offset * PI / 180;
	nearest_u[2] = speed * cos(new_angle);
	nearest_v[2] = speed * sin(new_angle);

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest_u[0], nearest_u[1], nearest_u[2]);
	set_wind(filename, ldate, ltime, 'v', nearest_v[0], nearest_v[1], nearest_v[2]);

	/* Libération de la mémoire */
	codes_handle_delete(hu);
	codes_handle_delete(hv);

	return 0;
}

int set_wind_speed(char *filename, char *date, char *time, double lat, double lon, double value)
{
	codes_handle *hu = NULL, *hv = NULL;
	long ldate, ltime;
	double nearest_u[3] = {0,};
	double nearest_v[3] = {0,};
	double speed, new_speed;

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention des messages contenant u et v */
	hu = get_handle_from_date(filename, ldate, ltime, 'u');
	hv = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(hu, lat, lon, nearest_u);
	get_nearest_point(hv, lat, lon, nearest_v);

	/* On récupère la vitesse du vent et on applique les modifications */
	speed = calcul_wind_speed(nearest_u[2], nearest_v[2]);
	new_speed = knots_to_mps(value);
	if (new_speed <= 0)
	{
		new_speed = knots_to_mps(1);
	}
	nearest_u[2] *= new_speed / speed;
	nearest_v[2] *= new_speed / speed;

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest_u[0], nearest_u[1], nearest_u[2]);
	set_wind(filename, ldate, ltime, 'v', nearest_v[0], nearest_v[1], nearest_v[2]);

	/* Libération de la mémoire */
	codes_handle_delete(hu);
	codes_handle_delete(hv);

	return 0;
}

int set_wind_speed_offset(char *filename, char *date, char *time, double lat, double lon, double offset)
{
	codes_handle *hu = NULL, *hv = NULL;
	long ldate, ltime;
	double nearest_u[3] = {0,};
	double nearest_v[3] = {0,};
	double speed, new_speed;

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention des messages contenant u et v */
	hu = get_handle_from_date(filename, ldate, ltime, 'u');
	hv = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(hu, lat, lon, nearest_u);
	get_nearest_point(hv, lat, lon, nearest_v);

	/* On récupère la vitesse du vent et on applique les modifications */
	speed = calcul_wind_speed(nearest_u[2], nearest_v[2]);
	new_speed = knots_to_mps(speed + offset);
	if (new_speed <= 0)
	{
		new_speed = knots_to_mps(1);
	}
	nearest_u[2] *= new_speed / speed;
	nearest_v[2] *= new_speed / speed;

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest_u[0], nearest_u[1], nearest_u[2]);
	set_wind(filename, ldate, ltime, 'v', nearest_v[0], nearest_v[1], nearest_v[2]);

	/* Libération de la mémoire */
	codes_handle_delete(hu);
	codes_handle_delete(hv);

	return 0;
}

int set_wind_speed_percentage(char *filename, char *date, char *time, double lat, double lon, double percentage)
{
	codes_handle *hu = NULL, *hv = NULL;
	long ldate, ltime;
	double nearest_u[3] = {0,};
	double nearest_v[3] = {0,};
	double speed, new_speed;

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Obtention des messages contenant u et v */
	hu = get_handle_from_date(filename, ldate, ltime, 'u');
	hv = get_handle_from_date(filename, ldate, ltime, 'v');

	/* Obtention du point de grille le plus proche */
	get_nearest_point(hu, lat, lon, nearest_u);
	get_nearest_point(hv, lat, lon, nearest_v);

	/* On récupère la vitesse du vent et on applique les modifications */
	speed = calcul_wind_speed(nearest_u[2], nearest_v[2]);
	new_speed = speed + (speed * percentage / 100);
	if (new_speed <= 0)
	{
		new_speed = knots_to_mps(1);
	}
	nearest_u[2] *= new_speed / speed;
	nearest_v[2] *= new_speed / speed;

	/* On enregistre la modification dans le fichier */
	set_wind(filename, ldate, ltime, 'u', nearest_u[0], nearest_u[1], nearest_u[2]);
	set_wind(filename, ldate, ltime, 'v', nearest_v[0], nearest_v[1], nearest_v[2]);

	/* Libération de la mémoire */
	codes_handle_delete(hu);
	codes_handle_delete(hv);

	return 0;
}

int set_wind_area_generic(char *filename, char *date, char *time, double min_lat, double min_lon, double max_lat, double max_lon, val_type_t val_type, int is_offset, double value)
{
	int err = 0;
	FILE *in = NULL, *out = NULL;
	codes_multi_handle *mh = NULL;
	codes_handle* h = NULL, *hu = NULL, *hv = NULL;
	long ldate, ltime, mdate, mtime;
	size_t i = 0, values_len = 0;
	double *lat_values = NULL, *lon_values = NULL;
	double *u_values = NULL, *v_values = NULL;
	double speed, new_angle, new_speed;

	/* Ouverture du fichier en lecture */
	in = fopen(filename, "r");
	if (!in)
	{
		fprintf(stderr, "Error: unable to open file %s\n", filename);
		exit(1);
	}

	/* Création d'une multi field handle */
	mh = codes_grib_multi_handle_new(0);
	if (!mh)
	{
		fprintf(stderr, "Error: unable to create multi field handle\n");
		exit(1);
	}

	/* Conversion de la date et de l'heure en long */
	ldate = convert_date_format(date);
	ltime = convert_time_format(time);

	/* Boucle sur tous les messages dans le fichier */
	while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL)
	{
		/* Vérification des erreurs après avoir lu le message */
		if (err != CODES_SUCCESS)
		{
			CODES_CHECK(err, 0);
		}

		CODES_CHECK(codes_get_long(h, "time.validityDate", &mdate), 0);
		CODES_CHECK(codes_get_long(h, "time.validityTime", &mtime), 0);

		/* La date et l'heure du message correspondent aux valeurs définies */
		if (mdate == ldate && mtime == ltime)
		{
			/* Le message courant contient u et le message suivant contient v */
			hu = h;
			hv = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);

			/* Vérification des erreurs après avoir lu le message suivant */
			if (err != CODES_SUCCESS)
			{
				CODES_CHECK(err, 0);
			}

			CODES_CHECK(codes_get_size(hu, "values", &values_len), 0);

			lat_values = (double *)malloc(values_len * sizeof(double));
			lon_values = (double *)malloc(values_len * sizeof(double));
			u_values = (double *)malloc(values_len * sizeof(double));
			v_values = (double *)malloc(values_len * sizeof(double));

			CODES_CHECK(codes_get_double_array(hu, "latitudes", lat_values, &values_len), 0);
			CODES_CHECK(codes_get_double_array(hu, "longitudes", lon_values, &values_len), 0);
			CODES_CHECK(codes_get_double_array(hu, "values", u_values, &values_len), 0);
			CODES_CHECK(codes_get_double_array(hv, "values", v_values, &values_len), 0);

			/* Boucle sur toutes les valeurs pour hu et hv */
			for (i = 0; i < values_len; i++)
			{
				/* On se trouve dans la zone définies */
				if (lat_values[i] >= min_lat && lat_values[i] <= max_lat && lon_values[i] >= min_lon && lon_values[i] <= max_lon)
				{
					/* On récupère la vitesse du vent et on applique les modifications */
					speed = calcul_wind_speed(u_values[i], v_values[i]);
					switch (val_type)
					{
						case VAL_ANGLE:
							if (is_offset)
							{
								new_angle = atan2(v_values[i], u_values[i]) - value * PI / 180;
							}
							else
							{
								new_angle = (270 - value) * PI / 180;
							}
							u_values[i] = speed * cos(new_angle);
							v_values[i] = speed * sin(new_angle);
							break;
						case VAL_SPEED:
							if (is_offset)
							{
								new_speed = knots_to_mps(speed + value);
							}
							else
							{
								new_speed = knots_to_mps(value);
							}
							if (new_speed <= 0)
							{
								new_speed = knots_to_mps(1);
							}
							u_values[i] *= new_speed / speed;
							v_values[i] *= new_speed / speed;
							break;
						case VAL_POURC:
							if (is_offset)
							{
								new_speed = speed + (speed * value / 100);
								if (new_speed <= 0)
								{
									new_speed = knots_to_mps(1);
								}
								u_values[i] *= new_speed / speed;
								v_values[i] *= new_speed / speed;
							}
							else
							{
								fprintf(stderr, "Error: percentages are necessarily offset\n");
								exit(1);
							}
							break;
					}
				}
			}

			/* On enregistre les nouvelles valeurs dans hu et hv */
			CODES_CHECK(codes_set_double_array(hu, "values", u_values, values_len), 0);
			CODES_CHECK(codes_set_double_array(hv, "values", v_values, values_len), 0);

			/* On ajoute hu et hv à la multi field handle */
			codes_grib_multi_handle_append(hu, 0, mh);
			codes_grib_multi_handle_append(hv, 0, mh);
		}
		else
		{
			codes_grib_multi_handle_append(h, 0, mh);
		}
	}

	/* Fermeture du fichier en lecture */
	fclose(in);

	/* Ouverture du fichier en écriture */
	out = fopen(filename, "w");
	if (!out)
	{
		fprintf(stderr, "Error: unable to open file %s\n", filename);
		exit(1);
	}

	/* Ecriture de la multi field handle dans le fichier */
	codes_grib_multi_handle_write(mh, out);

	/* Fermeture du fichier en écriture */
	fclose(out);

	/* Libération de la mémoire */
	free(lat_values);
	free(lon_values);
	free(u_values);
	free(v_values);
	codes_handle_delete(h);
	codes_handle_delete(hu);
	codes_handle_delete(hv);
	codes_grib_multi_handle_delete(mh);

	return 0;
}
