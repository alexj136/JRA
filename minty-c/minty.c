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
	Program *q = parse_program(lex("fn main() {return 0;}"));
	Program *r = parse_program(lex("fn main() {return 1;}"));

	printf("p %s= q\n", Program_equals(p, q) ? "" : "!");
	printf("p %s= r\n", Program_equals(p, r) ? "" : "!");

	for(i = 0; i < LinkedList_length(tokens); i++)
		Token_free((Token *)LinkedList_get(tokens, i));
	LinkedList_free(tokens);
	Program_free(p);

	return 0;
}