#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "minty_util.h"
#include "lexer.h"

int main() {
	LinkedList *tokens = lex("x <- 10; x++; for i <- 0, i < 10 { blah }");
	while(LinkedList_length(tokens) > 0)
		Token_print((Token *)LinkedList_pop(tokens));
	return 0;
}