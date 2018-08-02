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
#define ERROR_VALUE_RANGE "Error: value not in range %d-%d\n"
#define ERROR_FIXED "Error: cell is fixed\n"
#define ERROR_SOL "Puzzle solution erroneous\n"
#define ERROR_UNSLOVABLE "Error: board is unsolvable\n"
#define ERROR_NOT_EMPTY "Error: board is not empty\n"
#define ERROR_GENERATE "Error: puzzle generator failed\n"
#define PUZ_SOLVED "Puzzle solved successfully\n"
#define ERROR_CONTAINS_VAL "Error: cell already contains a value\n"
#define HINT "Hint: set cell to %d\n"

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
	game->numEmpty = game->bSize * game->bSize;
	game->board = NULL;
	game->head = NULL;
	game->curr = NULL;
	return game;
}

gameData * newGame(gameData * game, int mode) {

}

int checkErrors(gameData * game) {

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

int validate(gameData * game);

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

	}
}

int mark_errors(gameData * game){

}

int set(gameData * game, char ** cmdArr){
	int x, y, z, prev;
	if (game->mode != 1 || game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, game->bSize, 3)) {
		printf(ERROR_VALUE_RANGE, 0, game->bSize);
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

int generate(gameData * game, char ** cmdArr){
	int x, y, i, j, f, k, try, res, numOfOnes, options = game->bSize;
	int * values;
	if(game->mode!=2) { /*not in edit mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, game->bSize, 2)) {
		printf(ERROR_VALUE_RANGE, 0, game->bSize);
		return 0;
	}
	if(game->numEmpty != game->bSize * game->bSize) { /*if board isn't empty*/
		printf(ERROR_NOT_EMPTY);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	values = calloc(game->bSize, sizeof(int));
	/*assert calloc*/
	for (try = 0; try < 1000; try++) { /*1000 tries*/
		res = 1;
		for (f = 0; f < x; f++) { /*find x random cells to fill with valid values*/
			options = game->bSize;
			for (k = 0; k < game->bSize; k++) {
				values[k] = 1;
			}
			do {
				i = (rand() % game->bSize);
				j = (rand() % game->bSize);
			}
			while (game->board[i][j] != 0); /*find empty cell*/
			while (options > 0) { /*while still have value options*/
				numOfOnes = (rand() % options) + 1; /*choose random value index*/
				for (k = 1; k <= game->bSize; k++) {
					if (values[k] == 1) {
						numOfOnes--;
					}
					if (numOfOnes == 0) { /*found the random chosen index of the value*/
						break;
					}
				}
				if (checkValid(game, i, j, k)) { /*random value is valid*/
					game->board[i][j] = k;
					game->board[i + game->bSize][j] = 1;
					break;
				}
				values[k] = 0; /*random value isn't an option anymore for this cell*/
				options--; /*one less option*/
			}
			if(game->board[i][j] == 0) { /*couldn't find random valid value*/
				for(i = 0; i < game->bSize; i++) {
					for(j = 0; j < game->bSize; j++) { /*clear board*/
						game->board[i][j] = 0;
						game->board[i + game->bSize][j] = 0;
					}
				}
				res = 0;
				break;
			}
		}
		if(res == 1) { /*was able to fill x cells*/
			res = ilpSolve(game);
		}
		if(res == 0) { /*board unsolvable*/
			if(try == 999) {
				printf(ERROR_GENERATE);
			}
			continue;
		}
		for (f = 0; f < (game->bSize * game->bSize) - y; f++) { /*find random cells to erase*/
			i = (rand() % game->bSize);
			j = (rand() % game->bSize);
			game->board[i][j] = 0;
			game->board[i + game->bSize][j] = 0;
		}
		game->numEmpty = (game->bSize * game->bSize) - y;
		printBoard(game);
	}
	addList()
	free(values);
	return res;
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

int hint(gameData * game, char ** cmdArr){
	int x, y, res;
	gameData * gameC = NULL;
	if(game->mode != 1) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if(!checkArgs(cmdArr, game->bSize, 2)){ /*invalid arguments*/
		printf(ERROR_VALUE_RANGE, 1, game->bSize);
		return 0;
	}
	if(game->errors) {
		printf(ERROR_VALUES);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	if(game->board[x + game->bSize - 1][y-1] == 1) {
		printf(ERROR_FIXED);
		return 0;
	}
	if(game->board[x-1][y-1] != 0){
		printf(ERROR_CONTAINS_VAL);
		return 0;
	}
	copyGame(gameC, game); /*allocate gameC and copy game into it*/
	res = ilpSol(gameC);
	if(!res) { /*board unsolvable*/
		printf(ERROR_UNSLOVABLE);
		freeGame(gameC);
		return 0;
	}
	printf(HINT, gameC->board[x-1][y-1]);
	freeGame(gameC);
	return 1;
}

int num_solutions(gameData * game, int x, int y, int z){

}

int autofill(gameData * game, int x, int y, int z){

}

int reset(gameData * game, int x, int y, int z){

}

int exit(gameData * game, int x, int y, int z){

}

