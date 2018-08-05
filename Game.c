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
#define ERROR_FILE2 "Error: File cannot be opened\n"
#define ERROR_VALUES "Error: board contains erroneous values\n"
#define ERROR_INVALID "Error: board validation failed\n"
#define ERROR_VALUE_RANGE "Error: value not in range %d-%d\n"
#define ERROR_FIXED "Error: cell is fixed\n"
#define ERROR_SOL "Puzzle solution erroneous\n"
#define ERROR_UNSLOVABLE "Error: board is unsolvable\n"
#define ERROR_NOT_EMPTY "Error: board is not empty\n"
#define ERROR_GENERATE "Error: puzzle generator failed\n"
#define PUZ_SOLVED "Puzzle solved successfully\n"
#define ERROR_MARK_ERRORS "Error: the value should be 0 or 1\n"
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
	if (game->board) {
		freeBoard(game->board);
	}
}

int ** initBoard(int bSize, int multi) { /*imported from hw3*/
	int i;
	int *p;
	int **board;
	p = calloc(multi * bSize * bSize, sizeof(int)); /*from col bSize we keep matrix of 0/1 which tells which cell is fixed*/
	board = calloc(multi * bSize, sizeof(int *));
	if (p == NULL || board == NULL) {
		printf("Error: calloc has failed\n");
		exit(0);
	}
	for(i = 0; i < multi * bSize ; i++ ) {
		board[i] = p + i*bSize;
	}
	return board;
}

void freeBoard(int** board){
	free(board[0]);
	free(board);
}

int ChangeCellsWithValTo(gameData * game, int num){
	int i,j;
	for (i = 0; i < game->bSize; ++i) {
		for (j = 0; j < game->bSize; ++j) {
			if (game->board[i][j]) {
				game->board[i][game->bSize+j]=num;
			}
		}
	}
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

int edit(gameData * game, char* path){
	/*need to clear old data and board*/
	FILE * gameF;
	int i, j;
	if (path == NULL) {
		newGame(game, 2);
		game->board = initBoard(game->bSize , 2);
	}
	else {
		gameF = fopen(path, "r");
		if (gameF == NULL) {
			printf(ERROR_FILE2);
			return 0;
		}
		newGame(game, 2);
		fscanf(gameF, "%d", &game->n);
		fscanf(gameF, "%d", &game->m);
		game->bSize = game->n * game->m;
		game->board = initBoard(game->bSize , 2);
		for(j = 0; j < game->bSize; j++) {
			for(i = 0; i < game->bSize; i++) {
				fscanf(gameF, "%d", &game->board[i][j]);
			}
		}
		fclose(gameF);
	}

}

int mark_errors(gameData * game, char ** cmdArr){
	int i;
	if (game->mode != 1) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkInt(cmdArr[0])) {/*check if valid int*/
		printf(ERROR_MARK_ERRORS);
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
	if (game->mode != 1 && game->mode != 2) {
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
	if (game->mode == 1 && checkFixed(game, x, y)) {
		printf(ERROR_FIXED);
		return 0;
	}
	setList(game, cmdArr); /*clear all next moves and mark this "set" as current one*/
	game->board[x-1][y-1] = z;
	if (z!=0) {
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
	if(game->mode==0) { /*not in edit or solve mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->errors!=0) {
		printf("Validation passed: board is solvable\n");
		return 0;
	}
	else if (ilpSolver(game)) {
		printf("Validation passed: board is solvable\n");
		return 1;
	}
	else{
		printf("Validation failed: board is unsolvable\n");
		return 0;
	}
}

int generate(gameData * game, char ** cmdArr){
	int x, y, i, j, f, k, try, res, numOfOnes, options = game->bSize;
	int * values;
	if(game->mode!=2) { /*not in edit mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, game->bSize * game->bSize, 2)) {
		printf(ERROR_VALUE_RANGE, 0, game->bSize * game->bSize);
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
					break;
				}
				values[k] = 0; /*random value isn't an option anymore for this cell*/
				options--; /*one less option*/
			}
			if(game->board[i][j] == 0) { /*couldn't find random valid value*/
				for(i = 0; i < game->bSize; i++) {
					for(j = 0; j < game->bSize; j++) { /*clear board*/
						game->board[i][j] = 0;
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
		for (f = 0; f < y;) { /*find random cells to keep*/
			i = (rand() % game->bSize);
			j = (rand() % game->bSize);
			if (game->board[i + game->bSize][j] != 3) {
				game->board[i + game->bSize][j] = 3;
				f++;
			}
		}
		for(i = 0; i < game->bSize; i++) {
			for(j = 0; j < game->bSize; j++) {
				if (game->board[i + game->bSize][j] != 3) {
					game->board[i][j] = 0;
				}
				else {
					game->board[i + game->bSize][j] = 0;
				}
			}
		}
		game->numEmpty = (game->bSize * game->bSize) - y;
		printBoard(game);
		break;
	}
	if (res != 0) {
		addList();
	}
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

int num_solutions(gameData * game){
	int numOsols;
	if (game->mode == 0) {
			printf(ERROR_INV_CMD);
			return 0;
	}
	if (game->errors!=0) {
		printf(ERROR_VALUES);
		return 0;
	}
	printf("%d",exhaustiveBT(game));
	numOsols =  exhaustiveBT(game);
	printf("Number of solutions: %d\n",numOsols);
	if (numOsols == 1) {
		printf("This is a good board!\n");
	}
	else{
		printf("The puzzle has more than 1 solution, try to edit it further\n");
	}

}

int autofill(gameData * game, int x, int y, int z){

}

int reset(gameData * game, int x, int y, int z){

}

int exit(gameData * game, int x, int y, int z){

}

int exhaustiveBT(gameData * game){
	int x, y, i, counter=0 ,dir=1;
	gameData * gameC = NULL;
	copyGame(gameC, game);
	ChangeCellsWithValTo(gameC,1);

	for (x = 1; x < gameC->bSize;) { /*x is column*/
		for (y = 1; y < gameC->bSize;) { /*y is row*/
			if (y==0) { /*finished to check all possibilities*/
				return counter;
			}
			if (gameC->board[y][gameC->bSize+x] != 0) { /*cell is fixed*/
				btMove(gameC, &x, &y, dir); /*move to next cell*/
				continue;
			}
			else{
				for (i = gameC->board[x][y]+1; i <= gameC->bSize; i++) { /*otherwise we check all valid values*/
					if (checkValid(gameC, x, y, i)) { /*need to check if x and y are correct order*/
						gameC->board[x][y] = i;
						dir = 1;
						btMove(gameC, &x, &y, dir);
						break;
					}
				}
				if (gameC->board[x][y] > gameC->bSize) { /*no more options for current cell*/
					gameC->board[x][y] = 0;
					dir = -1;
					btMove(gameC, &x, &y, dir);
					continue;
				}
			}
			if (x==gameC-1 && y) { /*if the board is solved*/
				counter++;
				dir = -1;
				btMove(gameC, &x, &y, dir);
			}
		}
	}
}

/*	dir == 1 to go forward
	dir == -1 to go backward
	x is column
	y is row					*/
void btMove(gameData * game,int * x, int * y, int dir) {
	if (dir == 1) {
		if (x == game->bSize) { /*end of row*/
				*x=1;
				(*y)++;
		}
		else { /*move to next cell in row*/
			(*x)++;
		}
	}
	if (dir == -1) {
		if (x == 1) { /*start of row*/
			*x = game->bSize;
			(*y)--;
		}
		else { /*move to previous cell in row*/
			(*x)--;
		}
	}
}


