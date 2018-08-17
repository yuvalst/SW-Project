/*
 * main.c
 *
 *  Created on: 16 Aug 2018
 *      Author: guywaldman
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "Parser.h"

int main(int argc, char ** argv) {
	char ** cmdArr = (char **)calloc(3, sizeof(char*));
	/*assert*/
	gameData * game = initGame();
	if (argc == 0 || argv == NULL) {
		printf(" ");
	}
	while (1) {
		getCommand(game, cmdArr);
	}
	return 1;
}
