/*
 * GameAux.h Summary:
 *
 * Functions assisting the functions of the Game module.
 *
 */

#ifndef GAMEAUX_H_
#define GAMEAUX_H_

#include "Game.h"

/*checks memory allocation for the pointer p*/
void checkAlloc(void * p);

/*checks if fscanf succeeded*/
int checkScan(int check, int expect);

/*checks if fprintf succeeded*/
int checkPrint(int check);

/* Used to initialize a new gameData object with default fields.*/
gameData * initGame();

/* Used to free the memory of a gameData object*/
void freeGame(gameData* game);

/*Used to free the memory of a gameData object and also the pointer gameC*/
void freeGameC(gameData * gameC);

/*
 * Receives pointer to gameC and game and copies the attributes of game to gameC
 *
 * @param gameC - Empty game structure
 * @param game - Game structure to be copied
 *
 */
void copyGame(gameData ** gameC, gameData * game);

/* Receives the game structure and int and creates a new puzzle board according to game->bSize, changes mode according to the int.
 *
 * @param game - Game structure
 * @param mode - Requested game mode: 0 - init, 1 - solve, 2 - edit
 *
 */
void newGame(gameData * game, int mode);

/*Prints row separator*/
void printRowSep(gameData * game);

/*Receives game,x,y,z - if z is valid value for cell <x,y> the function will return 1, else 0*/
int checkValid(gameData * game, int x, int y, int z);

/*Receives game and updates the cells with errors and the error count*/
void updateErrors(gameData * game);

/*Used after set and checks only cells which could be affected by it. Updates the cells with errors and the error count*/
int updateSetErrors(gameData * game, int x, int y, int prev, int z);

/*Used to update numEmpty after value change*/
void updateEmpty(gameData * game, int prev, int z);

/*Checks if string represents an int, if true returns 1 else 0*/
int checkInt(char * cmd);

/* Receives string array and checks if the the strings represent ints in the required range.
 *
 * @param cmdArr - String array
 * @param min - Minimum value permitted
 * @param max - Maximum value permitted
 * @param len - number of arguments expected in cmdArr
 *
 * @return
 *  1 - all arguments are valid and in range
 *  0 - else
 */
int checkArgs(char ** cmdArr, int min, int max, int len);

/*Returns 1 if cell <x,y> is fixed, else 0*/
int checkFixed(gameData * game, int x, int y);

/*Returns 1 if the puzzle is solved, else 0*/
int checkIfSolved(gameData * game);

/*Returns 1 if cell <i,j> has only 1 valid value, else 0*/
int singleValue(gameData * gameC, int i, int j);

/*Returns 1 if the board is solvable else 0*/
int ilpSolver(gameData * game);

/*Returns the number of possible board solutions*/
int exhaustiveBT(gameData * gameC);

/* Receives game structure, int X and pointer to int res. Tries to fill X cells with random legal values. If succeeded, changes the int pointed
 * to by res to 1, else 0.
 */
void fillXcells(gameData * game, int x, int * res);




#endif /* GAMEAUX_H_ */
