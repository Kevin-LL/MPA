/*
* C Implementation: xmlToDat
*
* Description: convert a polar in xml format in serialized format
*
*
* Author: G. Guillou
*
*
*/
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 #define TAILLE 128

 
 void usage(char* prog) {
   printf("Usage: %s polar_file.xml\n",prog);
   exit(1);
 }
 
int readInt(char* ind){
   int n = 0;
   while (ind[0]!='"'){
   n=n*10+ind[0]-'0';
   ind++;
   }
   return n;
}

double readDouble(char* ind){
   double n = 0, puis10 = 10;
   while (ind[0]!='.' && ind[0]!='"'){
   n=n*10+ind[0]-'0';
   ind++;
   }
   if (ind[0]=='.') ind++;
   while (ind[0]!='"'){
   n=n+(ind[0]-'0')/puis10;
   puis10 *= 10;
   ind++;
   }   
   return n;
}



 int lib_main(char *paramfile) {
   FILE* in = NULL;
   char* filename = NULL;
   FILE* out = NULL;
   char* outfile = "polar.dat";
   char line [TAILLE];
   char* slashPolarItem = "/PolarItem";
   char* slashPolarCurve = "/PolarCurve";
   int i = 0, j = 0;
   char* polarCurveIndex = "PolarCurveIndex";
   char* angle = "Angle value";
   char * indice;
   int intValue, intForce;
   double doubleValue;
  
   //if (argc != 2) usage(argv[0]);
 
   filename=strdup(paramfile);
 
   in = fopen(filename,"r");
   out = fopen(outfile,"w");

   if(!in) {
     printf("ERROR: unable to open file %s\n",filename);
     return 1;
   }

   while ( fgets(line, TAILLE, in) != NULL ){ /* read a line */
     if (strstr(line,slashPolarItem)!=NULL) i++;
     if (strstr(line,polarCurveIndex)!=NULL) j++;
   }

   j = i/j;

   fprintf(out,"%d\n",i+j); // add a null polar, see downward

   rewind(in);

   // add a null polar
   while ( fgets(line, TAILLE, in) != NULL){
       if (strstr(line,slashPolarCurve) != NULL) break;
       if (strstr(line,angle)!=NULL){
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                intValue = readInt(indice);
                fprintf(out,"0 %d %.1lf\n",intValue,0);
	       }
   }

   rewind(in);

   while ( fgets(line, TAILLE, in) != NULL ) /* read a line */
      {
       if (strstr(line,polarCurveIndex)!=NULL){
              indice = strchr(line,'"');
              if (indice==NULL) printf("Badformed polar\n");
              indice++;
              intForce = readInt(indice);
       } else if (strstr(line,angle)!=NULL){
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                intValue = readInt(indice);
                fgets(line, TAILLE, in);
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                doubleValue = readDouble(indice);
                fprintf(out,"%d %d %.2lf\n",intForce,intValue,doubleValue);
	       }
      }

   fclose(in);
   fclose(out);
 
   return 0;
}

 int main(int argc, char** argv) {
   FILE* in = NULL;
   char* filename = NULL;
   FILE* out = NULL;
   char* outfile = "polar.dat";
   char line [TAILLE];
   char* slashPolarItem = "/PolarItem";
   char* slashPolarCurve = "/PolarCurve";
   int i = 0, j = 0;
   char* polarCurveIndex = "PolarCurveIndex";
   char* angle = "Angle value";
   char * indice;
   int intValue, intForce;
   double doubleValue;
  
   if (argc != 2) usage(argv[0]);
 
   filename=strdup(argv[1]);
 
   in = fopen(filename,"r");
   out = fopen(outfile,"w");

   if(!in) {
     printf("ERROR: unable to open file %s\n",filename);
     return 1;
   }

   while ( fgets(line, TAILLE, in) != NULL ){ /* read a line */
     if (strstr(line,slashPolarItem)!=NULL) i++;
     if (strstr(line,polarCurveIndex)!=NULL) j++;
   }

   j = i/j;

   fprintf(out,"%d\n",i+j); // add a null polar, see downward

   rewind(in);

   // add a null polar
   while ( fgets(line, TAILLE, in) != NULL){
       if (strstr(line,slashPolarCurve) != NULL) break;
       if (strstr(line,angle)!=NULL){
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                intValue = readInt(indice);
                fprintf(out,"0 %d %.1lf\n",intValue,0);
	       }
   }

   rewind(in);

   while ( fgets(line, TAILLE, in) != NULL ) /* read a line */
      {
       if (strstr(line,polarCurveIndex)!=NULL){
              indice = strchr(line,'"');
              if (indice==NULL) printf("Badformed polar\n");
              indice++;
              intForce = readInt(indice);
       } else if (strstr(line,angle)!=NULL){
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                intValue = readInt(indice);
                fgets(line, TAILLE, in);
                indice = strchr(line,'"');
                if (indice==NULL) printf("Badformed polar\n");
                indice++;
                doubleValue = readDouble(indice);
                fprintf(out,"%d %d %.2lf\n",intForce,intValue,doubleValue);
	       }
      }

   fclose(in);
   fclose(out);
 
   return 0;
}

