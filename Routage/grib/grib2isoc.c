/*
* C Implementation: grib2isoc
*
* Description: convert a grib file to an isoc file
*
*
* Author: G. Guillou
*
*
*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <string.h>

#include "/usr/local/gribapi/include/grib_api.h"

 void usage(char* prog) {
   printf("Usage: %s grib_file\n",prog);
   exit(1);
 }

 int main(int argc, char** argv) {
   FILE* in = NULL;
   int err = 0,i,nbMessages;
   double *valuesU,*valuesV = NULL;
   double lat,lon,value,missingValue = 0;
   double latitudeOfFirstGridPointInDegrees;
   double latitudeOfLastGridPointInDegrees;
   double jDirectionIncrementInDegrees, xSize;
   double iDirectionIncrementInDegrees, ySize;

   long numberOfPointsAlongAParallel;
   long numberOfPointsAlongAMeridian;

   char* filename = NULL;
   size_t values_len = 0;
   FILE* out = NULL;
   char* outfile = "out.isoc";

   /* Message handle. Required in all the grib_api calls acting on a message.*/
   grib_handle *h = NULL;


   if (argc != 2) usage(argv[0]);

   filename=strdup(argv[1]);

   in = fopen(filename,"r");
   if(!in) {
     printf("ERROR: unable to open file %s\n",filename);
     return 1;
   }

   GRIB_CHECK(grib_count_in_file(0,in,&nbMessages),0);

  h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
  }

  GRIB_CHECK(grib_get_long(h,"Ni",&numberOfPointsAlongAParallel),0);
  GRIB_CHECK(grib_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
  ySize = jDirectionIncrementInDegrees*60;
  GRIB_CHECK(grib_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
  //approximation : average latitude
  xSize = abs(60*cos(M_PI*(latitudeOfFirstGridPointInDegrees+latitudeOfLastGridPointInDegrees)/(2*180))*iDirectionIncrementInDegrees);
  GRIB_CHECK(grib_get_long(h,"Nj",&numberOfPointsAlongAMeridian),0);

  GRIB_CHECK(grib_get_size(h,"values",&values_len),0);
  out = fopen(outfile,"w");

  valuesU = malloc(values_len*sizeof(double));
  valuesV = malloc(values_len*sizeof(double));

  fprintf(out,"%d 6 %ld %ld %lf %lf\n",nbMessages/2,numberOfPointsAlongAParallel,numberOfPointsAlongAMeridian,xSize,ySize);

  GRIB_CHECK(grib_get_double_array(h,"values",valuesU,&values_len),0);

  h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
  }

  GRIB_CHECK(grib_get_double_array(h,"values",valuesV,&values_len),0);

  for(i = 0; i < values_len; i++){
    fprintf(out,"%lf %lf\n",sqrt(valuesU[i]*valuesU[i]+valuesV[i]*valuesV[i])*3600/1852.0,atan2(valuesU[i],valuesV[i])*180/M_PI);
  }

   /* Loop on all the messages in a file.*/
   while ((h = grib_handle_new_from_file(0,in,&err)) != NULL ) {
         /* Check of errors after reading a message. */
     if (err != GRIB_SUCCESS) GRIB_CHECK(err,0);

  /* get data values*/
  GRIB_CHECK(grib_get_double_array(h,"values",valuesU,&values_len),0);

 h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
    printf("Error: unable to create handle from file %s\n",filename);
  }

  /* get data values*/
  GRIB_CHECK(grib_get_double_array(h,"values",valuesV,&values_len),0);

  for(i = 0; i < values_len; i++){
    fprintf(out,"%lf %lf\n",sqrt(valuesU[i]*valuesU[i]+valuesV[i]*valuesV[i])*3600/1852.0,atan2(valuesU[i],valuesV[i])*180/M_PI);
  }

   }
  free(valuesU);
  free(valuesV);
     /* At the end the grib_handle is deleted to free memory. */
     grib_handle_delete(h);

   fclose(in);
   fclose(out);

   return 0;
}

