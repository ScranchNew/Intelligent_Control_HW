/***************************************************************/
/* This is a simple genetic algorithm implementation where     */
/* the evaluation function takes positive values only and      */
/* the fitness of an individual is the same as the value of    */
/* the objective function                                      */
/***************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#pragma warning(disable:4996)

/* Change any of these parameters to match your needs */

#define PHI			3.14159265
#define	NRUNS		1      	    /* Number of Runs */
#define POPSIZE 	50		    /* Population size */
#define MAXGENS 	500	        /* Max. number of generations */

//For Problem #2
#define NVARS 		36        /* Number of bits in a chromosome        */
                              /* This is for Problem #2, 18 bits each variable */
//

#define PXOVER 		0.25	    /* Probability of crossover */
#define PMUTATION 	0.02	    /* Probability of mutation */
#define TRUE 		1
#define FALSE 		0
#define OFFSET		100    /* To make a max. problem - 1:50,2:100, 3:100, 4:100 5:10000 */
#define OPTIMAL     0
#define ERRORRANGE  0.0001

//Problem #2
#define NX          2          /* No. of variables in func. of Problem #2 */
#define FROM        -2.048     /* start of interval for Problem #2 */
#define TO          2.048      /* end of interval for Problem #2  */
//

int generation;			  /* Current generation no. */
int n_generation;		  /* Generation for obtaining OFFSET solution */
int avg_n_generation;	  /* Average of n_generation */
int cur_best;		      /* Best individual */
FILE *galog, *solution;	  /* Output files */

struct genotype           /* Genotype (GT), a member of the population */
{
	int	gene[NVARS];	  /* A binary string of chromosome */
	double	fitness;	  /* GT's fitness */
	double	rfitness;	  /* Relative fitness */
	double	cfitness;	  /* Cumulative fitness */
};

struct genotype population[POPSIZE+1];		/* Population */
struct genotype newpopulation[POPSIZE+1];	/* New population */
											/* replaces the old population */

/* Declaration of procedures used by this genetic algorithm */

void initialize(void);
void evaluate(void);
void keep_the_best(void);
void elitist(void);
void Select(void);
void RANKING(void);
void TOURNAMENT(void);
void crossover(void);
void Xover(int,int);
void swap(int *, int *);
void mutate(void);
void report(void);
double DEJONG1(double *x);
double DEJONG2(double *x);
double DEJONG3(double *x);
double DEJONG4(double *x);
double DEJONG5(double *x);

double DEJONG1(double *x)
{
	int i;
	double y=0.;
	for ( i=0; i<NX; i++){
		y+=x[i]*x[i];
	}
	return y;
}

double DEJONG2(double *x)
{
	double y=0.;
	y=100.*pow((x[0]*x[0]-x[1]),2.)+pow((1.-x[0]),2.);
	return y;
}

double DEJONG3(double *x)
{
	int i;
	double y=0.;
	for (i=0; i<NX; i++){
		if(x[i]<0)
		   x[i]-=1;
		y+=(int)(x[i]);
	}
	return y;
}

double gaussian(double Xmean,double dev)
{
	double X=0;
	double y;
	X=2.56*(double)rand()/RAND_MAX;
	y=(1./sqrt(2*PHI*dev*dev))*exp(-pow(X-Xmean,2)/(2.*dev*dev));
	return y;
}

double DEJONG4(double *x)
{
	int i;
	double y=0.;
	for (i=0; i<NX; i++){
		y +=(i+1)*pow(x[i],4);
	}
	y +=gaussian(0.,1.);
	return y;
}

double DEJONG5(double *x)
{
	int i,j;
	double y=0.;
	double f=0.;
	int A[2][25];

	for (j=0; j<25; j++){
		if((j%5)==0){
			A[0][j]=-32;
			if (j==0)
				A[1][j]=-32;
			else
				A[1][j]=A[1][j-1]+16;
		}
		else{
			A[0][j] = A[0][j-1] +16;
			A[1][j] = A[1][j-1];
		}
	}



	for (j=0; j<25; j++){
		f = j+1;
		for (i=0; i<2; i++){
			f +=pow ( (x[i]-A[i][j]),6);
		}
		y +=1./f;
	}
	y+=1./500;
	y=1./y;
	return y;
}


/***************************************************************/
/* Initialization function: Initializes the values of genes    */
/***************************************************************/

void initialize(void)
{
	int i, j;

	for (i = 0; i < POPSIZE+1; i++) {
		population[i].fitness = 0.;
		population[i].rfitness = 0.;
		population[i].cfitness = 0.;
		for (j = 0; j < NVARS; j++) {
			population[i].gene[j] = rand() % 2;
		}
	}
}

