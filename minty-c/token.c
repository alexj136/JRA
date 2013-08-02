#include <stdio.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"

/*
 * 'Constructor' for Token structs. Tokens can indicate errors as well as valid
 * tokens. Errors simply have "ERROR" in their type field, whereas valid tokens
 * have token type names. Takes pointers to info & type strings, and uses them
 * to create heap-allocated copies. If they were not copies, the struct would
 * not be the only owner of the strings, which would potentially cause problems.
 */
Token *Token_init(token_type type, char *info) {
	// Allocate heap space for this Token
	Token *token = safe_alloc(sizeof(Token));

	// Copy & allocate the type string
	token->type = type;

	// Copy and allocate the info string
	token->info = safe_strdup(info);

	// Return a pointer to this Token
	return token;
}

/*
 * Setter for the token type field
 */
void Token_set_type(Token *token, token_type type) {
	token->type = type;
}

/*
 * Setter for the token info field - frees the old value, allocates the new one
 */
void Token_set_info(Token *token, char *info) {
	free(token->info);
	token->info = safe_strdup(info);
}

/*
 * Returns a string representation of a token type
 */
char *Token_str(token_type type) {
	
	char *token_str[] = {
		"ERROR",
		"IDENTIFIER",
		"LITERAL",
		"FN",
		"FOR",
		"WHILE",
		"IF",
		"ELSE",
		"PRINT",
		"RETURN",
		"EQUAL",
		"NOT_EQUAL",
		"LESS_THAN",
		"LESS_OR_EQUAL",
		"GREATER_THAN",
		"GREATER_OR_EQUAL",
		"PLUS",
		"MINUS",
		"MULTIPLY",
		"DIVIDE",
		"MODULO",
		"ASSIGNMENT",
		"INCREMENT",
		"DECREMENT",
		"INCREMENT_BY",
		"DECREMENT_BY",
		"OPEN_BRACE",
		"CLOSE_BRACE",
		"OPEN_PAREN",
		"CLOSE_PAREN",
		"COMMA",
		"SEMICOLON",
		"QUESTION_MARK",
		"COLON"
	};

	return token_str[type];
}

/*
 * Prints a token in the format 'T_type' or 'T_type_info'
 */
void Token_print(Token *token) {
	printf("T_%s", Token_str(token->type));
	if(*(token->info) != '\0') printf("_%s\n", token->info);
	else printf("\n");
}

/*
 * Deallocates a token object from the heap
 */
void Token_free(Token *token) {
	free(token->info);
	free(token);
}