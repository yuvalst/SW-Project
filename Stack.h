/*
 * Stack.h Summary:
 *
 * Module representing the stack used for exhaustiveBT.
 *
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include "Game.h"

#define MAX_LEN    3
#define EMPTY     -1


/*Declaration of struct stack*/
typedef struct stack {
   int * arr; /*array acting as the stack, holds the cells*/
   int top;
   int full;
} stack;


void initStack(gameData * game, stack *stk);
void push(stack *stk, int i, int j, int z);
void pop(stack *stk, int* cell);
void top(stack *stk, int* cell) ;
int isEmpty( stack *stk);
int isFull( stack *stk);
void freeStack(stack *stk);

#endif /* STACK_H_ */
