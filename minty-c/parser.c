#include <stdio.h>
#include "minty_util.h"
#include "lexer.h"
#include "AST.h"

/*
 * Checks that the 'found' string has an exact copy in the array of strings
 * 'expected'
 */
bool check_valid(char **expected, char *found) {
	printf("function: 'check_valid' not yet defined - defaulting to true\n");
	return true;
}

/*
 * This function is the entry point for the parser. It is responsible for making
 * calls to parse_function for each function in the program, putting each
 * function into a list, wrapping that list into a Program object and returning
 * that Program object.
 */
Program *parse_program(LinkedList *tokens) {
	LinkedList *function_list = LinkedList_init();

	while(LinkedList_length(tokens) < 0)
		LinkedList_append(parse_function(tokens));

	return Program_init(function_list);
}

/*
 * This function is responsible for parsing an entire function, and returning
 * that function in AST form as an FNDecl object. To acheive this, the
 * following steps occur:
 * 	- Process the 'fn' declaration
 * 	- Record the function's name
 * 	- Process the open-bracket that proceeds the arguments list
 * 	- Record the argument names, while processing the delimiting commas
 * 	- Process the close-bracket that ends the argument list
 * 	- Process the opening curly-bracket
 * 	- Iteratively process each statement in the function
 * 	- Process the closing curly-bracket
 * During these steps, the FNDecl ASTNode object is built up, and at the end it
 * is returned to the caller
 */
FNDecl *parse_function(LinkedList *tokens) {

	// Retrieve the first token, check that it's an FN
	Token *next_token = (Token *)LinkedList_pop(tokens);
	check_valid({"fn"}, next_token->type);

	// Retrieve the next token, check that it's an ID
	next_token = (Token *)LinkedList_pop(tokens);
	check_valid({"ID"}, next_token->type);

	// We know it's an ID now, so store its name
	char *function_name = safe_strdup(next_token->info);

	// Retrieve the next token, check it's an open-bracket
	next_token = (Token *)LinkedList_pop(tokens);
	check_valid({"("}, next_token->type)

	// Create a list for the argument names to be stored in
	LinkedList *arg_names = LinkedList_init();

	// Grab the first token in the argument list
	next_token = (Token *)LinkedList_pop(tokens);

	// Keep looking for argument names until we hit a ')' or an exception
	while(!str_equal(next_token->type, ")")) {
	
		// The only valid tokens at this point are IDs and commas to delimit
		// them
		check_valid({",", "ID"}, next_token->type);

		// Add the ID to the list of argument names
		if(str_equal(next_token->type, "ID"))
			LinkedList_append(arg_names, safe_strdup(next_token->info));

		// Retrieve the next token
		next_token = (Token *)LinkedList_pop(tokens);
	}
	// Create a list for the function's statements
	LinkedList *Statements = parse_statement_block(tokens);

	return FNDecl_init(function_name, arg_names, statements);
}