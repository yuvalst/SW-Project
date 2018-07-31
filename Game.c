/*
 * Game.c
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "List.h"

#define ERROR_INV_CMD "ERROR: invalid command\n"
#define ERROR_FILE "Error: File doesn't exist or cannot be opened\n"
#define ERROR_VALUES "Error: board contains erroneous values\n"
#define ERROR_INVALID "Error: board validation failed\n"
#define ERROR_VALUE_RANGE "Error: value not in range 0-%d\n"
#define ERROR_FIXED "Error: cell is fixed\n"
#define ERROR_SOL "Puzzle solution erroneous\n"
#define PUZ_SOLVED "Puzzle solved successfully\n"
#define ERROR_MARK_ERRORS "Error: the value should be 0 or 1\n"

typedef struct GameData {
	int mode; /*0 - init, 1 - solve, 2 - edit*/
	int mark;
	int errors;
	int n;
	int m;
	int bSize;
	int numEmpty; /*number of empty cells*/
	int ** board;
	node * head;
	node * curr;
} gameData;


gameData * initGame() {
	gameData * game = (gameData*)malloc(sizeof(gameData));
	/*check malloc*/
	game->mode = 0;
	game->mark = 1;
	game->errors = 0;
	game->n = 9;
	game->m = 9;
	game->bSize = game->n * game->m;
	numEmpty = game->bSize * game->bSize;
	game->board = NULL;
	game->head = NULL;
	game->curr = NULL;
	return game;
}


int checkInt(char * cmd) {
	while(*cmd != '\0') {
		if (!isdigit(*cmd)) {
			return 0;
		}
		cmd++;
	}
	return 1;
}

int checkArgs(char ** cmdArr, int max, int len) {
	int i;
	for (i=0; i < len; i++){
		if (!checkInt(cmdArr[i])){
			return 0;
		}
		if (atoi(cmdArr[i]) < 0 || atoi(cmdArr[i]) > max) {
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

int checkValid(gameData * game, int x, int y, int z) {
	int i, j, cStart, rStart;
	int** board = game->board;
	/*if (board[x - 1][y - 1] == z) { cell x, y already has value of z
		return 1;
	}*/
	for (i = 0; i < game->bSize; i++) {
		if ((board[i][y - 1] == z) || (board[x - 1][i] == z)) {
			return 0;
		}
	}
	cStart = (x-1) - ((x-1) % game->n); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->m); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->n; i++) {
		for (j = rStart; j < rStart + game->m; j++) {
			if (board[i][j] == z) {
				return 0;
			}
		}
	}
	return 1;
}

int solve(gameData * game, char * path) {
	FILE * gameF;
	int cell, i, j;
	char c;
	gameF = fopen(path, "r");
	if (gameF == NULL) {
		printf(ERROR_FILE);
		return 0;
	}
	newGame(game, 1); /*frees current game resources. changes mode to 1 (solve)*/
	fscanf(gameF, "%d", &game->n);
	fscanf(gameF, "%d", &game->m);
	game->bSize = game->n*game->m;
	for(j = 0; j < game->bSize; j++) {
		for(i = 0; i < game->bSize; i++) {
			fscanf(gameF, "%d%c", &cell, &c);
			game->board[i][j] = cell;
			if (cell != 0) {
				game->numEmpty--;
			}
			if (c == '.') {
				game->board[game->bSize + i][j] = 1;
			}
		}
	}
	fclose(gameF);
	checkErrors(game);
	return 1;
}

int edit(gameData * game, char* path,){
	if (path == NULL) {
		printf(ERROR_INV_CMD);
	}

}

int mark_errors(gameData * game, char ** cmdArr){
	int i;
	if (!checkInt(cmdArr[0])) {/*check if valid int*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	i = atoi(cmdArr[0]);
	if (i==0) {
		game->errors = 0;
	}
	else if (i==1) {
			game->errors = 1;
	}
	else{
		printf(ERROR_MARK_ERRORS); /*not a valid number*/
		return 0;
	}
	return 1;
}

int set(gameData * game, char ** cmdArr){
	int x, y, z, prev;
	if (game->mode != 1 || game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, game->bSize, 3)) {
		printf(ERROR_VALUE_RANGE, game->bSize);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	prev = game->board[x-1][y-1];
	z = atoi(cmdArr[2]);
	if (checkFixed(game, x, y)) {
		printf(ERROR_FIXED);
		return 0;
	}
	setList(game, cmdArr); /*clear all next moves and mark this "set" as current one*/
	game->board[x-1][y-1] = z;
	if (z!=0) {
		if (game->mode == 2) { /*if in edit mode make cell fixed*/
			game->board[x + game->bSize - 1][y-1] = 1;
		}
		checkSetError(game, x, y, z); /*if current set caused an error mark the cells*/
		if (prev == 0) {
			game->numEmpty--;
		}
	}
	else {
		if (prev != 0) {
			game->numEmpty++;
		}
	}
	printBoard(game);
	if (game->numEmpty == 0 && game->mode == 1) {
		if (!validate(game)) {
			printf(ERROR_SOL);
			return 0;
		}
		else {
			printf(PUZ_SOLVED);
			newGame(game, 0); /*init mode and new game*/
			return 1;
		}
	}
	return 1;
}

int validate(gameData * game) {

}

int generate(gameData * game, int x, int y, int z){

}

int undo(gameData * game, int x, int y, int z){

}

int redo(gameData * game, int x, int y, int z){

}

int save(gameData * game, char * path) {
	FILE * gameF;
	int i, j;
	if (game->mode == 0) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->mode == 2) {
		if (game->errors == 1) {
			printf(ERROR_VALUES);
			return 0;
		}
		if (validate(game) == 0) {
			printf(ERROR_INVALID);
			return 0;
		}
	}
	gameF = fopen(path, "w");
	if (gameF == NULL) {
		printf(ERROR_FILE);
		return 0;
	}
	fprintf(gameF, "%d %d\n", game->n, game->m);
	for(j = 0; j < game->bSize; j++) {
		for(i = 0; i < game->bSize; i++) {
			fprintf(gameF, "%d", game->board[i][j]);
			if (game->mode == 2 || game->board[game->bSize + i][j]) {
				fprintf(gameF, ".");
			}
		}
		if (j != game->bSize - 1) {
			fprintf(gameF, "\n");
		}
	}
	fclose(gameF);
	return 1;
}

int hint(gameData * game, int x, int y, int z){

}

int num_solutions(gameData * game){
	if (game->mode == 0) {
			printf(ERROR_INV_CMD);
			return 0;
	}
	if (game->errors!=0) {
		printf(ERROR_VALUES);
		return 0;
	}
	return exhaustiveBT(game);
}

int autofill(gameData * game, int x, int y, int z){

}

int reset(gameData * game, int x, int y, int z){

}

int exit(gameData * game, int x, int y, int z){

}

int exhaustiveBT(gameData * game){

}