/*************************************************************/
/* Evaluation function: This takes a user defined function.  */
/* Each time this is changed, the code has to be recompiled. */
/* Problem #2 is considered.                                 */
/*************************************************************/

void evaluate(void)
{
	int mem;
	int i,j;

	double x[NX], rate,fitemp;
        //Decoding each variable to decimal number
	for (mem = 0; mem < POPSIZE; mem++) {
	    fitemp=0.;
		for (j = 0; j < NX ; j++){
			x[j]=0.;
			rate = 1.;
		    for (i = NVARS-NVARS*j/NX; i > NVARS-NVARS*(j+1)/NX ; i--) {
			x[j] += (double)population[mem].gene[i-1] * rate;
			rate *= 2.;
			}
			x[j] = FROM + x[j] * (TO-FROM) / ( rate - 1. );	
			
		}
//Problem #2
        //If Problem #1, fitemp = DEJONG1(x); 
        fitemp = DEJONG2(x);
//
		
		//To convert a minimization problem to a max. problem
		if (fitemp > OFFSET ) fitemp = OFFSET-0.00001;
		population[mem].fitness = OFFSET-fitemp;
	}

}

/***************************************************************/
/* Keep_the_best function: This function keeps track of        */
/* the best member of the population. Note that the last entry */
/* in the array Population holds a copy of the best individual */
/***************************************************************/

void keep_the_best()
{
	int mem;
	int i;
	cur_best = 0;       /* stores the index of the best individual */

	for (mem = 0; mem < POPSIZE; mem++) {
		if (population[mem].fitness > population[POPSIZE].fitness) {
			cur_best = mem;
			population[POPSIZE].fitness = population[mem].fitness;
		}
	}
	/* Once the best member in the population is found, copy the genes */
	for (i = 0; i < NVARS; i++)
		population[POPSIZE].gene[i] = population[cur_best].gene[i];
}

/****************************************************************/
/* Elitist function: The best member of the previous generation */
/* is stored as the last in the array. If the best member of    */
/* the current generation is worse than the best member of the  */
/* previous generation, the latter one would replace the worst  */
/* member of the current population                             */
/****************************************************************/

void elitist()
{
	int i;
	double best, worst;			/* Best and worst fitness values */
	int best_mem, worst_mem;	/* Indexes of the best and worst member */

	best = population[0].fitness;
	worst = population[0].fitness;
	for (i = 0; i < POPSIZE-1; ++i) {
		if(population[i].fitness > population[i+1].fitness) {      
			if (population[i].fitness >= best) {
				best = population[i].fitness;
				best_mem = i;
			}
			if (population[i+1].fitness <= worst) {
				worst = population[i+1].fitness;
				worst_mem = i + 1;
			}
		} else {
			if (population[i].fitness <= worst) {
				worst = population[i].fitness;
				worst_mem = i;
			}
			if (population[i+1].fitness >= best)
			{
				best = population[i+1].fitness;
				best_mem = i + 1;
			}
		}
	}
	/* If best individual from the new population is better than */
	/* the best individual from the previous population, then    */
	/* copy the best from the new population; else replace the   */
	/* worst individual from the current population with the     */
	/* best one from the previous generation                     */

	if (best > population[POPSIZE].fitness) n_generation = generation+1;
	if (best >= population[POPSIZE].fitness) {
		for (i = 0; i < NVARS; i++)
			population[POPSIZE].gene[i] = population[best_mem].gene[i];
		population[POPSIZE].fitness = population[best_mem].fitness;
	} else {
		for (i = 0; i < NVARS; i++)
			population[worst_mem].gene[i] = population[POPSIZE].gene[i];
		population[worst_mem].fitness = population[POPSIZE].fitness;
	} 
}
/**************************************************************/
/* Selection function: Standard proportional selection for    */
/* maximization problems incorporating elitist model - makes  */
/* sure that the best member survives                         */
/**************************************************************/

void Select(void)
{
	int mem, i, j;
	double sum = 0;
	double p;

	/* Find total fitness of the population */
	for (mem = 0; mem < POPSIZE; mem++) {
		sum += population[mem].fitness;
	}

	/* Calculate relative fitness */
	for (mem = 0; mem < POPSIZE; mem++) {
		population[mem].rfitness =  population[mem].fitness/sum;
	}
	population[0].cfitness = population[0].rfitness;

	/* Calculate cumulative fitness */
	for (mem = 1; mem < POPSIZE; mem++) {
		population[mem].cfitness =  population[mem-1].cfitness +       
			population[mem].rfitness;
	} 

	/* Finally select survivors using cumulative fitness. */
	for (i = 0; i < POPSIZE; i++) { 
		p = (double)rand() /RAND_MAX;
		if (p < population[0].cfitness)
			newpopulation[i] = population[0];      
		else {
			for (j = 0; j < POPSIZE;j++)      
				if (p >= population[j].cfitness && 
						p<population[j+1].cfitness)
					newpopulation[i] = population[j+1];
		} 
	}

	/* Once a new population is created, copy it back */
	for (i = 0; i < POPSIZE; i++)
		population[i] = newpopulation[i];      
}

