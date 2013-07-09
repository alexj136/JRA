/* 
 * This file contains functions and data structures that are used for lexical
 * analysis.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

/*
 * Struct used to represent a token
 */
typedef struct {
	char *type;
	char *info;
} Token;

/*
 * 'Constructor' for Token structs. Tokens can indicate errors as well as valid
 * tokens. Errors simply have "ERROR" in their type field, whereas valid tokens
 * have token type names. Takes pointers to info & type strings, and uses them
 * to create heap-allocated copies. If they were not copies, the struct would
 * not be the only owner of the strings, which would potentially cause problems.
 */
Token *Token_init(char *type, char *info) {
	// Allocate heap space for this Token
	Token *token = malloc(sizeof(Token));

	// Copy & allocate the type string
	token->type = strdup(type);

	// Copy and allocate the info string
	token->info = strdup(info);

	// Return a pointer to this Token
	return token;
}

/*
 * Setter for the token type field - frees the old value, allocates the new one
 */
void Token_set_type(Token *token, char *type) {
	free(token->type);
	token->type = strdup(type);
}

/*
 * Setter for the token info field - frees the old value, allocates the new one
 */
void Token_set_info(Token *token, char *info) {
	free(token->info);
	token->info = strdup(info);
}

/*
 * Deallocates a token object - frees each string, followed by the object itself
 */
void Token_free(Token *token) {
	free(token->type);
	free(token->info);
	free(token);
}

/*
 * Prints a token in the format 'T_type' or 'T_type_info'
 */
void Token_print(Token *token) {
	printf("T_%s", token->type);
	if(*(token->info) != '\0') printf("_%s\n", token->info);
	else printf("\n");
}

/*
 * TokenNode struct represents a node in a linked list of tokens. Thus it
 * contains a pointer to a Token and a pointer to another TokenNode.
 */
typedef struct {
	Token *token;
	struct TokenNode *child_node;
} TokenNode;

/*
 * Constructs a root TokenNode object
 */
TokenNode *TokenNode_init_root(Token *root_token) {
	TokenNode *root_node = malloc(sizeof(TokenNode));
	root_node->token = root_token;
	root_node->child_node = NULL;
	return root_node;
}

/*
 * Constructs a TokenNode as a child of the passed node
 */
TokenNode *TokenNode_init(TokenNode *parent_node, Token *new_token) {
	TokenNode *new_node = malloc(sizeof(TokenNode));
	parent_node->child_node = new_node;
	new_node->token = new_token;
	new_node->child_node = NULL;
	return new_node;
}

/*
 * Frees the given TokenNode and all child TokenNodes. Should only be called on
 * the head of a list, otherwise the last of the non-free nodes will point to
 * an area of memory that the program does not have allocated, and a segfault
 * will likely occur if that pointer is subsequently used
 */
void TokenNode_free(TokenNode *node) {
	TokenNode_free(node->child_node);
	Token_free(node->token);
	free(node);
}

/*
 * Struct that contains the information returned as a result of retrieving a
 * token from the string given to get_next_token
 */
typedef struct {
	int chars_processed;
	Token *token;
} TupleIntToken;

/*
 * Create a TupleIntToken struct - a container for a token and an int. There is
 * no free function for TupleIntToken because we don't need to free the int, we
 * wouldn't want to free the contained token - that will be done elsewhere, and
 * thus it suffices to call free(instance_of_TupleIntToken)
 */
TupleIntToken *TupleIntToken_init(Token *token, int chars_processed) {
	TupleIntToken *tuple = malloc(sizeof(TupleIntToken));
	tuple->chars_processed = chars_processed;
	tuple->token = token;
	return tuple;
}

