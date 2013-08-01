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
 * Enum used to represent the different types a token can have
 */
typedef enum {

	// The error token
	ERROR,

	// Tokens that carry associated values
	IDENTIFIER,
	LITERAL,	

	// Keywords
	FN,
	FOR,
	WHILE,
	IF,
	ELSE,
	PRINT,
	RETURN,
	
	// Boolean operations
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_OR_EQUAL,
	GREATER_THAN,
	GREATER_OR_EQUAL,

	// Arithmetic operations
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MODULO,

	// Assignment operators
	ASSIGNMENT,
	INCREMENT,
	DECREMENT,
	INCREMENT_BY,
	DECREMENT_BY,

	// Brackets
	OPEN_BRACE,
	CLOSE_BRACE,
	OPEN_PAREN,
	CLOSE_PAREN,

	// Delimiters
	COMMA,
	SEMICOLON,

	// Ternary operator tokens
	QUESTION_MARK,
	COLON

} token_type;

/*
 * Array of strings representing tokens, used to convert an enum token type into
 * a string for printing purposes
 */
char *token_to_string[] = {
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

/*
 * Struct used to represent a token
 */
typedef struct {
	token_type type;
	char *info;
} Token;

void Token_print(Token *token);

void Token_free(Token *token);

/*
 * Lexer function
 */
LinkedList *lex(char *input);

#endif // LEXER