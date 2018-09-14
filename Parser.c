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

int checkTokens(char ** cmd, char ** cmdArr, int inputs, int * res){
	int i;
	for (i = 0; i < inputs; i++) {
		*cmd = strtok(NULL, DELIMITER);
		if(*cmd == NULL) {
			printf(ERROR_INV_CMD); /*invalid command*/
			*res = 0;
			return 0;
		}
		cmdArr[i] = *cmd;
	}
	*res = 1;
	return 1;
}

int checkLine(char input []) {
	if (!strchr(input, '\n')) { /*line has more tham 256 chars*/
		printf(ERROR_INV_CMD);
		while (!strchr(input,'\n') && fgets(input, cmdLen, stdin)) { }
		return 0;
	}
	return 1;
}


int getCommand(gameData * game, char ** cmdArr) {
	char input [cmdLen + 1] = {0};
	char * cmd = NULL, * checkEOF = NULL;
	int res = 0;
	checkEOF = fgets(input, cmdLen, stdin);
	if (checkEOF == NULL) { /*EOF - main exits game*/
		return -1;
	}
	if (checkLine(input) == 0) { /*check if command exceeds 256 chars*/
		return 0;
	}
	cmd = strtok(input, DELIMITER);
	if (cmd == NULL) { /*only whitespace*/
		return 0;
	}
	if (strcmp(cmd, SOLVE) == 0) { /*solve*/
		if (checkTokens(&cmd, cmdArr, 1, &res))
			solve(game, cmd);
	}
	else if (strcmp(cmd, EDIT) == 0) { /*edit*/
		cmd = strtok(NULL, DELIMITER);
		edit(game, cmd);
	}
	else if (strcmp(cmd, MARK) == 0) { /*mark_errors*/
		if (checkTokens(&cmd, cmdArr, 1, &res))
			markErrors(game, cmdArr);
	}
	else if (strcmp(cmd, PRINT) == 0) {	printBoard(game); } /*print_board*/
	else if (strcmp(cmd, SET) == 0) { /*set*/
		if (checkTokens(&cmd, cmdArr, 3, &res))
			set(game, cmdArr);
	}
	else if (strcmp(cmd, VALIDATE) == 0) { validate(game, 1); } /*validate*/
	else if (strcmp(cmd, GENERATE) == 0) { /*generate*/
		if (checkTokens(&cmd, cmdArr, 2, &res))
			generate(game, cmdArr);
	}
	else if (strcmp(cmd, UNDO) == 0) { undo(game, 1); } /*undo*/
	else if (strcmp(cmd, REDO) == 0) { redo(game); } /*redo*/
	else if (strcmp(cmd, SAVE) == 0) { /*save*/
		if (checkTokens(&cmd, cmdArr, 1, &res))
			save(game, cmd);
	}
	else if (strcmp(cmd, HINT) == 0) { /*hint*/
		if (checkTokens(&cmd, cmdArr, 2, &res))
			hint(game, cmdArr);
	}
	else if (strcmp(cmd, SOLS) == 0) { numSols(game); } /*num_solutions*/
	else if (strcmp(cmd, AUTO) == 0) { autofill(game); } /*autofill*/
	else if (strcmp(cmd, RESET) == 0) {	reset(game); } /*reset*/
	else if (strcmp(cmd, EXIT) == 0) { res = -1; } /*exit*/
	else { printf(ERROR_INV_CMD); } /*invalid command*/
	return res;
}
