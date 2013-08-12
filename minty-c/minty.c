#include <stdio.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "interpreter.h"

int evaluate_program(char *source_code, LinkedList *args) {

	// Lex the program to obtain the token list
	LinkedList *tokens = lex(source_code);

	// Run the parser on the token list to obtain an AST
	Program *ast = parse_program(tokens);

	// Now we have the AST, we can free the tokens
	int i;
	for(i = 0; i < LinkedList_length(tokens); i++) {
		Token_free((Token *)LinkedList_get(tokens, i));
	}
	LinkedList_free(tokens);

	// Evaluate the program and store the result
	int result = interpret_program(ast, args);

	// Now we have the result, we can free the AST
	Program_free(ast);

	// Finally return the result of the program
	return result;
}

int main() {
	LinkedList *args = LinkedList_init();

	printf("%d\n", evaluate_program("\
		fn main() {                  \
			return hundred();        \
		}                            \
		fn hundred() {               \
			return 100;              \
		}", args));

	free(args);
	return 0;
}