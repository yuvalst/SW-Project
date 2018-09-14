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
 * @return
 * 	1 - always.
 *
 */
int edit(gameData * game, char* path);

/*
 * Receives the game data and string array cmdArr and changes the "mark" attribute accordingly. Handles errors that are relevant.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array holding the argument given by the user.
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
 */
void printBoard(gameData * game);

/*
 * Receives game data and an array of arguments as strings, checks if the arguments are valid and if so changes the board accordingly.
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr - String array holding the arguments given by the user.
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
 * @param p - Binary argument to let the function know if we want it to print. 1 - yes, 0 - no.
 *
 * @return
 * 	1 - if board is solvable
 * 	0 - else.
 *
 */
int validate(gameData * game, int p);


int generate(gameData * game, char ** cmdArr);
int undo(gameData * game, int p);
int redo(gameData * game);
int save(gameData * game, char * path);
int hint(gameData * game, char ** cmdArr);
int numSols(gameData * game);
int autofill(gameData * game);
int reset(gameData * game);
void exitGame(gameData * game);

#endif /* GAME_H_ */
