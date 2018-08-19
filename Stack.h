/*
 * Stack.h
 *
 *  Created on: 18 ���� 2018
 *      Author: yuval
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include "Game.h"

#define MAX_LEN    3
#define EMPTY     -1


typedef struct stack {
   int * arr;
   int top;
   int full;
} stack;

int stkSize(stack * stk);

void initStack(gameData * game, stack *stk);
void push(stack *stk, int i, int j, int z);
void pop(stack *stk, int* cell);
void top(stack *stk, int* cell) ;
int isEmpty( stack *stk);
int isFull( stack *stk);
void freeStack(stack *stk);

#endif /* STACK_H_ */
