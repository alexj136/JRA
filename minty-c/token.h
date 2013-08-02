#ifndef TOKEN
#define TOKEN

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
 * Struct used to represent a token
 */
typedef struct {
	token_type type;
	char *info;
} Token;

/*
 * Token functions
 */

Token *Token_init(token_type type, char *info);

void Token_set_type(Token *token, token_type type);

void Token_set_info(Token *token, char *info);

char *Token_str(token_type type);

void Token_print(Token *token);

void Token_free(Token *token);

#endif // TOKEN