#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "paes.h"
#include "isochrone.h"

extern int NBEVALSUCCESS;
extern int  NBEVALFAILED;
extern int NBREPAIRSUCCESS;
extern int  NBREPAIRFAILED;

// inspired from PISA:
/* local parameters from paramfile*/
int maxgen; /* maximum number of generations (stop criterion) */
float maxnoise;
char outfile[128];
int Arclength;
int alpha; // max archive size

extern sol_t *Archive;
int dim;
extern int *Grid_pop;   // the array holding the population residing in each grid location
extern model_t The_Model[1];

int read_local_parameters(char *paramfile)
{
     	FILE *fp;
	char str[128], polarfile[256], weatherfile[256];
	point_t start, finish;

	// for the PAES
	int seed, depth;
        long date;
	int stime;

#ifndef WITHTACK
	dim = 2;
#else
	dim = 3;
#endif
	
	// default values
	//polarfile = POLARFILE;
	//weatherfile = WEATHERFILE;

     /* reading parameter file with parameters for selection */
     fp = fopen(paramfile, "r"); 
     assert(fp != NULL);

     fscanf(fp, "%s", str);
     assert(strcmp(str, "seed") == 0);
     fscanf(fp, "%d", &seed);
     
     fscanf(fp, "%s", str);
     assert(strcmp(str, "start") == 0);
     fscanf(fp, "%f%f", &(start.y), &(start.x));
     start.y = degMinToDeg(start.y);
     start.x = degMinToDeg(start.x);
     
     fscanf(fp, "%s", str);
     assert(strcmp(str, "finish") == 0);
     fscanf(fp, "%f%f", &(finish.y), &(finish.x));
     finish.y = degMinToDeg(finish.y);
     finish.x = degMinToDeg(finish.x);

     fscanf(fp, "%s", str);
     assert(strcmp(str, "date") == 0);
     fscanf(fp, "%ld", &date);

     fscanf(fp, "%s", str);
     assert(strcmp(str, "stime") == 0);
     fscanf(fp, "%d", &stime);

     fscanf(fp, "%s", str);
     assert(strcmp(str, "polar") == 0);
     fscanf(fp, "%s", polarfile);

     fscanf(fp, "%s", str);
     assert(strcmp(str, "weather") == 0);
     fscanf(fp, "%s", weatherfile);
    
     fscanf(fp, "%s", str);
     assert(strcmp(str, "noise") == 0);
     fscanf(fp, "%f", &(maxnoise));

     fscanf(fp, "%s", str);
     assert(strcmp(str, "gens") == 0);
     fscanf(fp, "%d", &maxgen);
     
     fscanf(fp, "%s", str);
     assert(strcmp(str, "alpha") == 0);
     fscanf(fp, "%d", &alpha);
     
     fscanf(fp, "%s", str);
     assert(strcmp(str, "depth") == 0);
     fscanf(fp, "%d", &depth);
     
     fscanf(fp, "%s", str);
     assert(strcmp(str, "outputfile") == 0);
     fscanf(fp, "%s", outfile); /* fscanf() returns EOF if
                                   reading failed. */
     srand(seed); /* seeding random number generator */
        
     fclose(fp);

     makeTheModel(dim, depth, start, finish, WAKESIZE, 140, ENDCONE, EPSILON, PANGLE, PTIME, MAXPOINTS, MAXFINISH, FULLANGLE, SCENARIO, NOISE, GRAPHSTAGES, GRAPHRATIO, 0);

     printModel(The_Model);	

	// init
	readPolar(polarfile);
//printf("%d polar couples read %s ...\n", NbPolar, polarfile);
	readWeather(weatherfile, 0,date,stime);
//printf("%s weather map read ...\n", weatherfile); 
//printf("X range [0 - %2.2lf] Y range [0 - %2.2lf] %d x %d squares of size %2.2lf * %2.2lf\n", 
//		WeatherMaxX, WeatherMaxY, WeatherNbX, WeatherNbY, WeatherSquareSizeX, WeatherSquareSizeY);
//printf("time range [0 %2.2lf] by %d period of %2.2lf\n", MaxTime, WeatherNbPeriods, WeatherTimePeriod);

     return (0);
}

