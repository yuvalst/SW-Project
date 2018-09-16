/*
 * GameAux.c
 *
 *  Functions assisting the functions of the Game module.
 *
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
#define SCAN_FAILED "fscanf failed\n"
#define PRINT_FAILED "fprintf failed\n"


void checkAlloc(void * p) { /*check memory allocations*/
	if (p == NULL) {
		printf(ALLOC_FAILED);
		exit(0);
	}
}

int checkScan(int check, int expect) {
	if (check < expect) {
		printf(SCAN_FAILED);
		return 0;
	}
	return 1;
}

int checkPrint(int check) {
	if (check < 0) {
		printf(PRINT_FAILED);
		return 0;
	}
	return 1;
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

/*
 * Receives two ints and allocates a game board accordingly. The board is "multi" times wide in order to keep more data for the puzzle cells.
 * For example, we create a board 2 times wide and for cell <x,y>, its status is kept at cell <x + game->bSize,y> as an int (0-valid, 1-fixed, 2-erroneous).
 *
 * @param bSize - length of puzzle border
 * @param multi - number of times we want the size of the board to be multiplied for saving more data about the cells
 *
 * @return
 *  board - pointer to the two dimensional array of ints created
 */
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
	for (i = 0; i < game->bSize * 2; i++) { /*copy game board*/
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

/*Change the game structure to represent a new puzzle in the given mode. Must be called after game->bSize was updated to the wanted value.
 *
 * @param game - Game structure
 * @param mode - Wanted game mode: 0 - init, 1 - solve, 2 - edit
 *
 * */
void newGame(gameData * game, int mode) {
	if (game->board != NULL) {
		freeGame(game); /*free board and list*/
	}
	if (mode != 0) { /*if not in init mode*/
		game->board = initBoard(game->bSize, 2);
	}
	game->mode = mode;
	game->errors = 0;
	game->numEmpty = game->bSize * game->bSize;
	game->curr = game->head;
}

/*Used by exhaustiveBT. Changes all cells without value 0 to be fixed*/
void ChangeCellsWithValTo(gameData * game, int num) {
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
	for (i = 0; i < 4 * game->bSize + game->m + 1; i++) {
		printf("-");
	}
	printf("\n");
}

/*
 * @param x - col of cell as user sees it (starting at 1)
 * @param y - row of cell as user sees it (starting at 1)
 * @param z - value to change to
 */
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

/*
 * Used by updateErrors to check for errors by cell.
 */
void handleError(gameData * game, int ** Arr, int i, int j, int Val) {
	if (Arr[Val] != NULL) { /*if already passed on a cell with the same value, a pointer to it was saved to Arr at Val position*/
		if (*(Arr[Val]) == 0) { /*if cell pointed to wasn't marked as error*/
			*(Arr[Val]) = 2;
			game->errors++;
		}
		if (game->board[i+game->bSize][j] == 0) { /*if current cell wasn't marked as error*/
			game->board[i+game->bSize][j] = 2;
			game->errors++;
		}
	}
	else if (Val != 0) { /*if no other cell was pointed to and value isn't 0*/
		Arr[Val] = &game->board[i+game->bSize][j]; /*add a pointer to the current cell to Arr at Val position*/
	}
}

/*
 * Goes over the cells and checks if there are any errors for each of them in their row, cols and block. The int pointer arrays are used to hold
 * pointers to the first cell with a certain value in each row, col or block. We go over each cell and check if a pointer was created at the position
 * of its value, if so it means there is already a cell with the same value as the current one, and we need to update errors for both of them if needed.
 * This is done in handleError.
 *
 */
void updateErrors(gameData * game) {
	int i, j, rowVal, colVal, blockJ, blockI, **rowArr = NULL, **colArr = NULL;
	rowArr = (int**)calloc(game->bSize +1 ,sizeof(int*));
	checkAlloc(rowArr);
	colArr = (int**)calloc(game->bSize +1 ,sizeof(int*));
	checkAlloc(colArr);
	for (i = 0; i < game->bSize; ++i) {  /*row and col check*/
		for (j = 0; j < game->bSize + 1; ++j) { /*erase pointer arrays*/
			rowArr[j] = NULL;
			colArr[j] = NULL;
		}
		for (j = 0; j < game->bSize; ++j) {
			rowVal = game->board[j][i];
			colVal = game->board[i][j];
			handleError(game, rowArr, j, i, rowVal); /*row check by value*/
			handleError(game, colArr, i, j, colVal); /*col check by value*/
		}
	}
	for (blockI = 0; blockI < game->n; blockI++) {
		for (blockJ = 0; blockJ < game->m; blockJ++) {
			for (j = 0; j < game->bSize + 1; ++j) { /*erase pointer array*/
				colArr[j] = NULL;
			}
			for (i = blockI*game->n; i < (blockI+1)*game->n; i++) {
				for (j = blockJ*game->m; j < (blockJ+1)*game->m; j++) { /*block check by value*/
					colVal = game->board[i][j];
					handleError(game, colArr, i, j, colVal);
				}
			}
		}
	}
	free(rowArr);
	free(colArr);
}

/*
 * Used by updateSetErrors. Let's it know if an error with the checked cell occurs by changed err int. Also, if an error occured it updates the status
 * of the other cells to erroneous and adds the change to set command's node in the command list and updates errors field.
 * Deals with deduction of errors as well.
 *
 * @param game - Game structure
 * @param x - col of cell as user sees it (starting at 1)
 * @param y - row of cell as user sees it (starting at 1)
 * @param prev - value changed from
 * @param z - value changed to
 * @param err - pointer to an int that let's updateSetErrors know if an error was found
 *
 */
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

/*
 * Use after you changed the cell's value in set command. Goes over the row, col and block of the cell changed and checks for errors with handleCellErrors.
 * If an error was found, update the cell's status to "erroneous" and add the error change to the set command's node in the command list.
 * Also update error field in gameData. Deals with deduction of errors as well.
 *
 * @param game - Game structure
 * @param x - col of cell as user sees it (starting at 1)
 * @param y - row of cell as user sees it (starting at 1)
 * @param prev - value changed from
 * @param z - value changed to
 *
 * @return
 * 	1 - update successful
 * 	0 - value wasn't changed
 *
 */
int updateSetErrors(gameData * game, int x, int y, int prev, int z) {
	int i, j, err = 0, cStart, rStart;
	if (z == prev) { /*value wasn't changed*/
		return 0;
	}
	for (i = 0; i < game->bSize; i++) {
		if (i != x - 1) { /*don't check the cell given*/
			handleCellErrors(game, i + 1, y, prev, z, &err);
		}
		if (i != y - 1) { /*don't check the cell given*/
			handleCellErrors(game, x, i + 1, prev, z, &err);
		}
	}
	cStart = (x-1) - ((x-1) % game->n); /*starting col of inner block*/
	rStart = (y-1) - ((y-1) % game->m); /*starting row of inner block*/
	for (i = cStart; i < cStart + game->n; i++) {
		for (j = rStart; j < rStart + game->m; j++) {
			if (i != x - 1 && j != y - 1) { /*don't check the cell given*/
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
		if(*cmd < 48 || *cmd > 57) { /*char doesn't represent digit 0-9*/
			return 0;
		}
		cmd++;
	}
	return 1;
}


int checkArgs(char ** cmdArr, int min, int max, int len) {
	int i;
	for (i=0; i < len; i++){ /*number of args passed*/
		if (!checkInt(cmdArr[i])){ /*if not int*/
			return 0;
		}
		if (i <= 1 && ((atoi(cmdArr[i]) < min) || (atoi(cmdArr[i]) > max))) { /*for X, Y in set, hint and generate commands*/
			return 0;
		}
		if (i == 2 && ((atoi(cmdArr[i]) < min - 1) || (atoi(cmdArr[i]) > max))) { /*for Z in set command*/
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

/*
 *
 */
int checkIfSolved(gameData * game) {
	if (game->numEmpty == 0 && game->mode == 1) { /*board has no empty cells and in solve mode*/
		if (!validate(game, 0)) { /*if board unsolved*/
			printf(ERROR_SOL);
			return 0;
		}
		else { /*board solved*/
			printf(PUZ_SOLVED);
			newGame(game, 0); /*init mode and new game*/
			return 1;
		}
	}
	return 0;
}



/*
 * Used by autofill to check if a cell has a single legal value (used on copy of the board). Checks all the value options and if only 1 is left returns it.
 *
 * @param gameC - copy of the game
 * @param i - cell's column how developer sees it (starts at 0)
 * @param j - cell's row how developer sees it (starts at 0)
 */
int singleValue(gameData * gameC, int i, int j) {
	int options = gameC->bSize, r = 0, c = 0, cStart, rStart;
	int * values = (int*)calloc(gameC->bSize, sizeof(int));
	checkAlloc(values);
	for (c = 0; c < gameC->bSize; c++) {
		values[c] = 1;
	}
	for (c = 0; c < gameC->bSize; c++) {
		if (gameC->board[c][j] != 0 && values[gameC->board[c][j] - 1] == 1) { /*options in row*/
			values[gameC->board[c][j] - 1] = 0;
			options--;
		}
		if (gameC->board[i][c] != 0 && values[gameC->board[i][c] - 1] == 1) { /*options in col*/
			values[gameC->board[i][c] - 1] = 0;
			options--;
		}
		if (options == 0) { /*no more options for cell*/
			free(values);
			return 0;
		}
	}
	cStart = (i) - ((i) % gameC->n); /*starting col of inner block*/
	rStart = (j) - ((j) % gameC->m); /*starting row of inner block*/
	for (c = cStart; c < cStart + gameC->n; c++) {
		for (r = rStart; r < rStart + gameC->m; r++) {
			if (gameC->board[c][r] != 0 && values[gameC->board[c][r] - 1] == 1) { /*options in block*/
				values[gameC->board[c][r] - 1] = 0;
				options--;
				if (options == 0) { /*no more options for cell*/
					free(values);
					return 0;
				}
			}
		}
	}
	if (options == 1) { /*if only 1 option left after all the checks*/
		for (c = 0; c < gameC->bSize; c++) { /*find the value and return it*/
			if (values[c] == 1) {
				free(values);
				return c + 1;
			}
		}
	}
	free(values);
	return 0;
}


int ilpSolver(gameData * game) {
	return gurobi_solver(game);
}

/*
 * Used by exhaustiveBT to traverse the board.
 * moves i,j in the direction dir.
 * Adds or removes the cell to/from the stack depending on the direction.
 *
 * @param dir - direction to move: 1 - forward, -1 - backward
 * @param i - cell's column how developer sees it (starts at 0)
 * @param j - cell's row how developer sees it (starts at 0)
 *
 */
void btMove(gameData * game, int * i, int * j, int dir, stack * stk, int * cell) {
	if (dir == 1) {
		if (*i == game->bSize - 1) { /*end of row*/
			*i = 0;
			(*j)++;
		}
		else { /*move to next cell in row*/
			(*i)++;
		}
		if (*j >= game->bSize) { /*push dummy cell*/
			push(stk, *i, *j, 0); /*add cell to stack*/
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
		pop(stk,cell); /*remove cell from stack and save it*/
	}
}

/*
 * Used by numSols to find number of board solutions. Does so on a copy of the game.
 * Check a cells options in order, puts a valid value in it and traverses the board using btMove.
 *
 * @return
 * 	counter - number of solutions
 *
 */
int exhaustiveBT(gameData * gameC) {
	int i = 0, j = 0, k, counter = 0 ,dir = 1, valid = 0;
	int cell[3] = {0};
	stack * stk = (stack*)malloc(sizeof(stack));
	checkAlloc(stk);
	initStack(gameC,stk);
	ChangeCellsWithValTo(gameC, 1); /*mark cells with value as fixed so we don't change them*/
	push(stk, 0, 0, gameC->board[0][0]);
	while(!isEmpty(stk)){
		top(stk, cell); /*get the cell at the top of stack*/
		i = cell[0];
		j = cell[1];
		if (gameC->board[gameC->bSize+i][j] != 0) { /*if cell is fixed*/
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
		if (isFull(stk)) { /*if the board is solved*/
			counter++; /*add 1 to counter of possible solutions*/
			dir = -1;
			btMove(gameC, &i, &j, dir, stk, cell);
		}
	}
	/*finished to check all possibilities*/
	freeStack(stk);
	return counter;
}


void fillXcells(gameData * game, int x, int * res) {
	int  k, f, i, j, numOfOnes, options = game->bSize;
	int * values;
	values = (int *)calloc(game->bSize, sizeof(int));
	checkAlloc(values);
	*res = 1;
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
	free(values);
}

