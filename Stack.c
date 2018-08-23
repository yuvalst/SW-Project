
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"
#include "GameAux.h"


void initStack(gameData * game, stack *stk)
{
   stk->top = EMPTY;
   stk->full = game->bSize * game->bSize - 1;
   stk->arr = (int*)calloc(3*(stk->full + 2),sizeof(int));
   checkAlloc(stk->arr);
}

void push(stack *stk, int i, int j, int z) {
    stk->top++;
    stk->arr[stk->top*3] = i;
    stk->arr[stk->top*3+1] = j;
    stk->arr[stk->top*3+2] = z;
}

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
