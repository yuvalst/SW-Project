/*
 * GameAux.c
 *
 *  Created on: 21 Aug 2018
 *      Author: guywaldman
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameAux.h"
#include "Game.h"
#include "List.h"
#include "Stack.h"
#include "gurobi.h"


#define ERROR_SOL "Puzzle solution erroneous\n"
#define PUZ_SOLVED "Puzzle solved successfully\n"
#define ALLOC_FAILED "Memory allocation failed\n"

void checkAlloc(void * p) { /*check memory allocations*/
	if (p == NULL) {
		printf(ALLOC_FAILED);
		exit(0);
	}
}


gameData * initGame() {
	gameData * game = (gameData*)malloc(sizeof(gameData));
	checkAlloc(game);
	game->mode = 0;
	game->mark = 1;
	game->errors = 0;
	game->m = 9;
	game->n = 9;
	game->bSize = game->m * game->n;
	game->numEmpty = game->bSize * game->bSize;
	game->board = NULL;
	game->head = NULL;
	game->curr = NULL;
	return game;
}

void freeBoard(gameData * game) {
	if (game->board != NULL && game->board[0] != NULL) {
		free(game->board[0]);
		game->board[0] = NULL;
	}
	if (game->board != NULL) {
		free(game->board);
		game->board = NULL;
	}
}

void freeGame(gameData* game) {
	freeBoard(game);
	freeList(game);
}

int ** initBoard(int bSize, int multi) {
	int i;
	int *p;
	int **board;
	p = (int*)calloc(multi * bSize * bSize, sizeof(int)); /*from col bSize we keep matrix of 0/1/2 which tells which cell is fixed or has error*/
	checkAlloc(p);
	board = (int**)calloc(multi * bSize, sizeof(int *));
	checkAlloc(board);
	for(i = 0; i < multi * bSize ; i++ ) {
		board[i] = p + i*bSize;
	}
	return board;
}


void freeGameC(gameData * gameC) {
	freeGame(gameC);
	if (gameC != NULL) {
		free(gameC);
		gameC = NULL;
	}
}

void copyGame(gameData ** gameC, gameData * game) {
	int i, j;
	*gameC = (gameData *)malloc(sizeof(gameData));
	checkAlloc(*gameC);
	(*gameC)->board = initBoard(game->bSize, 2);
	for (i = 0; i < game->bSize * 2; i++) {
		for (j = 0; j < game->bSize; j++) {
			(*gameC)->board[i][j] = game->board[i][j];
		}
	}
	(*gameC)->mode = game->mode;
	(*gameC)->mark = game->mark;
	(*gameC)->errors = game->errors;
	(*gameC)->m = game->m;
	(*gameC)->n = game->n;
	(*gameC)->bSize = game->bSize;
	(*gameC)->numEmpty = game->numEmpty;
	(*gameC)->head = NULL;
	(*gameC)->curr = NULL;
}



void newGame(gameData * game, int mode) {
	if (game->board != NULL) {
		freeGame(game); /*free board and list*/
	}
	if (mode != 0) {
		game->board = initBoard(game->bSize, 2);
	}
	game->mode = mode;
	game->errors = 0;
	game->numEmpty = game->bSize * game->bSize;
	game->curr = game->head;
}

void ChangeCellsWithValTo(gameData * game, int num) { /*used for exhaustiveBT. changes all cells without value 0 to fixed*/
	int i,j;
	for (i = 0; i < game->bSize; ++i) {
		for (j = 0; j < game->bSize; ++j) {
			if (game->board[i][j] != 0) {
				game->board[i + game->bSize][j] = num;
			}
		}
	}
}


void printRowSep(gameData * game) {
	int i;
	for (i = 0; i < 4 * game->bSize + game->m + 1 /*+ ((game->m + 1)%2)*/ ; i++) {
		printf("-");
	}
	printf("\n");
}


