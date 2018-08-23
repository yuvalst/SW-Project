/*
 * List.c
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */


/* Doubly Linked List implementation */

#include<stdio.h>
#include<stdlib.h>
#include "List.h"
#include "GameAux.h"



/*Creates a new Node and returns pointer to it*/
node * createNode() {
	node * newNode = (node*)malloc(sizeof(node));
	checkAlloc(newNode);
	newNode->numOfChanges = 0;
	newNode->numOfErrors = 0;
	newNode->changes = NULL;
	newNode->errorChanges = NULL;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

/*call this function before changing the gameboard*/
void addToNode(gameData * game, int x, int y, int z, int type) {
	if (type == 0) { /*change in board cell*/
		if (game->curr->changes == NULL){
			game->curr->changes = (int *)malloc(4 * sizeof(int));
			checkAlloc(game->curr->changes);
		}
		else {
			game->curr->changes = (int *)realloc(game->curr->changes, (game->curr->numOfChanges + 1) * 4 * sizeof(int));
			checkAlloc(game->curr->changes);
		}
		game->curr->changes[4 * game->curr->numOfChanges] = x;
		game->curr->changes[(4 * game->curr->numOfChanges) + 1] = y;
		if(game->curr->cmd == 2) { /*was generate cmd, ilp already changed board*/
			game->curr->changes[(4 * game->curr->numOfChanges) + 2] = 0;
		}
		else { /*set or autofill*/
			game->curr->changes[(4 * game->curr->numOfChanges) + 2] = game->board[x - 1][y - 1];
		}
		game->curr->changes[(4 * game->curr->numOfChanges) + 3] = z;
		game->curr->numOfChanges++;
	}
	else { /*error created/solved*/
		if (game->curr->errorChanges == NULL){
			game->curr->errorChanges = (int *)malloc(4 * sizeof(int));
			checkAlloc(game->curr->errorChanges);
		}
		else {
			game->curr->errorChanges = (int *)realloc(game->curr->errorChanges, (game->curr->numOfErrors + 1) * 4 * sizeof(int));
			checkAlloc(game->curr->errorChanges);
		}
		game->curr->errorChanges[4 * game->curr->numOfErrors] = x;
		game->curr->errorChanges[(4 * game->curr->numOfErrors) + 1] = y;
		game->curr->errorChanges[(4 * game->curr->numOfErrors) + 2] = game->board[x + game->bSize - 1][y - 1];
		game->curr->errorChanges[(4 * game->curr->numOfErrors) + 3] = z;
		game->curr->numOfErrors++;
	}
}

void clearToEnd(node ** head) {
	node * tail = *head;
	node * temp = NULL;
	while (tail != NULL) {
		temp = tail->next;
		if (tail->changes != NULL) {
			free((tail)->changes);
			tail->changes = NULL;
		}
		if (tail->errorChanges != NULL) {
			free(tail->errorChanges);
			tail->errorChanges = NULL;
		}
		free(tail);
		tail = temp;
	}
	*head = NULL;
}

/*Inserts a Node at head of doubly linked list*/
void insertAtCurr(gameData * game, int cmd) {
	struct node * newNode = createNode();
	newNode->cmd = cmd;
	if(game->head == NULL) {
		game->head = newNode;
		game->curr = newNode;
		return;
	}
	clearToEnd(&game->curr->next);
	game->curr->next = newNode;
	newNode->prev = game->curr;
	game->curr = newNode;
}




void freeList(gameData * game) {
	if (game->head != NULL) {
		clearToEnd(&game->head);
	}
}

/*
Prints all elements in linked list in reverse traversal order.
void ReversePrint() {
	struct Node* temp = head;
	if(temp == NULL) return; // empty list, exit
	// Going to last Node
	while(temp->next != NULL) {
		temp = temp->next;
	}
	// Traversing backward using prev pointer
	printf("Reverse: ");
	while(temp != NULL) {
		printf("%d ",temp->data);
		temp = temp->prev;
	}
	printf("\n");
}
*/
