/*
 * Game.c
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
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
#define AUTO_SET "Cell <%d,%d> set to %d\n"
#define NUM_OF_SOLS "Number of solutions: %d\n"
#define GOOD_B "This is a good board!\n"
#define MORE_THAN_1_SOL "The puzzle has more than 1 solution, try to edit it further\n"
#define VAL_PASSED "Validation passed: board is solvable\n"
#define VAL_FAILED "Validation failed: board is unsolvable\n"
#define NO_UNDO "Error: no moves to undo\n"
#define UNDO "Undo %d,%d: from "
#define NO_REDO "Error: no moves to redo\n"
#define REDO "Redo %d,%d: from "
#define RESET "Board reset\n"
#define EXIT "Exiting...\n"




gameData * initGame() {
	gameData * game = (gameData*)malloc(sizeof(gameData));
	/*check malloc*/
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

void freeBoard(gameData * game){
	free(game->board[0]);
	free(game->board);
}

void freeGame(gameData* game) {
	freeBoard(game);
	freeList(game);
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


void copyGame(gameData * gameC, gameData * game) {
	int i, j;
	gameC = (gameData *)malloc(sizeof(gameData));
	gameC->board = initBoard(game->bSize, 2);
	for (i = 0; i < game->bSize * 2; i++) {
		for (j = 0; j < game->bSize; j++) {
			gameC->board[i][j] = game->board[i][j];
		}
	}
	gameC->mode = game->mode;
	gameC->mark = game->mark;
	gameC->errors = game->errors;
	gameC->m = game->m;
	gameC->n = game->n;
	gameC->bSize = game->bSize;
	gameC->numEmpty = game->numEmpty;
	gameC->head = NULL;
	gameC->curr = NULL;
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
}

void ChangeCellsWithValTo(gameData * game, int num){
	int i,j;
	for (i = 0; i < game->bSize; ++i) {
		for (j = 0; j < game->bSize; ++j) {
			if (game->board[i][j]) {
				game->board[i][game->bSize+j] = num;
			}
		}
	}
}


void printRowSep(gameData * game) {
	int i;
	for (i = 0; i < 4 * game->bSize + game->m + 1; i++) {
		printf("-");
	}
}


int checkValid(gameData * game, int x, int y, int z) {
	int i, j, cStart, rStart;
	int** board = game->board;
	for (i = 0; i < game->bSize; i++) {
		if ( ((board[i][y - 1] == z)&&(i!=x-1)) || ((board[x - 1][i] == z)&&(i!=y-1)) ) {
			return 0;
		}
	}
	cStart = (x-1) - ((x-1) % game->m); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->n); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->m; i++) {
		for (j = rStart; j < rStart + game->n; j++) {
			if (board[i][j] == z) {
				return 0;
			}
		}
	}
	return 1;
}

void handleCellErrors(gameData * game, int x, int y, int prev, int z, int * err) {
	if (game->board[x - 1][y - 1] == z) {
		if (*err == 0) {
			*err = 1;
		}
		if (game->board[x + game->bSize - 1][y - 1] == 0) { /*cell erroneous before change*/
			addToNode(game, x, y, 2, 1);
			game->board[x + game->bSize - 1][y - 1] = 2;
			game->errors++;
		}
	}
	if (game->board[x - 1][y - 1] == prev && game->board[x + game->bSize - 1][y-1] == 2) {
		if (checkValid(game, x, y, prev) == 1) { /*no other cells caused the error*/
			addToNode(game, x, y, 0, 1);
			game->board[x + game->bSize - 1][y - 1] = 0;
			game->errors--;
		}
	}
}


int checkErrors(gameData * game) {

}