void RANKING(void)
{
	int i,j,mem;
	struct genotype temp;
	double sum = 0;
	double p;

	for (i=POPSIZE-1; i>=2 ; i--){
		for (j=0;j<=i-1; j++){
			if (population[j].fitness > population[j+1].fitness){
				temp = population[j];
				population[j]=population[j+1];
				population[j+1]=temp;
					
			}
		}
	}
	 /* To make the total sum equal to 1 - 1st:0.1, 2nd:0.05, 3rd:0.05, 4th:0.025, 5th:0.025, 6th-30th:0.02, 31st-50th: 0.0125 */
    
	for (mem = 0; mem < POPSIZE-30; mem++) {
		population[mem].rfitness = 0.0125;
	}
	for (mem = 20; mem < POPSIZE-5; mem++) {
		population[mem].rfitness = 0.02;
	}
	for (mem = 45; mem < POPSIZE-3; mem++) {
		population[mem].rfitness = 0.025;
	}
	for (mem = 47; mem < POPSIZE-1; mem++) {
		population[mem].rfitness = 0.05;
	}
	population[49].rfitness = 0.1;    
	population[0].cfitness = population[0].rfitness;

	for (mem = 1; mem < POPSIZE; mem++) {
		population[mem].cfitness =  population[mem-1].cfitness +       
			population[mem].rfitness;
	} 

	for (i = 0; i < POPSIZE; i++) { 
		p = (double)rand() /RAND_MAX;
		if (p < population[0].cfitness)
			newpopulation[i] = population[0];      
		else {
			for (j = 0; j < POPSIZE;j++)      
				if (p >= population[j].cfitness && 
						p<population[j+1].cfitness)
					newpopulation[i] = population[j+1];
		} 
	}


	/* Once a new population is created, copy it back */
	for (i = 0; i < POPSIZE; i++)
		population[i] = newpopulation[i];      
}

void TOURNAMENT(void)
{
	int i;

	for (i = 0; i < POPSIZE/2; i++) { 
		if( population[i].fitness >= population[i+POPSIZE/2].fitness) {
			newpopulation[i]=population[i];
			newpopulation[i+POPSIZE/2]=population[i];
		}
		else{
			newpopulation[i]=population[i+POPSIZE/2];
			newpopulation[i+POPSIZE/2]=population[i+POPSIZE/2];
		}

	
	}

	/* Once a new population is created, copy it back */
	for (i = 0; i < POPSIZE; i++)
		population[i] = newpopulation[i];      
}
/***************************************************************/
/* Crossover selection: selects two parents that take part in  */
/* the crossover. Implements a single point crossover.          */
/***************************************************************/

void crossover(void)
{
	int mem, one;
	int first  =  0;	/* Count of the number of members chosen */
	double x;

	for (mem = 0; mem < POPSIZE; ++mem) {
		x = (double)rand() / RAND_MAX;
		if (x < PXOVER) {
			++first;
			if (first % 2 == 0)
				Xover(one, mem);
			else
				one = mem;
		}
	}
}
/**************************************************************/
/* Crossover: performs crossover of the two selected parents. */
/**************************************************************/

void Xover(int one, int two)
{
	int i;
	int point;                	/* Crossover point */

	/* Select crossover point */
	if(NVARS > 1) {
		if(NVARS == 2)
			point = 1;
		else
			point = (rand() % (NVARS-1)) + 1;

		for (i = 0; i < point; i++)
			swap(&population[one].gene[i], &population[two].gene[i]);

	}
}

/*************************************************************/
/* Swap: A swap procedure that helps in swapping 2 variables */
/*************************************************************/

void swap(int *x, int *y)
{
	int temp;

	temp = *x;
	*x = *y;
	*y = temp;
}

/**************************************************************/
/* Mutation: Random uniform mutation.                         */
/**************************************************************/

void mutate(void)
{
	int i, j;
	double x;

	for (i = 0; i < POPSIZE; i++)
		for (j = 0; j < NVARS; j++) {
			x = (double)rand() /RAND_MAX;
			if (x < PMUTATION) {
				if (population[i].gene[j])
					population[i].gene[j] = 0;
				else	population[i].gene[j] = 1;

			}
		}
}