int checkValid(gameData * game, int x, int y, int z) {
	int i, j, cStart, rStart;
	int** board = game->board;
	for (i = 0; i < game->bSize; i++) {
		if ((board[i][y - 1] == z) && i != x - 1) {
			return 0;
		}
		if ((board[x - 1][i] == z) && i != y - 1) {
			return 0;
		}
	}
	cStart = (x-1) - ((x-1) % game->n); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->m); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->n; i++) {
		for (j = rStart; j < rStart + game->m; j++) {
			if ((board[i][j] == z) && (i != x - 1) && (j != y - 1)) {
				return 0;
			}
		}
	}
	return 1;
}

void handleError(gameData * game, int ** Arr, int i, int j, int Val) {
	if (Arr[Val] != NULL) {
		if (*(Arr[Val])==0) {
			*(Arr[Val]) = 2;
			game->errors++;
		}
		if (game->board[i+game->bSize][j] == 0) {
			game->board[i+game->bSize][j] = 2;
			game->errors++;
		}
	}
	else if (Val != 0) {
		Arr[Val] = &game->board[i+game->bSize][j];
	}
}

void updateErrors(gameData * game) {
	int i, j, rowVal, colVal, blockJ, blockI, **rowArr = NULL, **colArr = NULL;
	rowArr = (int**)calloc(game->bSize +1 ,sizeof(int*));
	checkAlloc(rowArr);
	colArr = (int**)calloc(game->bSize +1 ,sizeof(int*));
	checkAlloc(colArr);
	for (i = 0; i < game->bSize; ++i) {  /*row and col check*/
		for (j = 0; j < game->bSize + 1; ++j) {
			rowArr[j] = NULL;
			colArr[j] = NULL;
		}
		for (j = 0; j < game->bSize; ++j) {
			/*might need to create function inside @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
			rowVal = game->board[j][i];
			colVal = game->board[i][j];
			handleError(game, rowArr, j, i, rowVal); /*row check by value*/
			handleError(game, colArr, i, j, colVal);
			/*if(rowArr[rowVal]!=NULL){
				if (*(rowArr[rowVal])==0) {
					*(rowArr[rowVal]) = 2;
					errors++;
				}
				if (game->board[j+game->bSize][i]==0) {
					game->board[j+game->bSize][i]=2;
					errors++;
				}
			}

			else if (rowVal != 0) {
				rowArr[rowVal] = &game->board[j+game->bSize][i];
			}

			if(colArr[colVal]!=NULL){
				if (*(colArr[colVal])==0) {
					*(colArr[colVal])=2;
					errors++;
				}
				if (game->board[i+game->bSize][j]==0) {
					game->board[i+game->bSize][j]=2;
					errors++;
				}
			}
			else if (colVal != 0) {
				colArr[colVal] = &game->board[i+game->bSize][j];
			}*/
		}
	}
	for (blockI = 0; blockI < game->n; blockI++) {
		for (blockJ = 0; blockJ < game->m; blockJ++) {
			for (j = 0; j < game->bSize + 1; ++j) {
				colArr[j] = NULL;
			}
			for (i = blockI*game->n; i < (blockI+1)*game->n; i++) {
				for (j = blockJ*game->m; j < (blockJ+1)*game->m; j++) {
					colVal = game->board[i][j];
					handleError(game, colArr, i, j, colVal);
					/*if(colArr[colVal]!=NULL){
						if (*(colArr[colVal])==0) {
							*(colArr[colVal]) = 2;
							errors++;
						}
						if (game->board[i+game->bSize][j]==0) {
							game->board[i+game->bSize][j]=2;
							errors++;
						}
					}
					else if (colVal != 0) {
						colArr[colVal] = &game->board[i+game->bSize][j];
					}*/
				}
			}
		}
	}
	free(rowArr);
	free(colArr);
}

