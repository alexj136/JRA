#include <stdio.h>
#include "minty_util.h"
#include "AST.h"

/*
 * This function is the entry point for the parser. It is responsible for making
 * calls to parse_function for each function in the program, putting each
 * function into a list, wrapping that list into a Program object and returning
 * that Program object.
 */
Program *parse_program(LinkedList *tokens) {
	LinkedList_init();
	return NULL;
}