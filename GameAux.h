/*
 * GameAux.h
 *
 *  Created on: 21 Aug 2018
 *      Author: guywaldman
 */

#ifndef GAMEAUX_H_
#define GAMEAUX_H_

#include "Game.h"

/*checks memory allocation for the pointer p*/
void checkAlloc(void * p);
/*used to initialize the new gameData object*/
gameData * initGame();
/*used to free the memory of a gameData object*/
void freeGame(gameData* game);
/*used to free the memory of a gameData object and also the pointer gameC*/
void freeGameC(gameData * gameC);
/*Receives pointer to gameC and game and copys the gameData of game to gameC*/
void copyGame(gameData ** gameC, gameData * game);
/*Receives the game data and int for mode and create a board and mode that matches to the data*/
void newGame(gameData * game, int mode);
/*prints row seperator*/
void printRowSep(gameData * game);
/*Receives game,x,y,z - if z is valid number for cell[x,y] in game.board the funciton will return 1 else 0*/
int checkValid(gameData * game, int x, int y, int z);
/*Receives game and updates the cell with errors and the error count*/
void updateErrors(gameData * game);
/*Used after Set to update the cells with errors and the errors count*/
int updateSetErrors(gameData * game, int x, int y, int prev, int z);
/*Used to update numEmpty after value change*/
void updateEmpty(gameData * game, int prev, int z);
/*checks if string represnt an int if true returns 1 else 0*/
int checkInt(char * cmd);
/*Receives command array, min, max and len*/
int checkArgs(char ** cmdArr, int min, int max, int len);
/*return 1 if cell[x,y] in game.board is fixed else 0*/
int checkFixed(gameData * game, int x, int y);
/*return 1 if the puzzle is solved else 0*/
int checkIfSolved(gameData * game);
/*returns 1 if game.board[i,j] has only 1 valid value else 0*/
int singleValue(gameData * gameC, int i, int j);
/*returns 1 if the board is solvable else 0*/
int ilpSolver(gameData * game);
/*returns the number of possible board solutions*/
int exhaustiveBT(gameData * gameC);
/*Receives game, number of cells to fill, and int pointer to the result, 
the funciton returns 1 if filling x cells with random values succeeded*/
void fillXcells(gameData * game, int x, int * res);




#endif /* GAMEAUX_H_ */