void handleCellErrors(gameData * game, int x, int y, int prev, int z, int * err) {
	if (z != 0 && game->board[x - 1][y - 1] == z) {
		if (*err == 0) {
			*err = 1;
		}
		if (game->board[x + game->bSize - 1][y - 1] == 0) { /*cell wasn't erroneous before change*/
			addToNode(game, x, y, 2, 1);
			game->board[x + game->bSize - 1][y - 1] = 2;
			game->errors++;
		}
	}
	if (prev != 0 && game->board[x - 1][y - 1] == prev && game->board[x + game->bSize - 1][y-1] == 2) {
		if (checkValid(game, x, y, prev) == 1) { /*no other cells caused the error*/
			addToNode(game, x, y, 0, 1);
			game->board[x + game->bSize - 1][y - 1] = 0;
			game->errors--;
		}
	}
}

/*use after changed the cells value in set command*/
int updateSetErrors(gameData * game, int x, int y, int prev, int z) {
	int i, j, err = 0, cStart, rStart;
	if (z == prev) {
		return 0;
	}
	for (i = 0; i < game->bSize; i++) {
		if (i != x - 1) {
			handleCellErrors(game, i + 1, y, prev, z, &err);
		}
		if (i != y - 1) {
			handleCellErrors(game, x, i + 1, prev, z, &err);
		}
	}
	cStart = (x-1) - ((x-1) % game->n); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->m); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->n; i++) {
		for (j = rStart; j < rStart + game->m; j++) {
			if (i != x - 1 && j != y - 1) {
				handleCellErrors(game, i + 1, j + 1, prev, z, &err);
			}
		}
	}
	if (err == 1 && game->board[x + game->bSize - 1][y - 1] == 0) { /*cell wasn't erroneous and is now*/
		addToNode(game, x, y, 2, 1);
		game->board[x + game->bSize - 1][y - 1] = 2;
		game->errors++;
	}
	else if (err == 0 && game->board[x + game->bSize - 1][y - 1] == 2) { /*cell was erroneous and isn't now*/
		addToNode(game, x, y, 0, 1);
		game->board[x + game->bSize - 1][y - 1] = 0;
		game->errors--;
	}
	return 1;
}

void updateEmpty(gameData * game, int prev, int z) {
	if (z != 0) {
			if (prev == 0) {
				game->numEmpty--;
			}
		}
	else {
		if (prev != 0) {
			game->numEmpty++;
		}
	}
}


int checkInt(char * cmd) {
	while(*cmd != '\0') {
		if(*cmd < 48 || *cmd > 57) {
			return 0;
		}
		cmd++;
	}
	return 1;
}

int checkArgs(char ** cmdArr, int min, int max, int len) {
	int i;
	for (i=0; i < len; i++){
		if (!checkInt(cmdArr[i])){
			return 0;
		}
		if (i <= 1 && ((atoi(cmdArr[i]) < min) || (atoi(cmdArr[i]) > max))) { /*for X/Y in set, hint and generate*/
			return 0;
		}
		if (i == 2 && ((atoi(cmdArr[i]) < min - 1) || (atoi(cmdArr[i]) > max))) { /*for Z in set*/
			return 0;
		}
	}
	return 1;
}

int checkFixed(gameData * game, int x, int y) {
	if (game->mode == 1 && game->board[x + game->bSize - 1][y - 1] == 1) { /*only in solve mode*/
		return 1;
	}
	return 0;
}

int checkIfSolved(gameData * game) {
	if (game->numEmpty == 0 && game->mode == 1) {
		if (!validate(game, 0)) {
			printf(ERROR_SOL);
			return 0;
		}
		else {
			printf(PUZ_SOLVED);
			newGame(game, 0); /*init mode and new game*/
			return 1;
		}
	}
	return 0;
}