/***************************************************************/
/* Report function: Reports progress of the simulation. Data   */
/* dumped into the  output file are separated by commas.       */
/***************************************************************/

void report(void)
{
	int i;
	double best_val;		/* Best population fitness */
	double avg_fitness;	    /* Avg population fitness */
	double stddev;			/* Std. deviation of population fitness */
	double sum_square;		/* Sum of square for std. calc */
	double square_sum;		/* Square of sum for std. calc */
	double sum;				/* Total population fitness */

	sum = 0.0;
	sum_square = 0.0;

	for (i = 0; i < POPSIZE; i++)
	{
		sum += OFFSET-population[i].fitness;
		sum_square += (OFFSET-population[i].fitness) * (OFFSET-population[i].fitness);
	}

	avg_fitness = sum/(double)POPSIZE;
	square_sum = avg_fitness * avg_fitness * POPSIZE;
	stddev = sqrt((sum_square - square_sum)/(POPSIZE - 1));
	best_val = OFFSET-population[POPSIZE].fitness;

	fprintf(galog, "%5d       %10.9lf   %10.9lf   %10.9lf \n", generation, 
			best_val, avg_fitness, stddev);
}

/**************************************************************/
/* Main function: Each generation involves selecting the best */
/* members, performing crossover & mutation and then          */
/* evaluating the resulting population, until the terminating */
/* condition is satisfied                                     */
/**************************************************************/

int main(void)
{
	int i,j,n,oleastg,omostg, cnt=0;
	double x[NX], rate;
	double obest,oworst;

	if ((galog = fopen("galog.txt","w"))==NULL) {
		exit(1);
	}
	if ((solution = fopen("solution.txt","w"))==NULL) {
		exit(1);
	}

	avg_n_generation = 0;
	for(n=0; n<NRUNS; n++) {
		generation = 0;
		n_generation = 0;

		if(n==0){
			fprintf(galog, "\n Generation    Best        Average       Standard \n");
			fprintf(galog,   " number        value       fitness       deviation \n");
		}

		srand( (unsigned)time(NULL) );
		initialize();
		evaluate();
		keep_the_best();
		while(generation<MAXGENS) {
			generation++;
			
// Three methods
			Select();
			//RANKING();
			//TOURNAMENT();
//		
			crossover();
			mutate();
			if (n==0) report();
			evaluate();
			elitist();
		}

		if( population[POPSIZE].fitness >= OFFSET-OPTIMAL-ERRORRANGE ) {
			avg_n_generation += n_generation;
		} else {
			avg_n_generation += MAXGENS;
			n_generation = MAXGENS;
			cnt++;
		}
		fprintf(solution,"Best member of %d-th run : \n", n+1);

		for (i = 0; i < NVARS; i++) {
			fprintf (solution,"%d",population[POPSIZE].gene[i]);
		}
		fprintf(solution," => ");
        for (j = 0; j < NX ; j++){
			x[j]=0.;
			rate = 1.;
		    for (i = NVARS-NVARS*j/NX; i > NVARS-NVARS*(j+1)/NX ; i--) {
			x[j] += (double)population[POPSIZE].gene[i-1] * rate;
			rate *= 2.;
			}
			x[j] = FROM + x[j] * (TO-FROM) / ( rate - 1. );
            fprintf(solution,"   %10.9lf ", x[j]);
		}
		fprintf(solution," \n ");
		
		/* Find overall best , worst    */
		if (n==0) {
			obest = population[POPSIZE].fitness;
			oworst = population[POPSIZE].fitness;
			oleastg = n_generation;
			omostg =  n_generation;
		}
		else{
            if (population[POPSIZE].fitness >= obest) 
				obest = population[POPSIZE].fitness;
			
		    if (population[i+1].fitness <= oworst) 
				oworst = population[POPSIZE].fitness;

			if (n_generation <= oleastg) 
				oleastg = n_generation;

			if (n_generation >= omostg) 
				omostg = n_generation;
		}
					
		
		fprintf(solution,"Best fitness = %10.9lf",OFFSET-population[POPSIZE].fitness);
		fprintf(solution,"  for %d generation\n\n",n_generation);
		printf("%d-th run completed\n", n+1);
	}
	fprintf(solution, "Average Generation for Obtaining OPTIMAL Solution = %lf\n",
				(double)avg_n_generation/NRUNS);
	fprintf(solution, "Best solution = %10.9lf, Worst solution = %10.9lf\n", 70.-obest,70.-oworst);
	fprintf(solution, "Least generation = %d    , Most generation = %d\n", oleastg,omostg);
	fprintf(solution, "Number of 2000 generation = %d\n ", cnt);
	fclose(galog);
	fclose(solution);
	return 0;
}

/***************************************************************/


