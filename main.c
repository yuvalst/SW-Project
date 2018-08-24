/*
 * main.c
 *
 *  Created on: 16 Aug 2018
 *      Author: guywaldman
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Game.h"
#include "GameAux.h"
#include "Parser.h"
#define  ENTER_CMD "Enter your command:\n"
#define SUDOKU "Sudoku\n------\n"

/*for save*/

int main(void) {
	int run = 1;
	time_t seed;
	char * cmdArr[3];
	gameData * game = initGame();
	printf(SUDOKU);
	seed = time(NULL);
	srand(seed);
	while (run != -1) {
		printf(ENTER_CMD);
		run = getCommand(game, cmdArr);
		/*fseek(stdin,0,SEEK_END);*/  /*flush stdin*/
	}
	exitGame(game);
	return 1;
}
