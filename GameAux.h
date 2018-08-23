/*
 * GameAux.h
 *
 *  Created on: 21 Aug 2018
 *      Author: guywaldman
 */

#ifndef GAMEAUX_H_
#define GAMEAUX_H_

#include "Game.h"

void checkAlloc(void * p);
gameData * initGame();
void freeGame(gameData* game);
void freeGameC(gameData * gameC);
void copyGame(gameData ** gameC, gameData * game);
void newGame(gameData * game, int mode);
void printRowSep(gameData * game);
int checkValid(gameData * game, int x, int y, int z);
void updateErrors(gameData * game);
int updateSetErrors(gameData * game, int x, int y, int prev, int z);
void updateEmpty(gameData * game, int prev, int z);
int checkInt(char * cmd);
int checkArgs(char ** cmdArr, int min, int max, int len);
int checkFixed(gameData * game, int x, int y);
int checkIfSolved(gameData * game);
int singleValue(gameData * gameC, int i, int j);
int ilpSolver(gameData * game);
int exhaustiveBT(gameData * gameC);




#endif /* GAMEAUX_H_ */
