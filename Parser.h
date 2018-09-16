/*
 * Parser Summary:
 *
 * Functions used to receive input from the user and pass it on to the relevant command function.
 *
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "Game.h"

/*
 * Receives the input from stdin and parsers it, then passes it to the different game functions accordingly or acts if something is wrong
 * syntacticly. Works with command lines of up to 256 chars (including).
 *
 * @param game - Game structure holding all the game data.
 *
 * @return
 * 	1 - if parsing succeeded and function was called.
 * 	0 - if there was an invalid command
 * 	-1 - if EOF or "exit"
 */
int getCommand(gameData * game);


#endif /* PARSER_H_ */
