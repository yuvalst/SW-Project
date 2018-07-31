/*
 * Parser.c
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

#define cmdLen 256
#define DELIMITER " \t\r\n"
#define SOLVE "solve"
#define EDIT "edit"
#define MARK "mark_errors"
#define PRINT "print_board"
#define SET "set"
#define VALIDATE "validate"
#define GENERATE "generate"
#define UNDO "undo"
#define REDO "redo"
#define SAVE "save"
#define HINT "hint"
#define SOLS "num_solutions"
#define AUTO "auto_fill"
#define RESET "reset"
#define EXIT "exit"
#define ERROR_INV_CMD "ERROR: invalid command\n"

int checkToken(char * cmd){
	if(cmd == NULL) {
		printf(ERROR_INV_CMD); /*invalid command*/
		return 0;
	}
	return 1;
}

int getCommand(gameData * game, char ** cmdArr) {
	char input [cmdLen] = {0};
	char *cmd;
	int i;
	/*checkEOF = */
	fgets(input, cmdLen, stdin);
	/*if (checkEOF == NULL) {
		cmdArr[0] = -1;
		return 0;
	}*/
	cmd = strtok(input, DELIMITER);
	if(cmd == NULL) {
		return 0;
	}
	else if (strcmp(cmd, SOLVE) == 0) {
		cmd = strtok(NULL, DELIMITER);
		if(checkToken(cmd) == 0) { /*invalid command*/
			return 0;
		}
		solve(game, cmd);
	}
	else if (strcmp(cmd, EDIT) == 0) {
		cmd = strtok(NULL, DELIMITER);
		edit(game, cmd);
	}
	else if (strcmp(cmd, MARK) == 0) {
		cmd = strtok(NULL, DELIMITER);
		if(checkToken(cmd) == 0) {
			return 0;
		}
		mark_errors(game, cmd);
	}
	else if (strcmp(cmd, PRINT) == 0) {
		printBoard(game);
	}
	else if (strcmp(cmd, SET) == 0) {
		for (i = 0; i < 3; i++) {
			cmd = strtok(NULL, DELIMITER);
			if(checkToken(cmd) == 0) {
				return 0;
			}
			cmdArr[i] = cmd;
		}
		set(game, cmdArr);
	}
	else if (strcmp(cmd, VALIDATE) == 0) {
		validate(game);
	}
	else if (strcmp(cmd, GENERATE) == 0) {
		for (i = 0; i < 2; i++) {
			cmd = strtok(NULL, DELIMITER);
			if(checkToken(cmd) == 0) {
				return 0;
			}
			cmdArr[i] = cmd;
		}
		generate(game, cmdArr);
	}
	else if (strcmp(cmd, UNDO) == 0) {
		undo(game);
	}
	else if (strcmp(cmd, REDO) == 0) {
		redo(game);
	}
	else if (strcmp(cmd, SAVE) == 0) {
		cmd = strtok(NULL, DELIMITER);
		if(checkToken(cmd) == 0) {
			return 0;
		}
		save(game, cmd);
	}
	else if (strcmp(cmd, HINT) == 0) {
		for (i = 0; i < 2; i++) {
			cmd = strtok(NULL, DELIMITER);
			if(checkToken(cmd) == 0) {
				return 0;
			}
			cmdArr[i] = cmd;
		}
		hint(game, cmdArr);
	}
	else if (strcmp(cmd, SOLS) == 0) {
		numSols(game);
	}
	else if (strcmp(cmd, AUTO) == 0) {
		autoFill(game);
	}
	else if (strcmp(cmd, RESET) == 0) {
		reset(game);
	}
	else if (strcmp(cmd, EXIT) == 0) {
		exitGame(game);
	}
	else {
		printf(ERROR_INV_CMD);
	}
	return 0;
}