int transition_table[54][36] = {//                                     abcdjkx
// e   f   g   h   i   l   m   n   o   p   q   r   s   t   u   v   w   yzCAPS_   0-9   {   }   (   )   *   /   %   ,   ;   ?   :   <   >   =   +   -   !
{  50,  1, 49, 49,  4, 49, 49, 49, 49,  2, 49,  3, 49, 49, 49, 49,  5, 49,       48,   37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 23, 26, 36, 28, 31, 34 },  // state 0
{  49, 49, 49, 49, 49, 49, 49, 17,  6, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 1
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  8, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 2
{  12, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 3
{  49, 18, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 4
{  49, 49, 49, 19, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 5
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  7, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 6
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 7
{  49, 49, 49, 49,  9, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 8
{  49, 49, 49, 49, 49, 49, 49, 10, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 9
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 11, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 10
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 11
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 13, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 12
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 14, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 13
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 15, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 14
{  49, 49, 49, 49, 49, 49, 49, 16, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 15
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 16
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 17
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 18
{  49, 49, 49, 49, 20, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 19
{  49, 49, 49, 49, 49, 21, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 20
{  22, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 21
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 22
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 25, -1, 24, -1 },  // state 23
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 24
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 25
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 27, -1, -1, -1 },  // state 26
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 27
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 29, -1, -1 },  // state 28
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 29
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 30
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, 32, -1 },  // state 31
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 32
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 33
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 35, -1, -1, -1 },  // state 34
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 35
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 36
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 37
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 38
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 39
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 40
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 41
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 42
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 43
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 44
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 45
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 46
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 47
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       48,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 48
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 49
{  49, 49, 49, 49, 49, 51, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 50
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 52, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 51
{  53, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 52
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }}; // state 53

/* 
 * Maps possible input characters to columns in the finite state machine table.
 * If a character that does not belong in the alphabet of the language, -1 is
 * returned to indicate an error.
 */
int char_indices(char letter) {
	if     (letter == 'a') return 17; else if(letter == 'b') return 17;
	else if(letter == 'c') return 17; else if(letter == 'd') return 17;
	else if(letter == 'e') return  0; else if(letter == 'f') return  1;
	else if(letter == 'g') return  2; else if(letter == 'h') return  3;
	else if(letter == 'i') return  4; else if(letter == 'j') return 17;
	else if(letter == 'k') return 17; else if(letter == 'l') return  5;
	else if(letter == 'm') return  6; else if(letter == 'n') return  7;
	else if(letter == 'o') return  8; else if(letter == 'p') return  9;
	else if(letter == 'q') return 10; else if(letter == 'r') return 11;
	else if(letter == 's') return 12; else if(letter == 't') return 13;
	else if(letter == 'u') return 14; else if(letter == 'v') return 15;
	else if(letter == 'w') return 16; else if(letter == 'x') return 17;
	else if(letter == 'y') return 17; else if(letter == 'z') return 17;
	else if(letter == 'A') return 17; else if(letter == 'B') return 17;
	else if(letter == 'C') return 17; else if(letter == 'D') return 17;
	else if(letter == 'E') return 17; else if(letter == 'F') return 17;
	else if(letter == 'G') return 17; else if(letter == 'H') return 17;
	else if(letter == 'I') return 17; else if(letter == 'J') return 17;
	else if(letter == 'K') return 17; else if(letter == 'L') return 17;
	else if(letter == 'M') return 17; else if(letter == 'N') return 17;
	else if(letter == 'O') return 17; else if(letter == 'P') return 17;
	else if(letter == 'Q') return 17; else if(letter == 'R') return 17;
	else if(letter == 'S') return 17; else if(letter == 'T') return 17;
	else if(letter == 'U') return 17; else if(letter == 'V') return 17;
	else if(letter == 'W') return 17; else if(letter == 'X') return 17;
	else if(letter == 'Y') return 17; else if(letter == 'Z') return 17;
	else if(letter == '0') return 18; else if(letter == '1') return 18;
	else if(letter == '2') return 18; else if(letter == '3') return 18;
	else if(letter == '4') return 18; else if(letter == '5') return 18;
	else if(letter == '6') return 18; else if(letter == '7') return 18;
	else if(letter == '8') return 18; else if(letter == '9') return 18;
	else if(letter == '_') return 17; else if(letter == '{') return 19;
	else if(letter == '}') return 20; else if(letter == '(') return 21;
	else if(letter == ')') return 22; else if(letter == '*') return 23;
	else if(letter == '/') return 24; else if(letter == '%') return 25;
	else if(letter == ',') return 26; else if(letter == ';') return 27;
	else if(letter == '?') return 28; else if(letter == ':') return 29;
	else if(letter == '<') return 30; else if(letter == '>') return 31;
	else if(letter == '=') return 32; else if(letter == '+') return 33;
	else if(letter == '-') return 34; else if(letter == '!') return 35;
	else return -1;
}

/* 
 * Maps final states to strings that represent token types, for example, if
 * the FSM traversal finished on final state 3, the token type would be "ID"
 * because final_mapping[3] = "ID".
 */
char *final_mapping[] = {
	"ERROR", "ID", "ID", "ID", "ID", "ID", "ID", "for", "ID", "ID", "ID",
	"print", "ID", "ID", "ID", "ID", "return", "fn", "if", "ID", "ID", "ID",
	"while", "<", "<-", "<=", ">", ">=", "+", "++", "+=", "-", "--", "-=",
	"ERROR", "!=", "=", "{", "}", "(", ")", "*", "/", "%%", ",", ";", "?", ":",
	"INT", "ID", "ID", "ID", "ID", "else"
};

/*
 * Array that contains every token type that does not have any associated info
 * such as identifier name on integer literal value
 */
char *valueless_tokens[] = {
	"fn", "for", "if", "while", "print", "return", "else", "<", "<-", "<=", ">",
	">=", "+", "++", "+=", "-", "--", "-=", "!=", "=", "{", "}", "(", ")", "*",
	"/", "%%",",", ";", "?", ":"
};

/* 
 * Function that decides if two strings are equal. Returns 1 (true) if they are
 * the same, or 0 (false) if they differ.
 */
int str_equal(char *str1, char *str2) {
	// If they have different lengths, we can say immidiately that they differ
	if(strlen(str1) != strlen(str2)) return 0;
	// If they are she same length, we must use strncmp to compare them. strncmp
	// returns 0 for identical strings, and other ints for different ones, so we
	// negate the result.
	else return !strncmp(str1, str2, strlen(str1));
}

/*
 * Does simple sequential search to determine if a given string is in the
 * valueless_tokens array
 */
int in_valueless_tokens(char *token) {
	int found = 0;
	int i = 0;
	while(!found && i < 31/*length of valueless_tokens*/) {
		if(str_equal(valueless_tokens[i], token)) found = 1;
		i++;
	}
	return found;
}

/* 
 * Takes a pointer to a char array and returns a pointer to a value-equal
 * string (but for the appended character) on the heap. Will cause a memory leak
 * if the string at the returned pointer is not freed when appropriate.
 * May cause an error if appending exceeds the size of the given array.
 */
char *str_append(char *str, char c) {
	// Allocate the appropriate amount of memory for the new string - this is
	// the size of the old string, inlucing the null terminator, plus one.
	char *new_str = malloc((strlen(str) + 2) * sizeof(char));
	
	// Copy the caracters from the old string into the new one, but not the null
	// terminator
	int i;
	for(i = 0; i < strlen(str); i++) new_str[i] = str[i];

	// Write the new char after the contents of the old string
	new_str[strlen(str)] = c;

	// Write the null terminator
	new_str[strlen(str) + 1] = '\0';

	// Free the old string from memory
	free(str);

	// Return the pointer to the new string
	return new_str;
}


/*
 * Retrieves the first token present in the given string. Also keeps count of
 * how many characters of the input string were processed. The resulting token
 * is returned in a tuple with the number of characters processed.
 */
TupleIntToken *get_next_token(char *input) {

	// Create an empty token
	Token *next_token = Token_init("", "");

	// Char array to build the token array into
	char *token_str = malloc(sizeof(char));
	*token_str = '\0';

	// The state number for the final state after FSM traversal
	int final_state = 0;

	// The state in the FSM that we are currently in, during traversal
	int current_state = 0;

	// We need to keep a record of the number of chars we processed, so that the
	// caller knows how far to advance their pointer over the array to the next
	// token in the input string
	int chars_processed = 0;

	// Trim the leading whitespace from the input (need to update 
	// chars_processed for each character we skip)
	while(isspace(*input)) {
		input += sizeof(char);
		chars_processed += sizeof(char);
	}

	while(current_state != -1 && char_indices(*input) != -1) {

		// Get the next char from the input
		char next_char = *input;

		// Determinethe state that processing the next char will lead to
		int next_state =
			transition_table[current_state][char_indices(next_char)];

		// If the next state produces a valid token...
		if(next_state != -1) {
			// Add the next char to the current string
			token_str = str_append(token_str, next_char);

			// Record the current state as final
			final_state = next_state;

			// Advance the input pointer to the next character
			input += sizeof(char);

			// We've processed a character so increment chars_processed
			chars_processed += sizeof(char);
		}

		// Update the current state variable
		current_state = next_state;
	}

	// If the token is not a 'valueless token' - i.e. a valid token that is
	// not an INT or ID - we store the string we've built up in the info
	// field
	if(!in_valueless_tokens(token_str)) Token_set_info(next_token, token_str);

	// If the text found was an error, process any remaining erroneous text
	if(str_equal(final_mapping[final_state], "ERROR")) {
		while(*input != '\0' && char_indices(*input) == -1) {
			next_token->info = str_append(next_token->info, *input);
			input += sizeof(char);
			chars_processed += sizeof(char);
		}
	}

	// Set the token type, return it in a TupleIntToken with chars_processed
	Token_set_type(next_token, final_mapping[final_state]);
	return TupleIntToken_init(next_token, chars_processed);
} 

/*
 * Lexical analyser function - takes a pointer to a string, which is the
 * program on which to perform lexical analysis, and returns a linked-list of
 * the tokens found in the lexical analysis
 */
TokenNode *lex(char *input) {
	// If the input is an empty string, return NULL
	if(*input == '\0') return NULL;

	// Make a copy of the original pointer to the input (we make a copy to
	// modify, we need to keep the original to free once we're done)
	char *cur_input = input;

	// Get the first token & the number of chars processed in retrieving it
	TupleIntToken *tuple = get_next_token(cur_input);

	// Construct a root TokenNode
	TokenNode *root_node = TokenNode_init_root(tuple->token);

	// Advance the input pointer past the token just processed
	cur_input += tuple->chars_processed;

	// We're finished with the tuple object now, so free it
	free(tuple);

	// Again we must check that there is input left before getting the next
	// token
	if(*cur_input == '\0') return root_node;

	// Get the next (token, chars_processed), Put the new token in a TokenNode,
	// Advance the input pointer
	tuple = get_next_token(cur_input);
	TokenNode *cur_node = TokenNode_init(root_node, tuple->token);
	cur_input += tuple->chars_processed;
	free(tuple);

	// Repeatedly get a new token and add it to the linked list in the above
	// way, until there is no input left
	while(*cur_input != '\0') {
		tuple = get_next_token(cur_input);
		cur_node = TokenNode_init(cur_node, tuple->token);
		cur_input += tuple->chars_processed;
		free(tuple);
	}

	// Return the root node
	return root_node;
}

int main() {
	// TupleIntToken *tuple = get_next_token("123=");
	// Token_print(tuple->token);

	char *s = strdup("hello, my name != 123 if for >= ##~!# lol !");
	TokenNode *node = lex(s);
	Token_print(node->token);
	while(node->child_node != NULL) {
		node = node->child_node;
		Token_print(node->token);
	}
	free(node);
	free(s);
	return 0;
}