sol_t *paes_init(char *paramfile)
{
	int i;
	sol_t *current, *first;

	read_local_parameters(paramfile); 
	initPopulationBase(alpha); 
	// first solution is isochrone
	first = new_sol(relativeStartTime); 

	paes_update_grid(first, Arclength);//here
	paes_archive_soln(first, &Arclength, alpha, PAES_MINIMIZE);

	FILE *fp;
	fp = fopen("noisy_iso.dat", "w");
	printIso(fp, -1, first->chrom);
	fclose(fp);
	fp = fopen("noisy_isofront.dat", "w");
	int j;
	for(j=0; j<dim; j++) fprintf(fp, "%2.3f ", first->obj[j]);
	fprintf(fp, "\n");
	fclose(fp);
	fp = fopen("noisy_isojournal.dat", "w");
	printDetailedPath2(fp, first->chrom, The_Model);
	printf("Journal of initial isochrone in noisy_isojournal.dat\n");
	fclose(fp);
	fp = fopen("noisy_isojournalwithsegments.dat", "w");
	pointSet_t *buildDetailedPathIso();
	printDetailedPath2(fp, buildDetailedPathIso(first->chrom, The_Model), The_Model);
	printf("Detailed Journal of initial isochrone in noisy_isojournalwithsegments.dat\n");
	fclose(fp);

	for (i=1; i<alpha; i++) {
		generateNoise(maxnoise);
		current = new_sol(relativeStartTime);
		if (!current) continue;
		paes_update_grid(current, Arclength);
		paes_archive_soln(current, &Arclength, alpha, PAES_MINIMIZE);
		//printf("add %d th new sol => a=%d\n", i, Arclength);
		//printIso(stdout, -1, current->chrom);
		free_sol(current);
    	} 
	printf("starting with %d archive\n", Arclength);
	printf("archive front stored in noisy_archive0.dat\n");
	fp = fopen("noisy_archive0.dat", "w");
	for(i=0; i<Arclength; i++) {
	if (!eval_sol(Archive + i)) printf("invalid sol in archive 0\n");
			for(j=0; j<dim; j++) fprintf(fp, "%2.3f ", Archive[i].obj[j]);
			fprintf(fp, "\n");
	}
	fclose(fp);


	return first;
}