/*use after changed the cells value in set command*/
int checkSetErrors(gameData * game, int x, int y, int prev, int z) {
	int i, j, err = 0, cStart, rStart;
	if (z == prev) {
		return 0;
	}
	for (i = 0; i < game->bSize; i++) {
		handleCellErrors(game, i + 1, y, prev, z, &err);
		handleCellErrors(game, x, i + 1, prev, z, &err);
	}
	cStart = (x-1) - ((x-1) % game->m); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->n); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->m; i++) {
		for (j = rStart; j < rStart + game->n; j++) {
			handleCellErrors(game, i + 1, j + 1, prev, z, &err);
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


int validate(gameData * game);



int singleValue(gameData * gameC, int i, int j) {
	int options = gameC->bSize, r = 0, c, cStart, rStart;
	int * values = calloc(gameC->bSize, sizeof(int));
	/*assert calloc*/
	for (c = 0; c < gameC->bSize; c++) {
		if (gameC->board[c][j] != 0 && values[gameC->board[r][j]] == 1) {
			values[gameC->board[c][j]] = 0;
			options--;
		}
		if (gameC->board[i][r] != 0 && values[gameC->board[i][r]] == 1) {
			values[gameC->board[i][r]] = 0;
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
			if (gameC->board[c][r] != 0 && values[gameC->board[c][r]] == 1) {
				values[gameC->board[c][r]] = 0;
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


int solve(gameData * game, char * path) {
	FILE * gameF;
	int cell, i, j;
	char c;
	gameF = fopen(path, "r");
	if (gameF == NULL) {
		printf(ERROR_FILE);
		return 0;
	}
	fscanf(gameF, "%d", &(game->m));
	fscanf(gameF, "%d", &(game->n));
	game->bSize = game->m * game->n;
	newGame(game, 1); /*frees current game resources, builds new board according to bSize, changes mode to 1 (solve)*/
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
		game->m = 3;
		game->n = 3;
		game->bSize = 9;
		newGame(game, 2);
	}
	else {
		gameF = fopen(path, "r");
		if (gameF == NULL) {
			printf(ERROR_FILE2);
			return 0;
		}
		fscanf(gameF, "%d", &game->m);
		fscanf(gameF, "%d", &game->n);
		game->bSize = game->m * game->n;
		newGame(game, 2);
		for(j = 0; j < game->bSize; j++) {
			for(i = 0; i < game->bSize; i++) {
				fscanf(gameF, "%d", &game->board[i][j]);
				if (game->board[i][j] != 0) {
					game->numEmpty--;
				}
			}
		}
		fclose(gameF);
	}
	return 1;
}

int markErrors(gameData * game, char ** cmdArr){
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
		game->mark = 0;
	}
	else if (i==1) {
		game->mark = 1;
	}
	else{
		printf(ERROR_MARK_ERRORS); /*not a valid number*/
		return 0;
	}
	return 1;
}

void printBoard(gameData * game) {
	int i, j;
	for (j = 0; j < game->bSize; j++) {
		if (j % game->n == 0) {
			printRowSep(game);
		}
		for (i = 0; i < game->bSize; i++) {
			if (i % game->m == 0) {
				printf("|");
			}
			if (game->board[i][j] == 0) {
				printf("    ");
				continue;
			}
			printf(" ");
			if (game->mode == 1 && game->board[i + game->bSize][j] == 1) {
				printf("%d.", game->board[i][j]);
			}
			else if (game->board[i + game->bSize][j] == 2) {
				if ((game->mode == 1 && game->mark == 1) || game->mode == 2) {
					printf("%d*", game->board[i][j]);
				}
				else {
					printf("%2d", game->board[i][j]);
				}
			}
			printf(" ");
		}
		printf("|\n");
	}
	printRowSep(game);
}


int set(gameData * game, char ** cmdArr){
	int x, y, z, prev;
	if (game->mode != 1 && game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, 1, game->bSize, 3)) {
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
	insertAtCurr(game, 1); /*clear all next moves and mark this "set" as current one*/
	addToNode(game, x, y, z, 0);
	game->board[x-1][y-1] = z;
	checkSetErrors(game, x, y, prev, z); /*if current set caused/solved an error mark the cells accordingly and update curr node in list. also update errors field*/
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
		printf(ERROR_VALUES);

		return 0;
	}
	else if (ilpSolver(game)) {
		printf(VAL_PASSED);
		return 1;
	}
	else{
		printf(VAL_FAILED);
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
	if (!checkArgs(cmdArr, 0, game->bSize * game->bSize, 2)) {
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
		insertAtCurr(game, 2);
		for (f = 0; f < y;) { /*find random cells to keep*/
			i = (rand() % game->bSize);
			j = (rand() % game->bSize);
			if (game->board[i + game->bSize][j] != 3) {
				game->board[i + game->bSize][j] = 3;
				addToNode(game, i + 1, j + 1, game->board[i][j], 0);
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
	free(values);
	return res;
}

int undo(gameData * game, int p){ /*p tells us if prints are needed*/
	int x, y, prevZ, z, i; /*z is current value, prevZ is the one we are changing to*/
	if (game->mode != 1 || game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (game->head == NULL || game->curr->prev == NULL) { /*cmd list is empty or we are the at head*/
		printf(NO_UNDO);
		return 0;
	}
	for (i = 0; i < game->curr->numOfChanges; i++) {
		x = game->curr->changes[4 * i];
		y = game->curr->changes[(4 * i) + 1];
		prevZ = game->curr->changes[(4 * i) + 2];
		game->board[x - 1][y - 1] = prevZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) {
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		prevZ = game->curr->errorChanges[(4 * i) + 2];
		game->board[x + game->bSize - 1][y - 1] = prevZ;
	}
	if (p) {
		printBoard(game);
		for (i = 0; i < game->curr->numOfChanges; i++) {
			x = game->curr->changes[4 * i];
			y = game->curr->changes[(4 * i) + 1];
			prevZ = game->curr->changes[(4 * i) + 2];
			z = game->curr->changes[(4 * i) + 3];
			printf(UNDO, x, y);
			if (prevZ == 0) {
				printf("_");
			}
			else {
				printf("%d", prevZ);
			}
			printf(" to ");
			if (z == 0) {
				printf("_\n");
			}
			else {
				printf("%d\n", z);
			}
		}
	}
	game->curr = game->curr->prev;
	return 1;
}

int redo(gameData * game){
	int x, y, z, nextZ, i;
	if (game->mode != 1 || game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (game->head == NULL || game->curr->next == NULL) {
		printf(NO_REDO);
		return 0;
	}
	game->curr = game->curr->next;
	for (i = 0; i < game->curr->numOfChanges; i++) {
		x = game->curr->changes[4 * i];
		y = game->curr->changes[(4 * i) + 1];
		nextZ = game->curr->changes[(4 * i) + 3];
		game->board[x - 1][y - 1] = nextZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) {
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		nextZ = game->curr->errorChanges[(4 * i) + 3];
		game->board[x + game->bSize - 1][y - 1] = nextZ;
	}
	printBoard(game);
	for (i = 0; i < game->curr->numOfChanges; i++) {
		x = game->curr->changes[4 * i];
		y = game->curr->changes[(4 * i) + 1];
		z = game->curr->changes[(4 * i) + 2];
		nextZ = game->curr->changes[(4 * i) + 3];
		printf(REDO, x, y);
		if (z == 0) {
			printf("_");
		}
		else {
			printf("%d", z);
		}
		printf(" to ");
		if (nextZ == 0) {
			printf("_\n");
		}
		else {
			printf("%d\n", nextZ);
		}
	}
	return 1;
}

int save(gameData * game, char * path) {
	FILE * gameF;
	int i, j;
	if (game->mode == 0) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->mode == 2) {
		if (game->errors != 0) {
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
	fprintf(gameF, "%d %d\n", game->m, game->n);
	for(j = 0; j < game->bSize; j++) {
		for(i = 0; i < game->bSize; i++) {
			fprintf(gameF, "%d", game->board[i][j]);
			if (game->mode == 2 || game->board[game->bSize + i][j] == 1) {
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
	if(!checkArgs(cmdArr, 1, game->bSize, 2)){ /*invalid arguments*/
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

int numSols(gameData * game){
	int numOsols;
	if (game->mode == 0) {
			printf(ERROR_INV_CMD);
			return 0;
	}
	if (game->errors!=0) {
		printf(ERROR_VALUES);
		return 0;
	}
	numOsols =  exhaustiveBT(game);
	printf(NUM_OF_SOLS,numOsols);
	if (numOsols == 1) {
		printf(GOOD_B);
	}
	else{
		printf(MORE_THAN_1_SOL);
	}
	return 1;
}


int autofill(gameData * game){
	int i, j, first = 1, val = 0;
	gameData * gameC = NULL;
	if(game->mode != 1) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if(game->errors != 0) {
		printf(ERROR_VALUES);
		return 0;
	}
	copyGame(gameC, game);
	for (i = 0; i < game->bSize; i++) {
		for (j = 0; j < game->bSize; j++) {
			if (game->board[i][j] == 0) {
				val = singleValue(gameC, i, j);
				if (val != 0) {
					if(first) {
						first = 0;
						insertAtCurr(game, 3);
					}
					addToNode(game, i + 1, j + 1, val, 0);
					game->board[i][j] = val;
					printf(AUTO_SET, i, j, val);
					game->numEmpty--;
				}
			}
		}
	}
	printBoard(game);
	freeGame(gameC);
	return 1;
}

int reset(gameData * game){
	if(game->mode != 1 || game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	while (game->curr != game->head) {
		undo(game, 0); /*undo without print*/
	}
	freeList(game);
	printf(RESET);
	return 1;
}

void exitGame(gameData * game){
	freeGame(game);
	printf(EXIT);
	exit(0);
}

int exhaustiveBT(gameData * game){
	int x = 1, y = 1, i, counter=0 ,dir=1;
	gameData * gameC = NULL;
	copyGame(gameC, game);
	ChangeCellsWithValTo(gameC,1);

	while(y!=0){

		if (gameC->board[gameC->bSize+x][y] != 0) { /*cell is fixed*/
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
		if (x==1 && y==gameC->bSize+1) { /*if the board is solved*/
			counter++;
			dir = -1;
			btMove(gameC, &x, &y, dir);
		}
	}
	/*finished to check all possibilities*/
	return counter;
}

/*	dir == 1 to go forward
	dir == -1 to go backward
	x is column
	y is row					*/
void btMove(gameData * game,int * x, int * y, int dir) {
	if (dir == 1) {
		if (*x == game->bSize) { /*end of row*/
				*x=1;
				(*y)++;
		}
		else { /*move to next cell in row*/
			(*x)++;
		}
	}
	if (dir == -1) {
		if (*x == 1) { /*start of row*/
			*x = game->bSize;
			(*y)--;
		}
		else { /*move to previous cell in row*/
			(*x)--;
		}
	}
}


