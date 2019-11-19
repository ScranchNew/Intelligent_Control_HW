/*

ST2FLC for wall following without any sensory noises
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
//#define PI 3.14159265359

double LW_L = 0, LW_U = 0, RW_L = 0, RW_U = 0, RW = 0, LW = 0; /* Global dectaration for outputs as N-o-OP>1
/************************************************************************************/

typedef struct in_mem
{
	double *width;  // Input membership function width (1/2 of triangle base).
	double *center; // Center of each input membership function.
	double *FoU;	// FoU values for each input membership function
	double *dom_U;  // Degree of membership for each upper membership function.
	double *dom_L;  // Degree of membership for each lower membership function.
} IN_MEM;

typedef struct out_mem
{
	double *width;  /* Output membership function width (1/2 of triangle base). */
	double *center; /* Center of each output membership function.   */
	double *FoU;	// FoU values for each output membership function
} OUT_MEM;

typedef struct fuz_sys
{
	IN_MEM *emem; /* Groups all fuzzy system parameters in a single variable. */
	IN_MEM *edotmem;
	OUT_MEM *outmem_LM;
	OUT_MEM *outmem_RM;
} FUZ_SYS;

/************************************************************************************/

/* Function Prototypes: */

// Expanded the init function with a type parameter
void fuzzy_init(FUZ_SYS *fuzzy_system, int sys_type);
// Added a function to create the control output
double control_output();

void fuzzy_free(FUZ_SYS *fuzzy_system);
double fuzzy_control(double e, double edot, FUZ_SYS *fuzzy_system);
void fuzzyify(double u, IN_MEM *mem);
double leftall(double u, double w, double c, double f);
double rightall(double u, double w, double c, double f);
double triangle(double u, double w, double c, double f);
double gaussian(double u, double w, double c, double f);
void match(const IN_MEM *emem, const IN_MEM *edotmem, int *pos_U, int *pos_L);
double inf_defuzz(IN_MEM *emem, IN_MEM *edotmem, OUT_MEM *outmem_LM, OUT_MEM *outmem_RM, int *pos, int flg);

/************************************************************************************/

