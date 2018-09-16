/*
 * List.h
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#ifndef LIST_H_
#define LIST_H_

#include "Game.h"

/*Declaration of struct node*/
 struct node {
		int cmd; /*command of the node*/
		int numOfChanges; /*number of cells changed by command - each change is represented by 4 ints*/
		int numOfErrors; /*number of errors changed by command - each change is represented by 4 ints*/
		int * changes; /*array to hold the changes*/
		int * errorChanges; /*for saving error changes if set created errors*/
		node * next;
		node * prev;
};



void addToNode(gameData * game, int x, int y, int z, int type);

/*Receives a pointer to node and clears it and all the nodes after*/
void clearToEnd(node ** head);

/*
 * Erases list after curr node and inserts a new one.
 *
 * @param game - Game structure
 * @param cmd - Command executed: 0 - set, 1 - autofill, 2 - generate
 */
void insertAtCurr(gameData * game, int cmd);

/*Frees the list from the head*/
void freeList(gameData * game);

#endif /* LIST_H_ */
