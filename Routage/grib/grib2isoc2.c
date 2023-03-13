/*
* C Implementation: grib2isoc2
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

 #include "eccodes.h"

 #define DELTA 6 //time interval between two messages

 void usage(char* prog) {
   printf("Usage: %s grib_file [date time [latMin longMin latMax longMax]] \n",prog);
   exit(1);
 }

 int main(int argc, char** argv) {
   FILE* in = NULL;
   int err = 0,i,nbMessages;
   double *valuesU,*valuesV = NULL;
   double lat,lon,value,missingValue = 0;
   double latitudeOfFirstGridPointInDegrees,firstLat;
   double latitudeOfLastGridPointInDegrees, lastLat;
   double longitudeOfFirstGridPointInDegrees,firstLon;
   double longitudeOfLastGridPointInDegrees, lastLon;
   double jDirectionIncrementInDegrees;
   double iDirectionIncrementInDegrees;
   double lat1,lon1,lat2,lon2,latMin,latMax,longMin,longMax,cLon,cLat;
   long date, time, start, startStep;
   int calendar[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
   long dataTime,dataDate,i1,i2,j1,j2,date2,date3,day,month,year,mDay,mMonth,mYear,nb_p,nb_d;
   int delta;


   long numberOfPointsAlongAParallel;
   long numberOfPointsAlongAMeridian;
   long ci, cj;

   char* filename = NULL;
   size_t values_len = 0;
   FILE* out = NULL;
   char* outfile = "out.isoc";

   /* Message handle. Required in all the grib_api calls acting on a message.*/
   grib_handle *h = NULL;

   delta = DELTA;

   if (argc != 2 && argc != 4 && argc != 8) usage(argv[0]);

   filename=strdup(argv[1]);

   if (argc >= 4){
   date = atoi(argv[2]);
   time = atoi(argv[3]);
   }
   if (argc == 8){
   lat1 = atof(argv[4]);
   lon1 = atof(argv[5]);
   lat2 = atof(argv[6]);
   lon2 = atof(argv[7]);


   latMin = (((int)(lat1*100)%100)/60.0)+floor(lat1);
   latMax = (((int)(lat2*100)%100)/60.0)+floor(lat2);
   if (lon1<0)
   longMin = -((((int)(-lon1*100)%100)/60.0)+floor(-lon1));
   else
   longMin = ((((int)(lon1*100)%100)/60.0)+floor(lon1));
   if (lon2<0)
   longMax = -((((int)(-lon2*100)%100)/60.0)+floor(-lon2));
   else
   longMax = ((((int)(lon2*100)%100)/60.0)+floor(lon2));
   }
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

  GRIB_CHECK(grib_get_long(h,"dataDate",&dataDate),0);
  GRIB_CHECK(grib_get_long(h,"dataTime",&dataTime),0);
  GRIB_CHECK(grib_get_long(h,"startStep",&startStep),0);

  dataTime+=startStep*100;

  GRIB_CHECK(grib_get_long(h,"Ni",&numberOfPointsAlongAParallel),0);
  GRIB_CHECK(grib_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
  GRIB_CHECK(grib_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
  //approximation : average latitude
  //xSize = abs(60*cos(M_PI*(latitudeOfFirstGridPointInDegrees+latitudeOfLastGridPointInDegrees)/(2*180))*iDirectionIncrementInDegrees);
  GRIB_CHECK(grib_get_long(h,"Nj",&numberOfPointsAlongAMeridian),0);

  if (argc <= 4){
        latMin=latitudeOfFirstGridPointInDegrees;
        latMax=latitudeOfLastGridPointInDegrees;
        longMin=longitudeOfFirstGridPointInDegrees;
        longMax=longitudeOfLastGridPointInDegrees;
  }

  /* bounds for latitude */
  cLat = latitudeOfFirstGridPointInDegrees;
  i=0;
  j1=0;
  firstLat = cLat;
  while (cLat<=latMin){
    i++;
    cLat+=jDirectionIncrementInDegrees;
  }
  if (i>0) {
    j1=i-1;
    i=i-1;
    cLat = cLat-jDirectionIncrementInDegrees;
    firstLat = cLat;
  }
  lastLat = latitudeOfLastGridPointInDegrees;
  while (cLat<latMax && cLat<latitudeOfLastGridPointInDegrees){
    i++;
    cLat+=jDirectionIncrementInDegrees;
  }
  j2=i;
  lastLat = cLat;

  /* bounds for longitude */
  cLon = longitudeOfFirstGridPointInDegrees;
  i=0;
  i1=0;
  firstLon = cLon;
  while (cLon<=longMin){
    i++;
    cLon+=iDirectionIncrementInDegrees;
  }

  if (i>0) {
    i1=i-1;
    i=i-1;
    cLon = cLon-iDirectionIncrementInDegrees;
    firstLon = cLon;
  }
  lastLon = longitudeOfLastGridPointInDegrees;
  while (cLon<longMax && cLon<longitudeOfLastGridPointInDegrees){
    i++;
    cLon+=iDirectionIncrementInDegrees;
  }
  i2=i;
  lastLon = cLon;

  date2=dataDate;
  day=date2%100;
  date2=date2/100;
  month=date2%100;
  year=date2/100;

  date3=date;
  mDay=date3%100;
  date3=date3/100;
  mMonth=date3%100;
  mYear=date3/100;

  time = time/100;

if (argc==2){
  time = dataTime;
  date = dataDate;
  date3=dataDate;
  mDay=date3%100;
  date3=date3/100;
  mMonth=date3%100;
  mYear=date3/100;
}


  nb_p = nbMessages-1;
  if (mYear==year && mMonth==month){
  nb_d = mDay-day;
if (nb_d>=0) {nb_p = (int)2*(nb_d*24+(time-dataTime)/100)/delta;
}
  } else
     if (mYear==year && mMonth>month){
         nb_d = mDay + calendar[month-1]-day;
         if (month == 2 && (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)) nb_d++;
nb_p = (int)2*(nb_d*24+(time-dataTime)/100)/delta;
      } else
        if (mYear>year){
        // we suppose mMonth = january and month = december
        nb_d = mDay+31-day;
nb_p = (int)2*(nb_d*24+(time-dataTime)/100)/delta;
        }


     if (nb_p < nbMessages-1){
         start = (dataTime+delta*nb_p*100/2)%2400;
         out = fopen(outfile,"w");
	 fprintf(out,"%d %ld %ld %d %ld %ld %lf %lf %lf %lf\n",(nbMessages-nb_p)/2,date,start,delta,j2-j1+1,i2-i1+1,firstLat,firstLon,jDirectionIncrementInDegrees,iDirectionIncrementInDegrees);

  GRIB_CHECK(grib_get_size(h,"values",&values_len),0);

  valuesU = malloc(values_len*sizeof(double));
  valuesV = malloc(values_len*sizeof(double));

  while (nb_p > 0) {h = grib_handle_new_from_file(0,in,&err);
    if (h == NULL) {
      printf("Error: unable to create handle from file %s\n",filename);
    }
    nb_p--;
  }

  GRIB_CHECK(grib_get_double_array(h,"values",valuesU,&values_len),0);

  h = grib_handle_new_from_file(0,in,&err);
  if (h == NULL) {
     printf("Error: unable to create handle from file %s\n",filename);
  }

  GRIB_CHECK(grib_get_double_array(h,"values",valuesV,&values_len),0);

  for(i = 0; i < values_len; i++){
    ci = i%numberOfPointsAlongAParallel;
    cj = i/numberOfPointsAlongAParallel;

    if (cj>=j1 && cj<=j2 && ci>=i1 && ci<=i2)
      //fprintf(out,"%lf %lf\n",sqrt(valuesU[i]*valuesU[i]+valuesV[i]*valuesV[i])*3600/1852.0,270-atan2(valuesV[i],valuesU[i])*180/M_PI);
      fprintf(out,"%lf %lf\n ",valuesU[i],valuesV[i]);
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
    ci = i%numberOfPointsAlongAParallel;
    cj = i/numberOfPointsAlongAParallel;

    if (cj>=j1 && cj<=j2 && ci>=i1 && ci<=i2)
      //fprintf(out,"%lf %lf\n",sqrt(valuesU[i]*valuesU[i]+valuesV[i]*valuesV[i])*3600/1852.0,270-atan2(valuesV[i],valuesU[i])*180/M_PI);
    fprintf(out,"%lf %lf\n ",valuesU[i],valuesV[i]);
  }

   }
   free(valuesU);
   free(valuesV);
     /* At the end the grib_handle is deleted to free memory. */
   grib_handle_delete(h);
   fclose(out);
} else printf("No file created, date issue\n");

   fclose(in);

   return 0;
}

