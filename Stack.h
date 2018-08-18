/*
 * Stack.h
 *
 *  Created on: 18 баев 2018
 *      Author: yuval
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>

#define MAX_LEN    3
#define EMPTY     -1


typedef struct stack {
   int * arr;
   int top;
   int full;
} stack;

void initStack(stack *stk);
void push(char c, stack *stk);
void pop(stack *stk);
void top( stack *stk);
int isEmpty( stack *stk);
int isFull( stack *stk);
void freeStack(stack *stk);

#endif /* STACK_H_ */
