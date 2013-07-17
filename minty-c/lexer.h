/*
 * Header file for lexer.c
 * Contains structs used to represent tokens and lists of tokens, and functions
 * to free those objects from memory. A function for printing tokens and of
 * course the lex function itself are included.
 */

#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef LEXER
#define LEXER

/*
 * Struct used to represent a token
 */
typedef struct {
	char *type;
	char *info;
} Token;

void Token_print(Token *token);

void Token_free(Token *token);

/*
 * Lexer function
 */
LinkedList *lex(char *input);

#endif // LEXER