/*
 * List.h
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#ifndef LIST_H_
#define LIST_H_

#include "Game.h"

 struct node {
		int cmd;
		int numOfChanges; /*number of cells changed by command - each change is represented by 4 ints*/
		int numOfErrors;
		int * changes;
		int * errorChanges; /*for saving error changes if set created errors*/
		node * next;
		node * prev;
};


 void clearToEnd(node ** head);
void addToNode(gameData * game, int x, int y, int z, int type);
void insertAtCurr(gameData * game, int cmd);
void freeList(gameData * game);

#endif /* LIST_H_ */
