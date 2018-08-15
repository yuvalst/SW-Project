/*
 * List.h
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct Node node;
node * createNode();
void addToNode(gameData * game, int x, int y, int z);
void insertAtCurr(gameData * game);


#endif /* LIST_H_ */
