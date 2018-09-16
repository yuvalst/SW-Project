/*
 * List.h
 *
 * Module representing the command linked list.
 *
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


 /*
  * Used to add board changes to the curr node in the command list. Call this function before changing the game board.
  *
  * @param game- Game structure
  * @param x - col of cell as user sees it (starting at 1)
  * @param y - row of cell as user sees it (starting at 1)
  * @param z - value changed to
  * @param type - 0 - change happened to a cell value, 1 - change happened to a cell's status (erroneous)
  *
  */
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
