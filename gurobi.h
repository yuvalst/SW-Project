/*
 * gurobi.h
 *
 */

#ifndef GUROBI_H_
#define GUROBI_H_

#include "Game.h"

/*Receives game and returns 1 if the board is solvable else 0*/
int gurobi_solver(gameData *game);

#endif /* GUROBI_H_ */
