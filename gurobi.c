#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "gurobi_c.h"

int ilp(gameData* game, double* sol, int * ind, double* val, double* obj ,char * vtype)
{
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int       error = 0;
	int       optimstatus;
	double    objval;

	int i,j,v,bS,rows,cols,ig,jg,bS3,count;



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

	/* coefficients - for x,y,k  */
	/* variable types - for x,y,z (cells 0,1,2 in "vtype") */
	for (i = 0; i < bS3; ++i)
	{
		obj[i] = 0;
		vtype[i] = GRB_BINARY;
	}


	/* add variables to model */
	error = GRBaddvars(model, bS3, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/* Change objective sense to maximization */
	/*error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}*/

	/* update the model - to integrate new variables */

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		return 0;
	}


	/* First constraint: fixed cells */

	for (i = 0; i < bS; i++) {
		for (j = 0; j < bS; j++) {
			for (v = 0; v < bS; v++) {
				if (game->board[i][j] == v + 1){
					ind[0] = i*bS*bS+j*bS+v;      /*might need to change*/
					val[0] = 1.0;
					error = GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1.0, "fixed");
					if (error) {
						printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
						return 0;
					}
				}
			}
		}
	}

	/* Second constraint: Each cell gets a value */

	for (i = 0; i < bS; i++) {
		for (j = 0; j < bS; j++) {
			for (v = 0; v < bS; v++) {
				ind[v] = i*bS*bS + j*bS + v;
				val[v] = 1.0;
			}
			error = GRBaddconstr(model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return 0;
			}
		}
	}

	/* Third constraint: Each value must appear once in each row */

	for (j = 0; j < bS; j++) {
		for (v = 0; v < bS; v++) {
			for (i = 0; i < bS; i++) {
				ind[i] = i*bS*bS + j*bS + v;
				val[i] = 1.0;
			}

			error = GRBaddconstr(model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return 0;
			}
		}
	}

	/* 4th constraint: Each value must appear once in each column */

	for (i = 0; i < bS; i++) {
		for (v = 0; v < bS; v++) {
			for (j = 0; j < bS; j++) {
				ind[j] = i*bS*bS + j*bS + v;
				val[j] = 1.0;
			}

			error = GRBaddconstr(model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return 0;
			}
		}
	}

	/* 5th constraint: Each value must appear once in each subgrid */

	for (v = 0; v < bS; v++) {
		for (ig = 0; ig < cols; ig++) {
			for (jg = 0; jg < rows; jg++) {
				count = 0;
				for (i = ig*rows; i < (ig+1)*rows; i++) {
					for (j = jg*cols; j < (jg+1)*cols; j++) {
						ind[count] = i*bS*bS + j*bS + v;
						val[count] = 1.0;
						count++;
					}
				}
				error = GRBaddconstr(model, bS, ind, val, GRB_EQUAL, 1.0, NULL);
				if (error) {
					printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					return 0;
				}
			}
		}
	}




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
		printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
		/*return 0;*/
	}

	/* get the solution - the assignment to each variable */
	/* 3-- number of variables, the size of "sol" should match */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, bS3, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		/*return 0;*/
	}

	/* print results */
	printf("\nOptimization complete\n");

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		for (i = 0; i < bS; i++) {
			for (j = 0; j < bS; j++) {
				for (v = 0; v < bS; v++) {
					/*ind[v] = i*bS*bS + j*bS + v;
				  val[v] = 1.0;*/
					if (sol[i*bS*bS+j*bS+v]==1){
						game->board[i][j] = v+1;                   /* maybe need to mark as fixed?????*/
					}
				}

			}
		}
		return 1;
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("Model is infeasible or unbounded\n");
		/*return 0;*/
	}
	/* error or calculation stopped */
	else {
		printf("Optimization was stopped early\n");
		/*return 0;*/
	}

	/* IMPORTANT !!! - Free model and environment */
	GRBfreemodel(model);
	GRBfreeenv(env);
	return 0;
}


int gurobi_solver(gameData* game){
	int res,bS,bS3;
	double * sol, *val, *obj;
	int* ind;
	char* vtype;
	bS = game->bSize;
	bS3 = bS*bS*bS;
	if (!game){
		return -1;
	}
	sol  = (double *) calloc (bS3, sizeof(double));
	ind  = (int *) calloc (bS * bS, sizeof(int));
	val  = (double *) calloc (bS * bS, sizeof(double));
	obj  = (double *) calloc (bS3, sizeof(double));
	vtype  = (char *) calloc (bS3, sizeof(char));
	/*assert*/
	res = ilp(game,sol,ind,val,obj,vtype);
	free(sol);
	free(obj);
	free(vtype);
	free(ind);
	free(val);
	return res;
}
