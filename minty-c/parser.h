#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef AST
#include "AST.h"
#endif // AST

#ifndef PARSER
#define PARSER

/*
 * Parser functions
 */

Expression *parse_expression(LinkedList *tokens);

Statement *parse_statement(LinkedList *tokens);

LinkedList *parse_statement_block(LinkedList *tokens);

FNDecl *parse_function(LinkedList *tokens);

Program *parse_program(LinkedList *tokens);

#endif // PARSER