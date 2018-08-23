/*
 * Game.h
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#ifndef GAME_H_
#define GAME_H_




typedef struct node node;

typedef struct gameData {
	int mode; /*0 - init, 1 - solve, 2 - edit*/
	int mark;
	int errors;
	int m;
	int n;
	int bSize;
	int numEmpty; /*number of empty cells*/
	int ** board;
	node * head;
	node * curr;
}gameData;



int solve(gameData * game, char * path);
int edit(gameData * game, char* path);
int markErrors(gameData * game, char ** cmdArr);
void printBoard(gameData * game);
int set(gameData * game, char ** cmdArr);
int validate(gameData * game, int p);
int generate(gameData * game, char ** cmdArr);
int undo(gameData * game, int p);
int redo(gameData * game);
int save(gameData * game, char * path);
int hint(gameData * game, char ** cmdArr);
int numSols(gameData * game);
int autofill(gameData * game);
int reset(gameData * game);
void exitGame(gameData * game);

#endif /* GAME_H_ */
