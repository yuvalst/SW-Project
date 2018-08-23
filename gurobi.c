#include <stdlib.h>
#include <stdio.h>
#include "gurobi.h"
#include "Game.h"
#include "GameAux.h"
#include "gurobi_c.h"

#define GUR_ERROR "Gurobi error! Exiting\n"


int addFixedConstraints (gameData * game, GRBenv ** env, GRBmodel ** model, int * error, int * ind, double * val, int bS) {
	int i, j, v;
	for (i = 0; i < bS; i++) {
		for (j = 0; j < bS; j++) {
			for (v = 0; v < bS; v++) {
				if (game->board[i][j] == v + 1){
					ind[0] = i*bS*bS + j*bS + v;
					val[0] = 1.0;
					*error = GRBaddconstr(*model, 1, ind, val, GRB_EQUAL, 1.0, "fixed");
					if (*error) {
						printf("ERROR %d 1st GRBaddconstr(): %s\n", *error, GRBgeterrormsg(*env));
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

int addConstraints (GRBenv ** env, GRBmodel ** model, int * error, int * ind, double * val, int bS, int a, int b, int c) {
	int i, j, v;
	for (i = 0; i < bS; i++) {
		for (j = 0; j < bS; j++) {
			for (v = 0; v < bS; v++) {
				ind[v] = i*a + j*b + v*c;
				val[v] = 1.0;
			}
			*error = GRBaddconstr(*model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
			if (*error) {
				printf("ERROR %d 1st GRBaddconstr(): %s\n", *error, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}
	return 1;
}

int addBlockConstraints (GRBenv ** env, GRBmodel ** model, int * error, int * ind, double * val, int bS, int cols, int rows) {
	int i, j, v, blockRow, blockCol, count;
	for (v = 0; v < bS; v++) {
		for (blockRow = 0; blockRow < cols; blockRow++) {
			for (blockCol = 0; blockCol < rows; blockCol++) {
				count = 0;
				for (i = blockRow*rows; i < (blockRow+1)*rows; i++) {
					for (j = blockCol*cols; j < (blockCol+1)*cols; j++) {
						ind[count] = i*bS*bS + j*bS + v;
						val[count] = 1.0;
						count++;
					}
				}
				*error = GRBaddconstr(*model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
				if (*error) {
					printf("ERROR %d 1st GRBaddconstr(): %s\n", *error, GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}
	return 1;
}

int solution (gameData * game, int * optimstatus, double * sol, int bS) {
	int i, j, v;
	if (*optimstatus == GRB_OPTIMAL) {
		for (i = 0; i < bS; i++) {
			for (j = 0; j < bS; j++) {
				for (v = 0; v < bS; v++) {
					if (sol[i*bS*bS+j*bS+v]==1){
						game->board[i][j] = v + 1;
					}
				}

			}
		}
		return 1;
	}
	/* no solution found */
	else if (*optimstatus == GRB_INF_OR_UNBD) {
		/*printf("Model is infeasible or unbounded\n");*/
		return -1;
	}
	/* error or calculation stopped */
	else {
		/*printf("Optimization was stopped early\n");*/
		return -1;
	}
}



int ilp(gameData* game, double* sol, int * ind, double* val, double* obj ,char * vtype)
{
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int       error = 0;
	int       optimstatus;
	double    objval;

	int i, bS, rows, cols, bS3, res = 0;

	bS = game->bSize;
	bS3 = bS*bS*bS;
	cols = game->n;
	rows = game->m;

	/* Create environment - log file is ilp1.log */
	error = GRBloadenv(&env, "ilp1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/* Create an empty model named "ilp1" */
	error = GRBnewmodel(env, &model, "ilp1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}


	/* Add variables */
	/* coefficients and variable types for cells */
	for (i = 0; i < bS3; ++i)
	{
		obj[i] = 0;
		vtype[i] = GRB_BINARY;
	}
	error = GRBaddvars(model, bS3, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL); /* add variables to model */
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}


	/* First constraint: fixed cells */
	addFixedConstraints(game, &env, &model, &error, ind, val, bS);

	/* Second constraint: Each cell gets a value */
	addConstraints(&env, &model, &error, ind, val, bS, bS*bS, bS, 1);

	/* Third constraint: Each value must appear once in each row */
	addConstraints(&env, &model, &error, ind, val, bS, bS, 1, bS*bS);


	/* 4th constraint: Each value must appear once in each column */
	addConstraints(&env, &model, &error, ind, val, bS, bS*bS, 1, bS);


	/* 5th constraint: Each value must appear once in each block */
	addBlockConstraints(&env, &model, &error, ind, val, bS, cols, rows);

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/* Write model to 'ilp1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "ilp1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		/*return 0;*/
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		/*return 0;*/
	}

	/* get the objective -- the optimal result of the function */
	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		/*printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));*/
		return -1;
	}

	/* get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, bS3, sol);
	if (error) {
		/*printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));*/
		return -1;
	}

	/* get solution */
	res  = solution(game, &optimstatus, sol, bS);
	/* Free model and environment */
	GRBfreemodel(model);
	GRBfreeenv(env);
	return res;
}


int gurobi_solver(gameData* game){
	int res, bS, bS3;
	double * sol, *val, *obj;
	int* ind;
	char* vtype;
	bS = game->bSize;
	bS3 = bS*bS*bS;
	sol  = (double *) calloc (bS3, sizeof(double));
	checkAlloc(sol);
	ind  = (int *) calloc (bS * bS, sizeof(int));
	checkAlloc(ind);
	val  = (double *) calloc (bS * bS, sizeof(double));
	checkAlloc(val);
	obj  = (double *) calloc (bS3, sizeof(double));
	checkAlloc(obj);
	vtype  = (char *) calloc (bS3, sizeof(char));
	checkAlloc(vtype);
	res = ilp(game,sol,ind,val,obj,vtype);
	free(sol);
	free(obj);
	free(vtype);
	free(ind);
	free(val);
	if (res == -1) {
		return 0;
	}
	else if (res == 0) {
		printf(GUR_ERROR);
		exit(0);
	}
	return res;
}
