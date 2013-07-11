/*
 * Header file for lexer.c
 * Contains structs used to represent tokens and lists of tokens, and functions
 * to free those objects from memory. A function for printing tokens and of
 * course the lex function itself are included.
 */


/*
 * Struct used to represent a token
 */
typedef struct {
	char *type;
	char *info;
} Token;

void Token_print(Token *token);

void Token_free(Token *token);

/*
 * TokenNode struct represents a node in a linked list of tokens. Thus it
 * contains a pointer to a Token and a pointer to another TokenNode.
 */
typedef struct {
	Token *token;
	struct TokenNode *child_node;
} TokenNode;

void TokenNode_free(TokenNode *node);

TokenNode *lex(char *input);