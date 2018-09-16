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

/* Receives gameData and stack pointer and inisializes a new stack object*/
void initStack(gameData * game, stack *stk);
/* Receives stack, i and j for coordinates on the board and z for value and pushes them to the stack array */
void push(stack *stk, int i, int j, int z);
/* Receives stack and int pointer and updates cell with the values of the last cell on the stack like top, 
but also removes the last cell from the stack*/
void pop(stack *stk, int* cell);
/* Receives stack and int pointer and updates cell with the values of the last cell on the stack like top*/
void top(stack *stk, int* cell) ;
/* Receives stack and returns 1 if the stack is empty else 0*/
int isEmpty( stack *stk);
/* Receives stack and returns 1 if the stack is full else 0*/
int isFull( stack *stk);
/* Receives stack and frees allocated memory*/
void freeStack(stack *stk);

#endif /* STACK_H_ */
