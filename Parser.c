/*
 * Parser.c
 *
 * Functions used to receive input from the user and pass it on to the relevant command function.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"


#define cmdLen 258
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


/*
 * Used by getCommand() to check if there are enough arguments in the input buffer for the command called. If so, adds them to cmdArr.
 *
 * @param cmd - Pointer to string used by getCommand().
 * @param cmdArr - String array used by getCommand() to pass arguments to the different game functions.
 * @param inputs - number of arguments expected.
 * @param res - pointer to int used by getCommand() to return to the main function.
 *
 * @return
 * 	1 - if there are enough arguments.
 * 	0 - if there were less than expected arguments.
 */
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

/*
 * Used by getCommand() to check if the command line from the user passes 256 chars or not. if so, prints that the command is invalid and clears the line
 * until \n.
 *
 * @param input - Input buffer from getCommand() holding the command line brought by fgets().
 *
 * @return
 * 	1 - if the line is less than 257 chars.
 * 	0 - else
 */
int checkLine(char input []) {
	if (!strchr(input, '\n') && !feof(stdin)) { /*line has more than 256 chars because '\n' isn't found in the first 256 chars*/
		printf(ERROR_INV_CMD);
		while (!strchr(input,'\n') && fgets(input, cmdLen, stdin)) { } /*clear input buffer*/
		return 0;
	}
	return 1;
}

/*
 * Receives the input from stdin and parsers it, then passes it to the different game functions accordingly or acts if something is wrong
 * syntacticly. Works with command lines of up to 256 chars (including).
 * First checks if there was EOF at the beginning of the line using fgets. If so, returns -1 for main to exit game.
 * Then checks if line doesn't exceed 256 chars.
 * If not, handles whitespace line or calls game function needed (checks if there are enough "tokens" before).
 *
 * @param game - Game structure holding all the game data.
 *
 * @return
 * 	1 - if parsing succeeded and function was called.
 * 	0 - if there was an invalid command
 * 	-1 - if EOF or "exit"
 */
int getCommand(gameData * game) {
	char input [cmdLen + 1] = {0};
	char * cmd = NULL, * cmdArr[3], * checkEOF = NULL;
	int res = 0;
	checkEOF = fgets(input, cmdLen, stdin); /*get 256 chars from stdin into input char array*/
	if (checkEOF == NULL) { /*EOF - main exits game*/
		return -1;
	}
	if (checkLine(input) == 0) { /*check if command exceeds 256 chars or reached EOF after reading from stdin*/
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
