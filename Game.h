/*
 * Game Summary:
 *
 * Functions representing the different user commands. Each one takes care of a command's situations.
 * Defines the struct gameData.
 *
 * solve - Starts a puzzle in "solve" mode of puzzle saved in file named the string saved in path. Notifies if not possible.
 * edit - Starts a puzzle in "edit" mode of puzzle saved in file named the string saved in path or of empty 9X9 board if path wasn't passed.
 * markErrors - Changes the mark errors attribute according to the input.
 * printBoard - Prints the board, takes in account the different modes and attributes.
 * set - Sets the value of cell <X,Y> to Z or acts accordingly of the different situations.
 * validate - Checks if the current game board had a solutions and notifies the user.
 * generate - Creates a new puzzle filling the requested number of cells.
 * undo - Undo a move done by the user.
 * redo - Redo a move done by the user.
 * save - Save the current puzzle to a file named by the string passed as path. First checks the relevant attributes needed to save.
 * hint - Give a hint to the user for a single cell.
 * numSols - Checks how many solutions the current board has and notifies the user.
 * autofill - Finds cells which have only one possible legal value and fills them with it.
 * reset - Undoes all moves therefore returning the board the its initial loaded state. User can't redo afterwards.
 * exit - Stops the game and exits cleanly.
 *
 */

#ifndef GAME_H_
#define GAME_H_




typedef struct node node;


/* Game data structure used to hold all the relevant information*/
typedef struct gameData {
	int mode; /*0 - init, 1 - solve, 2 - edit*/
	int mark;
	int errors;
	int m;
	int n;
	int bSize;
	int numEmpty; /*number of empty cells*/
	int ** board;
	node * head;
	node * curr;
}gameData;


/*
 * Receives the string path and looks for a file with that path. If found, starts a new game in "solve" mode and recreates the board from the data
 * saved in it. Then prints the board. Else, notifies the user accordingly.
 *
 * @param game - Game structure holding all the game data.
 * @param path - String holding the path to the file given by the user.
 *
 * @mode - solve, edit, init
 *
 *
 * @return
 * 	1 - if file found and new game opened.
 * 	0 - if file not found or error occurred trying to open it.
 *
 */
int solve(gameData * game, char * path);

/*
 * Receives the game data and string path and looks for a file with that path. Starts a new game in "edit" mode. If file was found, recreates the board
 * from the data saved in it. Else, starts a 9X9 empty puzzle. Prints the board.
 *
 * @param game - Game structure holding all the game data.
 * @param path - String holding the path to the file given by the user.
 *
 * @mode - solve, edit, init
 *
 * @return
 * 	1 - always.
 *
 */
int edit(gameData * game, char* path);

/*
 * Receives the game data and string array cmdArr and changes the "mark" attribute accordingly. Wanted argument should be placed at cmdArr[0].
 * Handles errors that are relevant.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array (of size 3) holding the argument given by the user. Legal values - 0, 1.
 *
 * @mode - solve
 *
 * @return
 * 	1 - if legal argument was given and command executed.
 * 	0 - else.
 *
 */
int markErrors(gameData * game, char ** cmdArr);

/*
 * Prints the game board.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve, edit
 *
 */
void printBoard(gameData * game);

/*
 * Receives game data and an array of arguments as strings, checks if the arguments are valid and if so changes the board accordingly. Then prints the board.
 * If puzzle was filled, checks if solutions is valid or not and tells the user. If solved, changes mode to init.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array (of size 3) holding the arguments given by the user.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if legal arguments were given and command executed.
 * 	0 - else.
 *
 */
int set(gameData * game, char ** cmdArr);

/*
 * Checks if the current game board had a solutions and notifies the user.
 *
 * @param game - Game structure holding all the game data.
 * @param p - Binary argument to let the function know if we want it to print messages. 1 - print, 0 - don't print.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if board is solvable
 * 	0 - else.
 *
 */
int validate(gameData * game, int p);

/*
 * Receives game data and an array of arguments as strings, checks if the arguments are valid and board is empty and if so generates a puzzle board
 * accordingly.
 * First argument decides how many cells to fill with legal values before solving the board, second argument decides how many cells should have values
 * in the puzzle.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array (of size 3) holding the arguments given by the user.
 *
 * @mode - edit
 *
 * @return
 * 	1 - if legal arguments were given and command executed under 1000 tries.
 * 	0 - else.
 *
 */
int generate(gameData * game, char ** cmdArr);

/*
 * Reverts one command back if available then prints the board and notifies which move was reverted.
 *
 * @param game - Game structure holding all the game data.
 * @param p - Binary argument to let the function know if we want it to print. 1 - yes, 0 - no.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if command available to undo.
 * 	0 - else.
 *
 */
int undo(gameData * game, int p);

/*
 * Redoes one command forward if available then prints the board and notifies which move was made again.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 -  if command available to redo.
 * 	0 - else.
 *
 */
int redo(gameData * game);

/*
 * Saves the current game's puzzle board to a file or notifies if error occurred.
 *
 * @param game - Game structure holding all the game data.
 * @param path - String holding the name of the file given by the user.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if board successfully saved.
 * 	0 - else.
 *
 */
int save(gameData * game, char * path);

/*
 * Receives game data and array of arguments as strings and prints a hint for the given cell.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array (of size 3) holding the arguments given by the user.
 *
 * @mode - solve
 *
 * @return
 * 	1 - if board is solvable and hint was printed.
 * 	0 - else.
 *
 */
int hint(gameData * game, char ** cmdArr);

/*
 * Check the number of solutions available to the current puzzle on a copy of the game and prints it to the user.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if board copy was solved and number of solutions was printed.
 * 	0 - else.
 *
 */
int numSols(gameData * game);

/*
 * Finds cells which have only one possible legal value and fills them with it. Prints the cells changed and their values.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve
 *
 * @return
 * 	1 - if command called in "solve" mode with no erroneous cells.
 * 	0 - else.
 *
 */
int autofill(gameData * game);

/*
 * Undoes all moves therefore returning the board the its initial loaded state. User can't redo afterwards.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve, edit
 *
 * @return
 * 	1 - if command called in "solve" or "edit" modes.
 * 	0 - else.
 *
 */
int reset(gameData * game);

/*
 * Stops the game and exits cleanly.
 *
 * @param game - Game structure holding all the game data.
 *
 * @mode - solve, edit
 *
 */
void exitGame(gameData * game);

#endif /* GAME_H_ */
