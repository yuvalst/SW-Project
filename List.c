/*
 * List.c
 *
 * Module representing the command linked list.
 */



#include<stdio.h>
#include<stdlib.h>
#include "List.h"
#include "GameAux.h"



/*Creates a new Node and returns pointer to it. Used by insertAtCurr.*/
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


void addToNode(gameData * game, int x, int y, int z, int type) {
	if (type == 0) { /*change in board cell*/
		if (game->curr->changes == NULL){ /*wasn't allocated already*/
			game->curr->changes = (int *)malloc(4 * sizeof(int));
			checkAlloc(game->curr->changes);
		}
		else {
			game->curr->changes = (int *)realloc(game->curr->changes, (game->curr->numOfChanges + 1) * 4 * sizeof(int));
			checkAlloc(game->curr->changes);
		}
		game->curr->changes[4 * game->curr->numOfChanges] = x;
		game->curr->changes[(4 * game->curr->numOfChanges) + 1] = y;
		if(game->curr->cmd == 2) { /*was generate command, so ilp already changed board*/
			game->curr->changes[(4 * game->curr->numOfChanges) + 2] = 0;
		}
		else { /*set or autofill*/
			game->curr->changes[(4 * game->curr->numOfChanges) + 2] = game->board[x - 1][y - 1];
		}
		game->curr->changes[(4 * game->curr->numOfChanges) + 3] = z;
		game->curr->numOfChanges++;
	}
	else { /*change in cell status - error created/solved*/
		if (game->curr->errorChanges == NULL){ /*wasn't allocated already*/
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
	while (tail != NULL) { /*keep erasing to the end*/
		temp = tail->next;
		if (tail->changes != NULL) { /*free array of changes*/
			free((tail)->changes);
			tail->changes = NULL;
		}
		if (tail->errorChanges != NULL) { /*free array of error changes*/
			free(tail->errorChanges);
			tail->errorChanges = NULL;
		}
		free(tail);
		tail = temp;
	}
	*head = NULL;
}


void insertAtCurr(gameData * game, int cmd) {
	struct node * newNode = createNode();
	newNode->cmd = cmd;
	if(game->head == NULL) {
		game->head = newNode;
		game->curr = newNode;
		return;
	}
	clearToEnd(&game->curr->next); /*clear next nodes*/
	game->curr->next = newNode; /*insert the new node*/
	newNode->prev = game->curr;
	game->curr = newNode;
}



/*Frees the list from the head*/
void freeList(gameData * game) {
	if (game->head != NULL) {
		clearToEnd(&game->head);
	}
}