int lib_main(char *paramfile)
{
	int const argc = 2;
	int i, j, result;
	sol_t *c, *m;
	int keepc;

	//char *paramfile = argv[1];




	/*
// test simple
{
point_t p1[1]= {1.0,1.0}, p2[1];
arrivalPoint(p1, 245.0, 150.0, p2);
printf("aOrtho=%2.5f aLoxo=%2.5f  d=%2.5f\n", dirToPoint(p1, p2), dirToPoint2(p1, p2), distanceToPoint(p1,p2));
exit(0);c
}*/

	if (argc != 2) goto usage;
        
	c = paes_init(paramfile);


	for (i = 0; i < maxgen; i++) {
		keepc = 1;
		generateNoise(maxnoise);
		m = mutate_sol(c);
		if (!m) continue;

		result = paes_compare_min(c->obj, m->obj, dim);
		if (result != 1) {
			if (result ==-1) {
				paes_update_grid(m, Arclength);
				paes_archive_soln(m, &Arclength, alpha, PAES_MINIMIZE);
				keepc = 0;
			}
			else if(result == 0) {
				result = paes_compare_to_archive(m, Arclength, PAES_MINIMIZE);
				if (result != -1) {
					paes_update_grid(m, Arclength);
					paes_archive_soln(m, &Arclength, alpha, PAES_MINIMIZE);
					if((Grid_pop[m->grid_loc] <= Grid_pop[c->grid_loc])||
                                                        (result==1)) {
						keepc = 0;
					}
				}
			}
		}
		if (keepc == 1)  
			free_sol(m);
		else {
			free_sol(c);
			c = m;
		}
	}

	// print the noisy Pareto front
	FILE *fp = fopen("noisy_front.dat", "w");
	for(i=0; i<Arclength; i++) {
		if (!eval_sol(Archive + i)) continue;  // 0 is OK for eval_sol
		for(j=0; j<dim; j++) fprintf(fp, "%2.3f ", Archive[i].obj[j]);
		fprintf(fp, "\n");
		
	}
	fclose(fp);

	fp = fopen("array.dat", "w");
	for(i=0; i<Arclength; i++) {
		if (!eval_sol(Archive + i)) continue;  // 0 is OK for eval_sol
		printDetailedPath3(fp, Archive[i].chrom, The_Model);
	}
	fclose(fp);



	// print the Pareto front
	fp = fopen("noisyfree_front.dat", "w");
	int invalidatedSols = 0;
	for(i=0; i<Arclength; i++) 
		if (eval_sol(Archive + i)) { // 0 is OK for eval_sol
			for(j=0; j<dim; j++) fprintf(fp, "%2.3f ", Archive[i].obj[j]);
			fprintf(fp, "\n");
		}
		else invalidatedSols++;
	fclose(fp);

	fp = fopen("pareto_set_journal.dat", "w");
	for(i=0; i<Arclength; i++) {
		if (!eval_sol(Archive + i)) continue;  // 0 is OK for eval_sol
		pointSet_t *buildDetailedPathIso();
		printDetailedPath2(fp, buildDetailedPathIso(Archive[i].chrom,The_Model), The_Model);
		fprintf(fp, "\n");
	}
	fclose(fp);



	printf("final archive of %d sols\n", Arclength);
	printf("final archive contains %d invalid solutions\n", invalidatedSols);
	printf("evals success %d (%2.2f%% of %d evals)\n", NBEVALSUCCESS, 100.0*NBEVALSUCCESS/(NBEVALSUCCESS + NBEVALFAILED), NBEVALSUCCESS + NBEVALFAILED);
	printf("repairs success %d (%2.2f%% of %d repairs)\n", NBREPAIRSUCCESS, 100.0*NBREPAIRSUCCESS/(NBREPAIRSUCCESS + NBREPAIRFAILED), NBREPAIRSUCCESS + NBREPAIRFAILED);
fp = fopen("noisy_ps.dat", "w");
for(i=0; i<Arclength; i++)
	printIso(fp, -1, Archive[i].chrom);
fclose(fp);

                printf("noisy free pareto front saved in %s \n", "noisyfree_front.dat" );
                printf("noisy pareto front saved in %s \n", "noisy_front.dat" );
                printf("draw front and associated paths set with:\n");
                printf("gnuplot -e 'set xlabel \"time (hours)\"; set ylabel "
#ifdef WITHSTRESS
" \"stress\";"
#else
" \"distance (ml)\";"
#endif
" plot \"%s\" u 1:2 title \"noisy front\" with point ps 2 , \"%s\" u 1:2 title \"noisyfree front\" with point ps 2 , \"noisy_isofront.dat\" u 1:2 title \"initial iso\" with points ps 3 lw 2' -persist\n", "noisy_front.dat", "noisyfree_front.dat");
                printf("gnuplot -e 'set size ratio -1; plot \"noisy_ps.dat\" u 3:4 with lines, \"noisy_iso.dat\" u 3:4 with lines lw 2' -persist\n");

	free_sol(c);
	freeLibrary(SCENARIO, Arclength);
	return 0;

usage:
	//printf("%s <param file> -- syntax:\n", argv[0]);
	printf("seed <int seed value>\n");
	printf("start <float x> <float y>\n");
	printf("finish <float x> <float y>\n");
	printf("polar <polar filename>\n");
	printf("weather <weather file>\n");
	printf("noise <float x> // noise bound x => shift in the range [-x .. +x]\n");
	printf("gens <iterations>\n");
	printf("alpha <archive size>\n");
	printf("depth <grid depth -- eg 4>\n");
	printf("outputfile <front output filename>\n");

	return 0;
}


/*
EXAMPLE OF PARAM FILE params

seed 5 
start  44.14012 -5.00085 
finish 47.47179 -5.04495
date 20151005 
stime 185544
polar ../gd_surprise.dat 
weather ../051015.isoc 
noise 0.0 
gens 4000 
alpha 100 
depth 4 
outputfile output.out
*/