void fuzzy_init(FUZ_SYS *fuzzy_system, int sys_type = 1)
{

	/* Define the input and output membership functions. */

	int i;
	/* Allocate memory for membership functions. */
	if (!(fuzzy_system->emem = (IN_MEM *)malloc(sizeof(IN_MEM))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem = (IN_MEM *)malloc(sizeof(IN_MEM))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_LM = (OUT_MEM *)malloc(sizeof(OUT_MEM))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_RM = (OUT_MEM *)malloc(sizeof(OUT_MEM))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->emem->center = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->emem->dom_U = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->emem->dom_L = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->emem->width = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->emem->FoU = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem->center = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem->dom_U = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem->dom_L = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem->width = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->edotmem->FoU = (double *)malloc(4 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_LM->center = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_LM->width = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_LM->FoU = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_RM->center = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_RM->width = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	if (!(fuzzy_system->outmem_RM->FoU = (double *)malloc(5 * sizeof(double))))
	{
		printf("Error allocating memory.\n");
		exit(1);
	}
	/* Initialize parameters */

#include "fuzzy_param.h"

	// All membership functions have a center a width and a field of uncertainty

	for (i = 0; i < MAX(input_dim, output_dim); i++)
	{
		if (i < input_dim)
		{
			fuzzy_system->emem->center[i] = INP_cent[i];
			fuzzy_system->edotmem->center[i] = INP_cent[i];
			fuzzy_system->emem->width[i] = INP_wid[i];
			fuzzy_system->edotmem->width[i] = INP_wid[i];
			// If the type is type 2 we set FoU to 0
			if (sys_type == 2) {
				fuzzy_system->emem->FoU[i] = 0;
				fuzzy_system->edotmem->FoU[i] = 0;
			} else {
				fuzzy_system->emem->FoU[i] = INP_FU[i];
				fuzzy_system->edotmem->FoU[i] = INP_FU[i];
			}
		}
		if (i < output_dim)
		{
			fuzzy_system->outmem_LM->center[i] = OUT_cent[i];
			fuzzy_system->outmem_RM->center[i] = OUT_cent[i];
			fuzzy_system->outmem_LM->width[i] = OUT_wid[i];
			fuzzy_system->outmem_RM->width[i] = OUT_wid[i];
			if (sys_type == 2) {
				fuzzy_system->outmem_LM->FoU[i] = 0;
				fuzzy_system->outmem_LM->FoU[i] = 0;
			}
			fuzzy_system->outmem_LM->FoU[i] = OUT_FU[i];
			fuzzy_system->outmem_LM->FoU[i] = OUT_FU[i];
		}
	}
}
/************************************************************************************/

void fuzzy_free(FUZ_SYS *fuzzy_system)
{

	/* Free memory allocated in fuzzy_init(). */

	/*  free(fuzzy_system->emem->center);
  free(fuzzy_system->emem->dom);
  free(fuzzy_system->edotmem->center);
  free(fuzzy_system->edotmem->dom);
  free(fuzzy_system->outmem->center);
  free(fuzzy_system->emem);
  free(fuzzy_system->edotmem);
  free(fuzzy_system->outmem);
*/
}

/************************************************************************************/

	/* Given crisp inputs e and edot, determine the crisp output u. */
double fuzzy_control(double e1, double e2, FUZ_SYS *fuzzy_system)
{


	int pos_U[2], pos_L[2], pos[2];
	double blah;
	fuzzyify(e1, fuzzy_system->emem);
	fuzzyify(e2, fuzzy_system->edotmem);
	match(fuzzy_system->emem, fuzzy_system->edotmem, pos_U, pos_L);
	pos[0] = pos_U[0];
	pos[1] = pos_U[1];
	blah = inf_defuzz(fuzzy_system->emem, fuzzy_system->edotmem, fuzzy_system->outmem_LM, fuzzy_system->outmem_RM, pos, 1);
	pos[0] = pos_L[0];
	pos[1] = pos_L[1];
	blah = inf_defuzz(fuzzy_system->emem, fuzzy_system->edotmem, fuzzy_system->outmem_LM, fuzzy_system->outmem_RM, pos, -1);
	LW = (LW_U + LW_L) / 2;
	RW = (RW_U + RW_L) / 2;

	return 0;
}

// return the crisp output
double control_output(){
	return (LW + RW) / 2;
}
/************************************************************************************/

void fuzzyify(double u, IN_MEM *mem)
{

	/* Fuzzify the input u by determining the degree of membership for each membership
   function in mem. Assumes 5 membership functions, with first and last membership
   functions leftall and rightall respectively.  Other membership functions are
   triangular. */

	// Sets the degree of membership of the input with the membership functions with and without uncertainty
	mem->dom_U[0] = leftall(u, mem->width[0], mem->center[0], 0);
	mem->dom_U[1] = gaussian(u, mem->width[1], mem->center[1], 0);
	mem->dom_U[2] = triangle(u, mem->width[2], mem->center[2], 0);
	mem->dom_U[3] = rightall(u, mem->width[3], mem->center[3], 0);
	mem->dom_L[0] = leftall(u, mem->width[0], mem->center[0], mem->FoU[0]);
	mem->dom_L[1] = gaussian(u, mem->width[1], mem->center[1], mem->FoU[1]);
	mem->dom_L[2] = triangle(u, mem->width[2], mem->center[2], mem->FoU[2]);
	mem->dom_L[3] = rightall(u, mem->width[3], mem->center[3], mem->FoU[3]);
}

/************************************************************************************/

double leftall(double u, double w, double c, double f)
{

	/* Determine degree of membership for a leftall (left shoulder) membership function.
   NOTE:  u is input, c is mem. fun. center, and w is mem. fun. width. */

	if (u < c)
		return 1.0;
	else
		return MAX(0, (1 - (u - (c - f)) / w));
}

/************************************************************************************/

double rightall(double u, double w, double c, double f)
{

	/* Determine degree of membership for a RIGHTALL (right shoulder) membership function
   NOTE:  u is input, c is mem. fun. center, and w is mem. fun. width. */

	if (u >= c)
		return 1.0;
	else
		return MAX(0, (1 - ((c + f) - u) / w));
}

/************************************************************************************/

double triangle(double u, double w, double c, double f)
{

	/* Determine degree of membership for a TRIANGLE membership function
   NOTE:  u is input, c is mem. fun. center, and w is mem. fun. width. */

	if (u >= c)
		return MAX(0, (1 - (u - c) / (w - f)));
	else
		return MAX(0, (1 - (c - u) / (w - f)));
}
/************************************************************************************/

double gaussian(double u, double w, double c, double f)
{
	// Gaussian membership function
	// CAUTION: For practical purposes, a 'close to zero' cut off to force the membership to zero  as GaussianMF theoretically never goes to zero
	double X, Y;
	X = pow((u - c), 2) / (2 * pow((w - f), 2));
	Y = exp(-X);

	return MAX(0, Y);
}
/************************************************************************************/

void match(const IN_MEM *emem, const IN_MEM *edotmem, int *pos_U, int *pos_L) {

/* For each universe of discourse, determine the index of the first membership function
   with a non-zero degree (i.e. match the rules to the current inputs to find which rules 
   are on).  These indices are used to determine which four rules to evaluate.  (NOTE: 
   A 2 input sytem with no more than 50% overlap for input membership functions only
   requires the evaluation of at most 4 rules.) */ 

#include "fuzzy_param.h"

	int i;
	for (i=0; i<input_dim; i++) {
		if(emem->dom_U[i] > 0.05) {
			pos_U[0] = i;
			break;
		}
	}
	for (i=0; i<input_dim; i++) {
		if(emem->dom_L[i] > 0.04) {
			pos_L[0] = i;
			break;
		}
	}
	for (i=0; i<input_dim; i++) {
		if(edotmem->dom_U[i] > 0.05) {
			pos_U[1] = i;
			break;
		}
	}
	for (i=0; i<input_dim; i++) {
		if(edotmem->dom_L[i] > 0.04) {
			pos_L[1] = i;
			break;
		}
	}
}

/************************************************************************************/

double inf_defuzz(IN_MEM *emem, IN_MEM *edotmem, OUT_MEM *outmem_LM, OUT_MEM *outmem_RM, int *pos, int Flg)
{

#include "fuzzy_rules.h"
#include "fuzzy_param.h"

	double outdom, area_R, Atot_R = 0, WAtot_R = 0;
	double area_L, Atot_L = 0, WAtot_L = 0;
	int i, j, out_index_L = 0, out_index_R = 0, k;

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			if (((pos[0] + i) < input_dim) && ((pos[1] + j) < input_dim))
			{ /* Check that bounds are not exceeded. */
				outdom = 0;
				/* ...................................... Consequent selection the Rule Base for this code .............................. */
				for (k = 0; k < rule_dim_1; k++)
				{
					if ((pos[0] + i) == ((rules[k][0] - 1)) && ((pos[1] + j)) == (rules[k][1] - 1))
					{
						out_index_L = rules[k][2] - 1;
						out_index_R = rules[k][3] - 1;
						// Added debugging messages
						if (Flg == 1){
							std::cout << k << "|"<< pos[0] + i + 1 << "; " << pos[1] + j + 1<< "; " << out_index_L + 1 << "; " <<out_index_R + 1 << "|";
						}break;
					}
				}
				/* ............................. Determine the upper and lowerfuzzy outputs ............................................. */
				if (Flg > 0)
				{ // check for upper sets
					outdom = MIN((emem->dom_U[pos[0] + i]), (edotmem->dom_U[pos[1] + j]));
					area_L = 2 * outmem_LM->width[out_index_L] * (outdom - (outdom * outdom) / 2);
					area_R = 2 * outmem_RM->width[out_index_R] * (outdom - (outdom * outdom) / 2);
				}
				else
				{
					outdom = MIN((emem->dom_L[pos[0] + i]), (edotmem->dom_L[pos[1] + j]));
					area_L = 2 * (outmem_LM->width[out_index_L] - outmem_LM->FoU[out_index_L]) * (outdom - (outdom * outdom) / 2);
					area_R = 2 * (outmem_RM->width[out_index_R] - outmem_RM->FoU[out_index_R]) * (outdom - (outdom * outdom) / 2);
				}
				/* ...... Defuzzify ......... */
				Atot_L += area_L;
				Atot_R += area_R;
				WAtot_L += area_L * outmem_LM->center[out_index_L];
				WAtot_R += area_R * outmem_RM->center[out_index_R];
			}
		}
	}

	// More debugging messages
	if (out_index_L == 0 && out_index_R == 0) {
		std::cout << "No matching rule found for " << pos[0] + 1 << " and " << pos[1] + 1 << std::endl;
	}

	if (Flg > 0)
	{
		LW_U = (WAtot_L / Atot_L);
		RW_U = (WAtot_R / Atot_R);
	}
	else
	{
		LW_L = (WAtot_L / Atot_L);
		RW_L = (WAtot_R / Atot_R);
	}
	return 0;
}

/************************************************************************************/

#if 0 /* Set to zero to call fuzzy_control() from another program. */

void main(void)
{

	double e1 = 0, e2 = 0, u; // initialize input and other variables

	FUZ_SYS fuzzy_system;	  // Declaring the fuzzy structure
	fuzzy_init(&fuzzy_system); // Fuzzy initialization

	u = fuzzy_control(e1, e2, &fuzzy_system);

	fuzzy_free(&fuzzy_system);
}
#endif

/************************************************************************************/
