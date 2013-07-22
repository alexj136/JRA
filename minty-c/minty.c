#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "minty_util.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"

int main() {
	LinkedList *tokens = lex("fn main() {return 0;}");
	
	int i;
	for(i = 0; i < LinkedList_length(tokens); i++)
		Token_print((Token *)LinkedList_get(tokens, i));

	Program *p = parse_program(tokens);

	//LinkedList_free(tokens);
	return 0;
}