int singleValue(gameData * gameC, int i, int j) {
	int options = gameC->bSize, r = 0, c = 0, cStart, rStart;
	int * values = (int*)calloc(gameC->bSize, sizeof(int));
	checkAlloc(values);
	for (c = 0; c < gameC->bSize; c++) {
		values[c] = 1;
	}
	for (c = 0; c < gameC->bSize; c++) {
		if (gameC->board[c][j] != 0 && values[gameC->board[c][j] - 1] == 1) {
			values[gameC->board[c][j] - 1] = 0;
			options--;
		}
		if (gameC->board[i][c] != 0 && values[gameC->board[i][c] - 1] == 1) {
			values[gameC->board[i][c] - 1] = 0;
			options--;
		}
		if (options == 0) {
			free(values);
			return 0;
		}
	}
	cStart = (i) - ((i) % gameC->m); /*starting col of inner block*/
	rStart = (j) - ((j) % gameC->n); /*starting row of inner block*/
	for (c = cStart; c < cStart + gameC->m; c++) {
		for (r = rStart; r < rStart + gameC->n; r++) {
			if (gameC->board[c][r] != 0 && values[gameC->board[c][r] - 1] == 1) {
				values[gameC->board[c][r] - 1] = 0;
				options--;
				if (options == 0) {
					free(values);
					return 0;
				}
			}
		}
	}
	if (options == 1) {
		for (c = 0; c < gameC->bSize; c++) {
			if (values[c] == 1) {
				free(values);
				return c+1;
			}
		}
	}
	free(values);
	return 0;
}

int ilpSolver(gameData * game) {
	return gurobi_solver(game);
}

/*	dir == 1 to go forward
	dir == -1 to go backward
	i is column
	j is row					*/
void btMove(gameData * game,int * i, int * j, int dir, stack * stk, int* cell) {
	if (dir == 1) {
		if (*i == game->bSize - 1) { /*end of row*/
			*i = 0;
			(*j)++;
		}
		else { /*move to next cell in row*/
			(*i)++;
		}
		if (*j >= game->bSize) { /*push dummy cell*/
			push(stk, *i, *j, 0);
		}
		else {
			push(stk,*i,*j,game->board[*i][*j]);
		}
	}
	if (dir == -1) {
		if (*i == 0) { /*start of row*/
			*i = game->bSize - 1;
			(*j)--;
		}
		else { /*move to previous cell in row*/
			(*i)--;
		}
		pop(stk,cell);
	}
}

int exhaustiveBT(gameData * gameC) {
	int i = 0, j = 0, k, counter=0 ,dir=1, valid = 0;
	int cell[3] = {0};
	stack * stk = (stack*)malloc(sizeof(stack));
	checkAlloc(stk);
	initStack(gameC,stk);
	ChangeCellsWithValTo(gameC, 1);
	push(stk, 0, 0, gameC->board[0][0]);
	while(!isEmpty(stk)){
		top(stk, cell);
		i = cell[0];
		j = cell[1];
		if (gameC->board[gameC->bSize+i][j] != 0) { /*cell is fixed*/
			btMove(gameC, &i, &j, dir, stk, cell); /*move to next cell*/
		}
		else{
			valid = 0;
			for (k = gameC->board[i][j]+1; k <= gameC->bSize; k++) { /*otherwise we check all valid values*/
				if (checkValid(gameC, i+1, j+1, k)) {
					gameC->board[i][j] = k;
					dir = 1;
					valid = 1;
					btMove(gameC, &i, &j, dir, stk, cell);
					break;
				}
			}
			if (valid == 0) { /*no more options for current cell*/
				gameC->board[i][j] = 0;
				dir = -1;
				btMove(gameC, &i, &j, dir, stk, cell);
			}
		}
		if (isFull(stk)) { /*if the board is solved, i==1 && y==gameC->bSize+1*/
			counter++;
			dir = -1;
			btMove(gameC, &i, &j, dir, stk, cell);
		}
	}
	/*finished to check all possibilities*/
	freeStack(stk);
	return counter;
}
