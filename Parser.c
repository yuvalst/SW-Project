/*
 * Parser.c
 *
 *  Created on: 23 Jul 2018
 *      Author: guywaldman
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"


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
#define AUTO "autofill"
#define RESET "reset"
#define EXIT "exit"
#define ERROR_INV_CMD "ERROR: invalid command\n"

int checkTokens(char ** cmd, char ** cmdArr, int inputs){
	int i;
	for (i = 0; i < inputs; i++) {
		*cmd = strtok(NULL, DELIMITER);
		if(*cmd == NULL) {
			printf(ERROR_INV_CMD); /*invalid command*/
			return 0;
		}
		cmdArr[i] = *cmd;
	}
	return 1;
}

int getCommand(gameData * game, char ** cmdArr) {
	char input [cmdLen + 1] = {0};
	char * cmd = NULL, * checkEOF = NULL;
	do {
		checkEOF = fgets(input, cmdLen, stdin);
		if (checkEOF == NULL) {
			return -1;
		}
		cmd = strtok(input, DELIMITER);
	}
	while (cmd == NULL); /*only whitespace*/
	if (strcmp(cmd, SOLVE) == 0) {
		if (checkTokens(&cmd, cmdArr, 1) == 0) { return 0; }
		solve(game, cmd);
	}
	else if (strcmp(cmd, EDIT) == 0) {
		cmd = strtok(NULL, DELIMITER);
		edit(game, cmd);
	}
	else if (strcmp(cmd, MARK) == 0) {
		if (checkTokens(&cmd, cmdArr, 1) == 0) { return 0; }
		markErrors(game, cmdArr);
	}
	else if (strcmp(cmd, PRINT) == 0) {	printBoard(game); }
	else if (strcmp(cmd, SET) == 0) {
		if (checkTokens(&cmd, cmdArr, 3) == 0) { return 0; }
		set(game, cmdArr);
	}
	else if (strcmp(cmd, VALIDATE) == 0) { validate(game, 1); }
	else if (strcmp(cmd, GENERATE) == 0) {
		if (checkTokens(&cmd, cmdArr, 2) == 0) { return 0; }
		generate(game, cmdArr);
	}
	else if (strcmp(cmd, UNDO) == 0) { undo(game, 1); }
	else if (strcmp(cmd, REDO) == 0) { redo(game); }
	else if (strcmp(cmd, SAVE) == 0) {
		if (checkTokens(&cmd, cmdArr, 1) == 0) { return 0; }
		save(game, cmd);
	}
	else if (strcmp(cmd, HINT) == 0) {
		if (checkTokens(&cmd, cmdArr, 2) == 0) { return 0; }
		hint(game, cmdArr);
	}
	else if (strcmp(cmd, SOLS) == 0) { numSols(game); }
	else if (strcmp(cmd, AUTO) == 0) { autofill(game); }
	else if (strcmp(cmd, RESET) == 0) {	reset(game); }
	else if (strcmp(cmd, EXIT) == 0) { return -1; }
	else { printf(ERROR_INV_CMD); }
	return 0;
}
