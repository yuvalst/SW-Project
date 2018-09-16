/*
 * Stack.c Summary:
 *
 * Module representing the stack used for exhaustiveBT.
 * each cell is represented by 3 ints in stk->arr which represnt x,y nad value
 * the stack object has 2 ints to represnt when and whats the current last cell
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"
#include "GameAux.h"


void initStack(gameData * game, stack *stk)
{
   stk->top = EMPTY; /* -1 */
   stk->full = game->bSize * game->bSize - 1;
   stk->arr = (int*)calloc(3*(stk->full + 2),sizeof(int));
   checkAlloc(stk->arr);
}

void push(stack *stk, int i, int j, int z) {
    stk->top++; /*increment index*/
    stk->arr[stk->top*3] = i;
    stk->arr[stk->top*3+1] = j;
    stk->arr[stk->top*3+2] = z;
}

/* Receives stack and int pointer and calls top to update cell, 
but also removes the last cell from the stack by decreasing top*/
void pop(stack *stk, int* cell) {
  top(stk, cell);
  stk->top--;
}

void top(stack *stk, int* cell) {
	cell[0] = stk->arr[stk->top*3];
	cell[1] = stk->arr[stk->top*3+1];
	cell[2] = stk->arr[stk->top*3+2];
}


int isEmpty(stack *stk) {
	return ((stk->top == EMPTY));
}


int isFull(stack *stk) {
	return ((stk->top == stk->full));
}

/* Receives stack and frees allocated memory of the array, the stack itself and its pointer */
void freeStack(stack *stk) {
	if (stk->arr != NULL) {
		free(stk->arr);
		stk->arr = NULL;
	}
	if (stk != NULL) {
		free(stk);
		stk = NULL;
	}
}
