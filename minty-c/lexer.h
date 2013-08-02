/*
 * Header file for lexer.c
 * Contains the lex function.
 */

#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef TOKEN
#include "token.h"
#endif // TOKEN

#ifndef LEXER
#define LEXER

/*
 * Lexer function
 */
LinkedList *lex(char *input);

#endif // LEXER