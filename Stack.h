/*
 * Stack.h Summary:
 *
 * Module representing the stack used for num_solutions.
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
   int top; /*acts as the index for the top of the stack, starts with -1 when empty*/
   int full; /*indicator for when stack is full*/
} stack;

/* Receives gameData and stack pointer and initializes a new stack object*/
void initStack(gameData * game, stack *stk);

/* Receives stack, i and j for coordinates on the board and z for value and adds them to the stack array */
void push(stack *stk, int i, int j, int z);

/* Receives stack and int pointer and updates cell with the values of the last cell on the stack, 
then removes the last cell from the stack*/
void pop(stack *stk, int* cell);

/* Receives stack and int pointer and updates cell with the values of the last cell on the stack*/
void top(stack *stk, int* cell) ;

/* Receives stack and returns 1 if the stack is empty else 0*/
int isEmpty( stack *stk);

/* Receives stack and returns 1 if the stack is full else 0*/
int isFull( stack *stk);

/* Receives stack and frees its allocated memory*/
void freeStack(stack *stk);

#endif /* STACK_H_ */
