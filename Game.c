/*
 * Game.c
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "GameAux.h"
#include "List.h"




#define ERROR_INV_CMD "ERROR: invalid command\n"
#define ERROR_FILE "Error: File doesn't exist or cannot be opened\n"
#define ERROR_FILE2 "Error: File cannot be opened\n"
#define ERROR_VALUES "Error: board contains erroneous values\n"
#define ERROR_INVALID "Error: board validation failed\n"
#define ERROR_VALUE_RANGE "Error: value not in range %d-%d\n"
#define ERROR_FIXED "Error: cell is fixed\n"
#define ERROR_UNSOLVABLE "Error: board is unsolvable\n"
#define ERROR_NOT_EMPTY "Error: board is not empty\n"
#define ERROR_GENERATE "Error: puzzle generator failed\n"
#define ERROR_MARK_ERRORS "Error: the value should be 0 or 1\n"
#define ERROR_CONTAINS_VAL "Error: cell already contains a value\n"
#define HINT "Hint: set cell to %d\n"
#define AUTO_SET "Cell <%d,%d> set to %d\n"
#define NUM_OF_SOLS "Number of solutions: %d\n"
#define GOOD_B "This is a good board!\n"
#define MORE_THAN_1_SOL "The puzzle has more than 1 solution, try to edit it further\n"
#define VAL_PASSED "Validation passed: board is solvable\n"
#define VAL_FAILED "Validation failed: board is unsolvable\n"
#define SAVED "Saved to: %s\n"
#define NO_UNDO "Error: no moves to undo\n"
#define UNDO "Undo %d,%d: from "
#define NO_REDO "Error: no moves to redo\n"
#define REDO "Redo %d,%d: from "
#define RESET "Board reset\n"
#define EXIT "Exiting...\n"

int validate(gameData * game, int p); /*just declaration so functions can call validate*/

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
	if (game->head == NULL) {
		insertAtCurr(game, 2); /*dummy head node*/
	}
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
	updateErrors(game);
	return 1;
}


int edit(gameData * game, char* path) {
	FILE * gameF;
	char c = 0;
	int i, j;
	if (path == NULL) { /*create empty 9x9 board*/
		game->m = 3;
		game->n = 3;
		game->bSize = 9;
		newGame(game, 2);
		if (game->head == NULL) {
			insertAtCurr(game, 2); /*dummy head node*/
		}
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
		if (game->head == NULL) {
			insertAtCurr(game, 2); /*dummy head node*/
		}
		for(j = 0; j < game->bSize; j++) {
			for(i = 0; i < game->bSize; i++) {
				fscanf(gameF, "%d%c", &game->board[i][j], &c);
				if (game->board[i][j] != 0) {
					game->numEmpty--;
				}
			}
		}
		updateErrors(game);
		fclose(gameF);
	}
	return 1;
}


int markErrors(gameData * game, char ** cmdArr) {
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
		if (j % game->m == 0) {
			printRowSep(game);
		}
		for (i = 0; i < game->bSize; i++) {
			if (i % game->n == 0) {
				printf("|");
			}
			if (game->board[i][j] == 0) {
				printf("    ");
				continue;
			}
			printf(" %2d", game->board[i][j]);
			if (game->mode == 1 && game->board[i + game->bSize][j] == 1) {
				printf(".");
			}
			else if (game->board[i + game->bSize][j] == 2) {
				if ((game->mode == 1 && game->mark == 1) || game->mode == 2) {
					printf("*");
				}
				else if (game->mark == 0) {
					printf(" ");
				}
			}
			else {
				printf(" ");
			}
		}
		printf("|\n");
	}
	printRowSep(game);
}


int set(gameData * game, char ** cmdArr) {
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
	updateSetErrors(game, x, y, prev, z); /*if current set caused/solved an error mark the cells accordingly and update curr node in list. also update errors field*/
	updateEmpty(game, prev, z);
	printBoard(game);
	checkIfSolved(game);
	return 1;
}


int validate(gameData * game, int p) { /*p tells us to print*/
	gameData * gameC = NULL;
	if(game->mode==0) { /*not in edit or solve mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->errors != 0) {
		if (p) {
			printf(ERROR_VALUES);
		}
		return 0;
	}
	copyGame(&gameC, game);
	if (ilpSolver(gameC)) {
		if (p) {
			printf(VAL_PASSED);
		}
		freeGameC(gameC);
		return 1;
	}
	else{
		if (p) {
			printf(VAL_FAILED);
		}
		freeGameC(gameC);
		return 0;
	}
}

int generate(gameData * game, char ** cmdArr) {
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
	values = (int *)calloc(game->bSize, sizeof(int));
	checkAlloc(values);
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
					if (values[k - 1] == 1) {
						numOfOnes--;
					}
					if (numOfOnes == 0) { /*found the random chosen index of the value*/
						break;
					}
				}
				if (checkValid(game, i + 1, j + 1, k)) { /*random value is valid*/
					game->board[i][j] = k;
					break;
				}
				values[k - 1] = 0; /*random value isn't an option anymore for this cell*/
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
			res = ilpSolver(game);
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

