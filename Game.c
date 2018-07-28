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

typedef struct GameData {
	int mode; /*0 - init, 1 - solve, 2 - edit*/
	int mark;
	int errors;
	int n;
	int m;
	int bSize;
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
	game->bSize = game->n*game->m;
	game->board = NULL;
	game->head = NULL;
	game->curr = NULL;
	return game;
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

int set(gameData * game, int x, int y, int z){

}

int validate(gameData * game, int x, int y, int z){

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

int num_solutions(gameData * game, int x, int y, int z){

}

int autofill(gameData * game, int x, int y, int z){

}

int reset(gameData * game, int x, int y, int z){

}

int exit(gameData * game, int x, int y, int z){

}

