#include "grib_wind_tools.h"

/*
 * Fonctions utilitaires
 */

long convert_date_format(char *date)
{
	char *endptr;
	long day = strtol(date, &endptr, 10);
	long month = strtol(++endptr, &endptr, 10);
	long year = strtol(++endptr, &endptr, 10);

	return (year * 100 + month) * 100 + day;
}

long convert_time_format(char *time)
{
	char *endptr;
	long hour = strtol(time, &endptr, 10);
	long minute = strtol(++endptr, &endptr, 10);

	return hour * 100 + minute;
}

codes_handle *get_handle_from_date(char *filename, long date, long time, char param)
{
	int err = 0;
	FILE *in = NULL;
	codes_handle *h = NULL;
	long mdate, mtime, edition, mparam, lparam;

	/* Ouverture du fichier en lecture */
	in = fopen(filename, "r");
	if (!in)
	{
		fprintf(stderr, "Error: unable to open file %s\n", filename);
		exit(1);
	}

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
		CODES_CHECK(codes_get_long(h, "editionNumber", &edition), 0);

		/* u et v sont identifiés par des clés différentes selon l'édition du fichier GRIB */
		if (edition == 1)
		{
			CODES_CHECK(codes_get_long(h, "indicatorOfParameter", &mparam), 0);
			if (param == 'u')
			{
				lparam = 33;
			}
			else
			{
				lparam = 34;
			}
		}
		else
		{
			CODES_CHECK(codes_get_long(h, "parameterNumber", &mparam), 0);
			if (param == 'u')
			{
				lparam = 2;
			}
			else
			{
				lparam = 3;
			}
		}

		/* La date, l'heure et le paramètre du message correspondent aux valeurs définies */
		if (mdate == date && mtime == time && mparam == lparam)
		{
			break;
		}

		/* Libération de la mémoire */
		codes_handle_delete(h);
	}

	/* Fermeture du fichier en lecture */
	fclose(in);

	return h;
}

int get_nearest_point(codes_handle *h, double lat, double lon, double *res)
{
	codes_nearest *nearest = NULL;
	int err = 0, mode = 0, i = 0, shortest = 0;
	double lats[4] = {0,};
	double lons[4] = {0,};
	double values[4] = {0,};
	double distances[4] = {0,};
	int indexes[4] = {0,};
	size_t size = 4;

	nearest = codes_grib_nearest_new(h, &err);
	CODES_CHECK(err, 0);
	CODES_CHECK(codes_grib_nearest_find(nearest, h, lat, lon, mode, lats, lons, values, distances, indexes, &size), 0);

	/* On recherche le point de grille le plus proche parmi les 4 retournés */
	for (i = 1; i < 4; i++)
	{
		if (distances[i] < distances[shortest])
		{
			shortest = i;
		}
	}
	res[0] = lats[shortest];
	res[1] = lons[shortest];
	res[2] = values[shortest];

	/* Libération de la mémoire */
	codes_grib_nearest_delete(nearest);

	return 0;
}

int set_wind(char *filename, long date, long time, char param, double lat, double lon, double value)
{
	int err = 0;
	FILE *in = NULL, *out = NULL;
	codes_handle *h = NULL;
	codes_multi_handle *mh = NULL;
	long mdate, mtime, edition, mparam, lparam;
	size_t i = 0, values_len = 0;
	double *lat_values = NULL, *lon_values = NULL;
	double *values = NULL;

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
		CODES_CHECK(codes_get_long(h, "editionNumber", &edition), 0);

		/* u et v sont identifiés par des clés différentes selon l'édition du fichier GRIB */
		if (edition == 1)
		{
			CODES_CHECK(codes_get_long(h, "indicatorOfParameter", &mparam), 0);
			if (param == 'u')
			{
				lparam = 33;
			}
			else
			{
				lparam = 34;
			}
		}
		else
		{
			CODES_CHECK(codes_get_long(h, "parameterNumber", &mparam), 0);
			if (param == 'u')
			{
				lparam = 2;
			}
			else
			{
				lparam = 3;
			}
		}

		/* La date, l'heure et le paramètre du message correspondent aux valeurs définies */
		if (mdate == date && mtime == time && mparam == lparam)
		{
			CODES_CHECK(codes_get_size(h, "values", &values_len), 0);

			lat_values = (double *)malloc(values_len * sizeof(double));
			lon_values = (double *)malloc(values_len * sizeof(double));
			values = (double *)malloc(values_len * sizeof(double));

			CODES_CHECK(codes_get_double_array(h, "latitudes", lat_values, &values_len), 0);
			CODES_CHECK(codes_get_double_array(h, "longitudes", lon_values, &values_len), 0);
			CODES_CHECK(codes_get_double_array(h, "values", values, &values_len), 0);

			/* Boucle sur toutes les valeurs du message */
			for (i = 0; i < values_len; i++)
			{
				if (fabs(lat - lat_values[i]) < 0.0001 && fabs(lon - lon_values[i]) < 0.0001)
				{
					values[i] = value;
					break;
				}
			}

			/* On enregistre les nouvelles valeurs dans le message */
			CODES_CHECK(codes_set_double_array(h, "values", values, values_len), 0);
		}

		codes_grib_multi_handle_append(h, 0, mh);
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
	free(values);
	codes_handle_delete(h);
	codes_grib_multi_handle_delete(mh);

	return 0;
}

double calcul_wind_speed(double u, double v)
{
	/* Valeur de retour en m/s */
	return sqrt((u * u) + (v * v));
}

double calcul_wind_angle(double u, double v)
{
	return fmod((270 - atan2(v, u) * 180 / PI), 360);
}

double knots_to_mps(double value)
{
	return value * KTS2MPS;
}
