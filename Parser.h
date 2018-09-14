/*
 * Parser Summary:
 *
 * Functions used to receive input from user and pass it on to the relevant command function:
 *
 * getCommand - Receives the input from stdin and parsers it, then passing it to the different game functions accordingly or acts if something is wrong
 * syntacticly. Works with command lines of up to 256 chars (including).
 *
 *
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "Game.h"

/*
 * Receives the input from stdin and parsers it, then passing it to the different game functions accordingly or acts if something is wrong
 * syntacticly. Works with command lines of up to 256 chars (including).
 *
 * @param game - Game structure holding all the game data.
 * @param cmdArr -
 * @return
 * 	pointer to the new board
 */
int getCommand(gameData * game);


#endif /* PARSER_H_ */