int undo(gameData * game, int p) { /*p tells us if prints are needed*/
	int x, y, prevZ, z, i; /*z is current value, prevZ is the one we are changing to*/
	if (game->mode != 1 && game->mode != 2) {
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
		updateEmpty(game, game->board[x-1][y-1], prevZ);
		game->board[x - 1][y - 1] = prevZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) {
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		prevZ = game->curr->errorChanges[(4 * i) + 2];
		z = game->curr->errorChanges[(4 * i) + 3];
		game->board[x + game->bSize - 1][y - 1] = prevZ;
		if (z == 2 && prevZ == 0) {
			game->errors--;
		}
		else if (z == 0 && prevZ == 2) {
			game->errors++;
		}
	}
	if (p) {
		printBoard(game);
		for (i = 0; i < game->curr->numOfChanges; i++) {
			x = game->curr->changes[4 * i];
			y = game->curr->changes[(4 * i) + 1];
			prevZ = game->curr->changes[(4 * i) + 2];
			z = game->curr->changes[(4 * i) + 3];
			printf(UNDO, x, y);
			if (z == 0) {
				printf("_");
			}
			else {
				printf("%d", z);
			}
			printf(" to ");
			if (prevZ == 0) {
				printf("_\n");
			}
			else {
				printf("%d\n", prevZ);
			}
		}
	}
	game->curr = game->curr->prev;
	return 1;
}

int redo(gameData * game) {
	int x, y, z, nextZ, i;
	if (game->mode != 1 && game->mode != 2) {
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
		updateEmpty(game, game->board[x-1][y-1], nextZ);
		game->board[x - 1][y - 1] = nextZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) {
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		nextZ = game->curr->errorChanges[(4 * i) + 3];
		z = game->curr->errorChanges[(4 * i) + 2];
		game->board[x + game->bSize - 1][y - 1] = nextZ;
		if (z == 2 && nextZ == 0) {
			game->errors--;
		}
		else if (z == 0 && nextZ == 2) {
			game->errors++;
		}
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
		if (validate(game, 0) == 0) {
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
			if (game->board[i][j] != 0 && (game->mode == 2 || game->board[game->bSize + i][j] == 1)) { /*fixed cell, not empty cell*/
				fprintf(gameF, ".");
			}
			fprintf(gameF, " ");
		}
		if (j != game->bSize - 1) {
			fprintf(gameF, "\n");
		}
	}
	fclose(gameF);
	printf(SAVED, path);
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
	copyGame(&gameC, game); /*allocate gameC and copy game into it*/
	res = ilpSolver(gameC);
	if(!res) { /*board unsolvable*/
		printf(ERROR_UNSOLVABLE);
		freeGameC(gameC);
		return 0;
	}
	printf(HINT, gameC->board[x-1][y-1]);
	freeGameC(gameC);
	return 1;
}

int numSols(gameData * game) {
	int numOsols;
	gameData * gameC = NULL;
	if (game->mode == 0) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (game->errors!=0) {
		printf(ERROR_VALUES);
		return 0;
	}
	copyGame(&gameC, game);
	numOsols =  exhaustiveBT(gameC);
	printf(NUM_OF_SOLS, numOsols);
	if (numOsols == 1) {
		printf(GOOD_B);
	}
	else if (numOsols != 0) {
		printf(MORE_THAN_1_SOL);
	}
	freeGameC(gameC);
	return 1;
}


int autofill(gameData * game) {
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
	copyGame(&gameC, game);
	for (i = 0; i < game->bSize; i++) {
		for (j = 0; j < game->bSize; j++) {
			if (game->board[i][j] == 0) {
				val = singleValue(gameC, i, j);
				if (val != 0) {
					if (first) {
						first = 0;
						insertAtCurr(game, 3);
					}
					addToNode(game, i + 1, j + 1, val, 0);
					game->board[i][j] = val;
					printf(AUTO_SET, i + 1, j + 1, val);
					game->numEmpty--;
				}
			}
		}
	}
	printBoard(game);
	freeGameC(gameC);
	checkIfSolved(game);
	return 1;
}

int reset(gameData * game) {
	if(game->mode != 1 && game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	while (game->curr != game->head) {
		undo(game, 0); /*undo without print*/
	}
	if (game->head->next != NULL) {
		clearToEnd(&game->head->next);
	}
	printf(RESET);
	return 1;
}

void exitGame(gameData * game) {
	freeGame(game);
	if (game != NULL) {
		free(game);
	}
	printf(EXIT);
	exit(0);
}





