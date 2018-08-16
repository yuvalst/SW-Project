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
	int n;
	int m;
	int bSize;
	int numEmpty; /*number of empty cells*/
	int ** board;
	node * head;
	node * curr;
}gameData;



gameData * initGame();


#endif /* GAME_H_ */
