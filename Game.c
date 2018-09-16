/*
 * Game.c
 *
 * Functions representing the different user commands. Each one takes care of a command's situations.
 *
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


int solve(gameData * game, char * path) {
	FILE * gameF;
	int cell, i, j, check;
	char c;
	gameF = fopen(path, "r");
	if (gameF == NULL) { /*error while opening file*/
		printf(ERROR_FILE);
		return 0;
	}
	check = fscanf(gameF, "%d", &(game->m));
	if (!checkScan(check, 1)) {
		return 0;
	}
	check = fscanf(gameF, "%d", &(game->n));
	if (!checkScan(check, 1)) {
		return 0;
	}
	game->bSize = game->m * game->n;
	newGame(game, 1); /*frees current game resources, builds new board according to bSize, changes mode to 1 (solve)*/
	if (game->head == NULL) {
		insertAtCurr(game, 2); /*dummy head node*/
	}
	for(j = 0; j < game->bSize; j++) { /*get cell values*/
		for(i = 0; i < game->bSize; i++) {
			check = fscanf(gameF, "%d%c", &cell, &c);
			if (j != game->bSize - 1 || i != game->bSize - 1) { /*not last cell*/
				if (!checkScan(check, 2)) {
					return 0;
				}
			}
			else {
				if (!checkScan(check, 1)) {
					return 0;
				}
			}
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
	printBoard(game);
	return 1;
}


int edit(gameData * game, char* path) {
	FILE * gameF;
	char c = 0;
	int i, j, check;
	if (path == NULL) { /*no path given, create empty 9x9 game*/
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
		check = fscanf(gameF, "%d", &game->m);
		if (!checkScan(check, 1)) {
			return 0;
		}
		check = fscanf(gameF, "%d", &game->n);
		if (!checkScan(check, 1)) {
			return 0;
		}
		game->bSize = game->m * game->n;
		newGame(game, 2);
		if (game->head == NULL) {
			insertAtCurr(game, 2); /*dummy head node*/
		}
		for(j = 0; j < game->bSize; j++) { /*read cell values*/
			for(i = 0; i < game->bSize; i++) {
				check = fscanf(gameF, "%d%c", &game->board[i][j], &c);
				if (j != game->bSize - 1 || i != game->bSize - 1) {
					if (!checkScan(check, 2)) {
						return 0;
					}
				}
				else {
					if (!checkScan(check, 1)) {
						return 0;
					}
				}
				if (game->board[i][j] != 0) {
					game->numEmpty--;
				}
			}
		}
		updateErrors(game);
		fclose(gameF);
	}
	printBoard(game);
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
	if (game->mode != 1 && game->mode !=2) {
		printf(ERROR_INV_CMD);
		return;
	}
	for (j = 0; j < game->bSize; j++) {
		if (j % game->m == 0) { /*every m rows*/
			printRowSep(game);
		}
		for (i = 0; i < game->bSize; i++) {
			if (i % game->n == 0) { /*every n cols*/
				printf("|");
			}
			if (game->board[i][j] == 0) { /*empty cell*/
				printf("    ");
				continue; /*move to next cell*/
			}
			printf(" %2d", game->board[i][j]);
			if (game->mode == 1 && game->board[i + game->bSize][j] == 1) { /*fixed cell in solve mode*/
				printf(".");
			}
			else if (game->board[i + game->bSize][j] == 2) { /*erroneous cell*/
				if ((game->mode == 1 && game->mark == 1) || game->mode == 2) { /*relevant modes and attributes to print errors*/
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
	if (!checkArgs(cmdArr, 1, game->bSize, 3)) { /*check 3 arguments in range 1 to bSize*/
		printf(ERROR_VALUE_RANGE, 0, game->bSize);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	prev = game->board[x-1][y-1]; /*save previous cell value for command list*/
	z = atoi(cmdArr[2]);
	if (game->mode == 1 && checkFixed(game, x, y)) {
		printf(ERROR_FIXED);
		return 0;
	}
	insertAtCurr(game, 1); /*clear all next moves from command list and make this "set" the current one*/
	addToNode(game, x, y, z, 0); /*add changes to command node*/
	game->board[x-1][y-1] = z;
	updateSetErrors(game, x, y, prev, z); /*if current set caused/solved an error mark the cells accordingly and update curr node in list. also update errors field*/
	updateEmpty(game, prev, z);
	printBoard(game);
	checkIfSolved(game);
	return 1;
}


int validate(gameData * game, int p) { /*p tells us if prints are needed*/
	gameData * gameC = NULL;
	if(game->mode != 1 && game->mode != 2) { /*not in edit or solve mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->errors != 0) { /*game has errors*/
		if (p) {
			printf(ERROR_VALUES);
		}
		return 0;
	}
	copyGame(&gameC, game); /*copy the game in order to solve it without changing it*/
	if (ilpSolver(gameC)) { /*solve game to check valid solution*/
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
	int x, y, i, j, f, try, res;
	if(game->mode != 2) { /*not in edit mode*/
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (!checkArgs(cmdArr, 0, game->bSize * game->bSize, 2)) { /*check that 2 arguments are in range 0 to bSize^2*/
		printf(ERROR_VALUE_RANGE, 0, game->bSize * game->bSize);
		return 0;
	}
	if(game->numEmpty != game->bSize * game->bSize) { /*if board isn't empty*/
		printf(ERROR_NOT_EMPTY);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	for (try = 0; try < 1000; try++) { /*1000 tries*/
		fillXcells(game, x, &res);
		if(res == 1) { /*was able to fill x cells*/
			res = ilpSolver(game); /*try to solve board*/
		}
		if(res == 0) { /*board unsolvable*/
			if(try == 999) {
				printf(ERROR_GENERATE);
			}
			continue; /*exits the loop after 999 tries*/
		}
		insertAtCurr(game, 2); /*new command node in list*/
		for (f = 0; f < y;) { /*find random y cells to keep*/
			i = (rand() % game->bSize);
			j = (rand() % game->bSize);
			if (game->board[i + game->bSize][j] != 3) { /*3 will mark if we keep that cell*/
				game->board[i + game->bSize][j] = 3;
				addToNode(game, i + 1, j + 1, game->board[i][j], 0); /*add change to command node*/
				f++;
			}
		}
		for(i = 0; i < game->bSize; i++) {
			for(j = 0; j < game->bSize; j++) {
				if (game->board[i + game->bSize][j] != 3) { /*cell wasn't picked so we erase its value*/
					game->board[i][j] = 0;
				}
				else {
					game->board[i + game->bSize][j] = 0; /*cell was picked so we leave the value and reset its status*/
				}
			}
		}
		game->numEmpty = (game->bSize * game->bSize) - y; /*update number of empty cells*/
		printBoard(game);
		break;
	}
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
	for (i = 0; i < game->curr->numOfChanges; i++) { /*make value changes*/
		x = game->curr->changes[4 * i];
		y = game->curr->changes[(4 * i) + 1];
		prevZ = game->curr->changes[(4 * i) + 2];
		updateEmpty(game, game->board[x-1][y-1], prevZ);
		game->board[x - 1][y - 1] = prevZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) { /*make error changes*/
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		prevZ = game->curr->errorChanges[(4 * i) + 2];
		z = game->curr->errorChanges[(4 * i) + 3];
		game->board[x + game->bSize - 1][y - 1] = prevZ;
		if (z == 2 && prevZ == 0) { /*cell was erroneous before the change but isn't after*/
			game->errors--;
		}
		else if (z == 0 && prevZ == 2) { /*opposite*/
			game->errors++;
		}
	}
	if (p) { /*prints*/
		printBoard(game);
		for (i = 0; i < game->curr->numOfChanges; i++) { /*print value changes*/
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
	game->curr = game->curr->prev; /*move curr pointer backwards*/
	return 1;
}


int redo(gameData * game) {
	int x, y, z, nextZ, i; /*z is current value, nextZ is the one we are changing to*/
	if (game->mode != 1 && game->mode != 2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (game->head == NULL || game->curr->next == NULL) {
		printf(NO_REDO);
		return 0;
	}
	game->curr = game->curr->next; /*move curr pointer to next node*/
	for (i = 0; i < game->curr->numOfChanges; i++) { /*make value changes*/
		x = game->curr->changes[4 * i];
		y = game->curr->changes[(4 * i) + 1];
		nextZ = game->curr->changes[(4 * i) + 3];
		updateEmpty(game, game->board[x-1][y-1], nextZ);
		game->board[x - 1][y - 1] = nextZ;
	}
	for (i = 0; i < game->curr->numOfErrors; i++) { /*make error changes*/
		x = game->curr->errorChanges[4 * i];
		y = game->curr->errorChanges[(4 * i) + 1];
		nextZ = game->curr->errorChanges[(4 * i) + 3];
		z = game->curr->errorChanges[(4 * i) + 2];
		game->board[x + game->bSize - 1][y - 1] = nextZ;
		if (z == 2 && nextZ == 0) { /*cell was erroneous before the change but isn't after*/
			game->errors--;
		}
		else if (z == 0 && nextZ == 2) { /*opposite*/
			game->errors++;
		}
	}
	printBoard(game);
	for (i = 0; i < game->curr->numOfChanges; i++) { /*print value changes*/
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
	int i, j, check;
	if (game->mode == 0) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	else if (game->mode == 2) {
		if (game->errors != 0) {
			printf(ERROR_VALUES);
			return 0;
		}
		if (validate(game, 0) == 0) { /*check if valid board without printing*/
			printf(ERROR_INVALID);
			return 0;
		}
	}
	gameF = fopen(path, "w");
	if (gameF == NULL) {
		printf(ERROR_FILE);
		return 0;
	}
	check = fprintf(gameF, "%d %d\n", game->m, game->n);
	if (!checkPrint(check)) {
		return 0;
	}
	for(j = 0; j < game->bSize; j++) { /*print the cells to the file*/
		for(i = 0; i < game->bSize; i++) {
			check = fprintf(gameF, "%d", game->board[i][j]);
			if (!checkPrint(check)) {
				return 0;
			}
			if (game->board[i][j] != 0 && (game->mode == 2 || game->board[game->bSize + i][j] == 1)) { /*fixed cell, not empty cell*/
				check = fprintf(gameF, ".");
				if (!checkPrint(check)) {
					return 0;
				}
			}
			check = fprintf(gameF, " ");
			if (!checkPrint(check)) {
				return 0;
			}
		}
		if (j != game->bSize - 1) {
			check = fprintf(gameF, "\n");
			if (!checkPrint(check)) {
				return 0;
			}
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
	if(!checkArgs(cmdArr, 1, game->bSize, 2)){ /*check 2 arguments, need to be in range 1 to bSize*/
		printf(ERROR_VALUE_RANGE, 1, game->bSize);
		return 0;
	}
	if(game->errors) { /*if game board has erroneous cells*/
		printf(ERROR_VALUES);
		return 0;
	}
	x = atoi(cmdArr[0]);
	y = atoi(cmdArr[1]);
	if(game->board[x + game->bSize - 1][y-1] == 1) { /*if cell is fixed*/
		printf(ERROR_FIXED);
		return 0;
	}
	if(game->board[x-1][y-1] != 0) { /*if cell isn't empty*/
		printf(ERROR_CONTAINS_VAL);
		return 0;
	}
	copyGame(&gameC, game); /*allocate gameC and copy game into it*/
	res = ilpSolver(gameC); /*get board solution on the copy*/
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
	if (game->mode != 1 && game->mode !=2) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if (game->errors!=0) {
		printf(ERROR_VALUES);
		return 0;
	}
	copyGame(&gameC, game);
	numOsols =  exhaustiveBT(gameC); /*get number of solutions*/
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
	int i, j, first = 1, val = 0, changes = 0, * p;
	gameData * gameC = NULL;
	if(game->mode != 1) {
		printf(ERROR_INV_CMD);
		return 0;
	}
	if(game->errors != 0) {
		printf(ERROR_VALUES);
		return 0;
	}
	copyGame(&gameC, game); /*copy of game to check the values on it without including the ongoing changes*/
	for (i = 0; i < game->bSize; i++) {
		for (j = 0; j < game->bSize; j++) {
			if (game->board[i][j] == 0) {
				val = singleValue(gameC, i, j); /*find if cell has single legal value*/
				if (val != 0) {
					if (first) { /*add new node to command list*/
						first = 0;
						insertAtCurr(game, 3);
					}
					addToNode(game, i + 1, j + 1, val, 0); /*add value change to node*/
					game->board[i][j] = val;
					changes++; /*count number of changes*/
					printf(AUTO_SET, i + 1, j + 1, val);
					game->numEmpty--;
				}
			}
		}
	}
	for (i = 0; i < changes; i++) { /*go over changes and update errors like they were added by "set" command*/
		p = &(game->curr->changes[4*i]);
		updateSetErrors(game, *p, *(p+1), *(p+2), *(p+3));
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





