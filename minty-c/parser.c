#include <stdio.h>
#include <stdlib.h>
#include "minty_util.h"
#include "lexer.h"
#include "AST.h"
#include "parser.h"

/*
 * Temporary store for the E' productions of the grammar that can be easily
 * built up into expression trees.
 */
typedef struct {
	char *op;
	Expression *expr;
	bool is_ternary;
	Expression *true_expr;
	Expression *false_expr;
} EPrime;

/*
 * Constructor of EPrime that represents a ternary expression
 */
EPrime *EPrime_tern_init(char *op, Expression *expr,
	Expression *true_expr, Expression *false_expr) {

	EPrime *this_eprime = safe_alloc(sizeof(EPrime));
	this_eprime->op = op;
	this_eprime->expr = expr;
	this_eprime->is_ternary = true;
	this_eprime->true_expr = true_expr;
	this_eprime->false_expr = false_expr;
	return this_eprime;
}

/*
 * Constructor of EPrime that does not represent a ternary expression
 */
EPrime *EPrime_init(char *op, Expression* expr) {

	EPrime *this_eprime = safe_alloc(sizeof(EPrime));
	this_eprime->op = op;
	this_eprime->expr = expr;
	this_eprime->is_ternary = false;
	this_eprime->true_expr = NULL;
	this_eprime->false_expr = NULL;
	return this_eprime;
}

/*
 * The following variables are used with the check_valid function defined below,
 * which is used to assert that the parser remains in a safe state, i.e. that it
 * will never return an AST if the input is invalid
 */
static char *FN[] = {"fn"};
static int FN_SIZE = 1;

static char *PRINT[] = {"print"};
static int PRINT_SIZE = 1;

static char *RETURN[] = {"return"};
static int RETURN_SIZE = 1;

static char *IF[] = {"if"};
static int IF_SIZE = 1;

static char *ELSE[] = {"else"};
static int ELSE_SIZE = 1;

static char *WHILE[] = {"while"};
static int WHILE_SIZE = 1;

static char *FOR[] = {"for"};
static int FOR_SIZE = 1;

static char *ID[] = {"ID"};
static int ID_SIZE = 1;

static char *INT[] = {"INT"};
static int INT_SIZE = 1;

static char *OPAREN[] = {"("};
static int OPAREN_SIZE = 1;

static char *CPAREN[] = {")"};
static int CPAREN_SIZE = 1;

static char *OBRACE[] = {"{"};
static int OBRACE_SIZE = 1;

static char *CBRACE[] = {"}"};
static int CBRACE_SIZE = 1;

static char *COMMA[] = {","};
static int COMMA_SIZE = 1;

static char *SEMICO[] = {";"};
static int SEMICO_SIZE = 1;

static char *PLUS[] = {"+"};
static int PLUS_SIZE = 1;

static char *PLPL[] = {"++"};
static int PLPL_SIZE = 1;

static char *PLEQ[] = {"+="};
static int PLEQ_SIZE = 1;

static char *MINUS[] = {"-"};
static int MINUS_SIZE = 1;

static char *MIMI[] = {"--"};
static int MIMI_SIZE = 1;

static char *MIEQ[] = {"-="};
static int MIEQ_SIZE = 1;

static char *ASSIGN[] = {"<-"};
static int ASSIGN_SIZE = 1;

static char *TIMES[] = {"*"};
static int TIMES_SIZE = 1;

static char *DIVIDE[] = {"/"};
static int DIVIDE_SIZE = 1;

static char *MODULO[] = {"%%"};
static int MODULO_SIZE = 1;

static char *EQUAL[] = {"="};
static int EQUAL_SIZE = 1;

static char *NEQUAL[] = {"!="};
static int NEQUAL_SIZE = 1;

static char *LESS[] = {"<"};
static int LESS_SIZE = 1;

static char *GRTR[] = {">"};
static int GRTR_SIZE = 1;

static char *LESSEQ[] = {"<="};
static int LESSEQ_SIZE = 1;

static char *GRTREQ[] = {">="};
static int GRTREQ_SIZE = 1;

static char *QMARK[] = {"?"};
static int QMARK_SIZE = 1;

static char *COLON[] = {":"};
static int COLON_SIZE = 1;

static char *ARGLIST[] = {"ID", ","};
static int ARGLIST_SIZE = 2;

static char *ARGDELIM[] = {",", ")"};
static int ARGDELIM_SIZE = 2;

static char *ARITH[] = {"+", "-", "*", "/", "%%"};
static int ARITH_SIZE = 5;

static char *BOOL[] = {"=", "!=", "<", ">", "<=", ">="};
static int BOOL_SIZE = 6;

static char *STMTSTART[] =
	{"print", "for", "return", "if", "while", "ID"};
static int STMTSTART_SIZE = 6;

/*
 * Checks that the 'found' string is equal to 'expected', and if not, quits the
 * program with a suitable error message
 */
void check_valid(char **expected, int expected_size, char *found) {

	// Have we found a match between found & expected?
	bool match_found = false;

	// Which index of expected to look at next
	int expected_index = 0;

	// Iterate over each value in expected until we reach the end or until no
	// match is found
	while((!match_found) && expected_index < expected_size) {
		if(str_equal(expected[expected_index], found)) match_found = true;
		expected_index++;
	}

	// If we didn't find a match, the input had a syntax error, so we print a
	// useful error message and exit
	if(!match_found){
		printf("Found token:\n");
		printf("    %s\n", found);
		printf("Expected:\n");

		int i;
		for(i = 0; i < expected_size; i++)
			printf("    %s\n", expected[i]);

		exit(EXIT_FAILURE);
	}
	// Otherwise, a match was found, so the found token was valid, and we can
	// return to the caller for them to carry on
	else return;
}

/*
 * This function, along with the parse_e_prime function, perform the job of
 * parsing expressions according to the context-free grammar specified in the
 * file CFG-parsable.txt, which can be found in the minty-spec directory of
 * the project repository. This function handles the following productions:
 * 
 * E' -> (epsilon)
 *     | + E
 *     | - E
 *     | * E
 *     | / E
 *     | % E
 *     | COMP E ? E : E
 * It also handles the 'COMP E' parts of the following productions:
 * ST -> if E COMP E { STMTS } else { STMTS } 
 *     | while E COMP E { STMTS }
 *     | for ID <- E, E COMP E, INCR { STMTS }
 * Accordingly, this function will look for an operator (either an arithmetic
 * operator, or a boolean comparison) and in the case of an arithmetic operator
 * it will then try to parse a single expression. If a boolean operation is
 * found (produced by the COMP nonterminal) then the function will attempt to
 * find a '?' indicating a ternary (although its absence is not an error).
 * It will return an EPrime object, which represents the production. EPrime
 * objects are not ASTNodes but are temporary stores for objects that
 * parse_expression will build into ASTNodes.
 */
EPrime *parse_e_prime(LinkedList *tokens) {

	// Peek at the next token (don't pop it yet - it might be needed elsewhere)
	Token *next_token = (Token *)LinkedList_get(tokens, 0);

	// If the next token is an arithmetic expression...
	if(str_equal(next_token->type, *PLUS) ||
		str_equal(next_token->type, *MINUS) ||
		str_equal(next_token->type, *TIMES) ||
		str_equal(next_token->type, *DIVIDE) ||
		str_equal(next_token->type, *MODULO)) {

		// We can pop the token in this case
		LinkedList_pop(tokens);

		// Return a non-ternary EPrime
		return EPrime_init(next_token->type, parse_expression(tokens));
	}

	// Or if the next token is an boolean expression...
	else if(str_equal(next_token->type, *EQUAL) ||
		str_equal(next_token->type, *NEQUAL) ||
		str_equal(next_token->type, *LESS) ||
		str_equal(next_token->type, *GRTR) ||
		str_equal(next_token->type, *LESSEQ) ||
		str_equal(next_token->type, *GRTREQ)) {

		// We can pop the token in this case
		LinkedList_pop(tokens);

		// Record the op type
		char *op = safe_strdup(next_token->type);

		// Parse the initial expression
		Expression *expr = parse_expression(tokens);

		// Peek at the next token
		next_token = (Token *)LinkedList_get(tokens, 0);

		// If it's a ternary...
		if(str_equal(next_token->type, *QMARK)) {

			// We can pop the token in this case
			LinkedList_pop(tokens);

			// Grab the expression the ternary evaluates to when true
			Expression *true_expr = parse_expression(tokens);

			// Grab the colon
			next_token = (Token *)LinkedList_get(tokens, 0);
			check_valid(COLON, COLON_SIZE, next_token->type);

			// Grab false expression
			Expression *false_expr = parse_expression(tokens);

			// Return a ternary EPrime with extra expressions
			return EPrime_tern_init(op, expr, true_expr, false_expr);
		}

		// If it's not a ternary, return an EPrime object without true_exp and
		// false_exp values
		else return EPrime_init(op, expr);
	}

	// Any other tokens indicates the epsilon production, so return None
	else return NULL;
}

/*
 * This function, along with the parse_e_prime function, perform the job of
 * parsing expressions according to the context-free grammar specified in the
 * file CFG-parsable.txt, which can be found in the minty-spec directory of
 * the project repository. This function handles the following productions:
 * E -> INT E'
 *     | ID E'
 *     | (E) E'
 *     | ID(EA) E'
 * Accordingly, this function will look for the part of the production that
 * comes before the E' part, and try to parse it, before making a call to
 * parse_e_prime to parse the E' part.
 */
Expression *parse_expression(LinkedList *tokens) {
	
	Token *next_token = (Token *)LinkedList_pop(tokens);

	Expression *left_part;

	// Function call case: T_ID T_( T_ID T_, T_ID ...
	if(str_equal(next_token->type, *ID) &&
		str_equal(((Token *)LinkedList_get(tokens, 0))->type, *OPAREN)) {
		
		// Record the name of the function
		char *fn_name = safe_strdup(next_token->info);

		// Get rid of the open bracket, we don't need it
		LinkedList_pop(tokens);

		// Create a list for the passed arguments (expressions) to go in
		LinkedList *arguments = LinkedList_init();

		// Peek at the top of the token_list, otherwise we might enter the
		// argument-collecting loop erroneously
		next_token = (Token *)LinkedList_get(tokens, 0);

		// Keep processing an arg, then a comma, until the close-bracket is
		// reached. The args are added to the list, the commas are thrown away
		while(!str_equal(next_token->type, *CBRACE)) {
	
			LinkedList_append(arguments, parse_expression(tokens));

			// Peek at the next token - should be a ',' if there are more args,
			// or a ')' if not
			next_token = (Token *)LinkedList_get(tokens, 0);
			check_valid(ARGDELIM, ARGDELIM_SIZE, next_token->type);

			// If the next token is a ',', we can pop it, and look for the next
			// argument. It it's a '(' we cannot pop it here - it must be done
			// outside the loop in case no arguments are found at all and the
			// loop is never entered
			if(str_equal(next_token->type, *COMMA)) LinkedList_pop(tokens);
		}

		// Clear the ')' from the token list
		LinkedList_pop(tokens);

		left_part = FNCall_init(fn_name, arguments);
	}

	// Identifier case
	else if(str_equal(next_token->type, *ID))
		left_part = Identifier_init(safe_strdup(next_token->info));

	// Integer literal case
	else if(str_equal(next_token->type, *INT))
		left_part = IntegerLiteral_init(
			(int)strtol(next_token->info, (char **)NULL, 10));

	// Brackets case
	else if (str_equal(next_token->type, *OBRACE)) {
		left_part = parse_expression(tokens);

		// Process the close-bracket
		next_token = (Token *)LinkedList_get(tokens, 0);
		check_valid(CPAREN, CPAREN_SIZE, next_token->type);
	}

	else {
		printf("Could not parse expression production, %s not expected",
			next_token->type);
		exit(EXIT_FAILURE);
	}

	// Try to parse the E' production
	EPrime *e_prime = parse_e_prime(tokens);
	
	// If parse_e_prime returned NULL, then we already reached the end of the
	// expression, so just return the 'left part'
	if(!e_prime) return left_part;

	// If parse_e_prime returned a non-ternary EPrime with an arithmetic
	// operation, create an ArithmeticExpr and return it
	else if((str_equal(e_prime->op, *PLUS) ||
		str_equal(e_prime->op, *MINUS) ||
		str_equal(e_prime->op, *TIMES) ||
		str_equal(e_prime->op, *DIVIDE) ||
		str_equal(e_prime->op, *MODULO)) &&
		!e_prime->is_ternary)
		return ArithmeticExpr_init(left_part, e_prime->op, e_prime->expr);

	// If parse_e_prime returned a ternary EPrime with an boolean operation,
	// create an Ternary and return it
	else if((str_equal(e_prime->op, *EQUAL) ||
		str_equal(e_prime->op, *NEQUAL) ||
		str_equal(e_prime->op, *LESS) ||
		str_equal(e_prime->op, *GRTR) ||
		str_equal(e_prime->op, *LESSEQ) ||
		str_equal(e_prime->op, *GRTREQ)) &&
		e_prime->is_ternary)
		return Ternary_init(BooleanExpr_init(left_part, e_prime->op, e_prime->expr),
			e_prime->true_expr, e_prime->false_expr);

	// Else if parse_e_prime returned a non-ternary EPrime with an boolean
	// operation, create a BooleanExpr and return it
	else if(str_equal(e_prime->op, *EQUAL) ||
		str_equal(e_prime->op, *NEQUAL) ||
		str_equal(e_prime->op, *LESS) ||
		str_equal(e_prime->op, *GRTR) ||
		str_equal(e_prime->op, *LESSEQ) ||
		str_equal(e_prime->op, *GRTREQ))
		return BooleanExpr_init(left_part, e_prime->op, e_prime->expr);

	else {
		printf("Could not parse expression, invalid syntax found");
		exit(EXIT_FAILURE);
	}
}

/*
 * This function is responsible for parsing a single statement. There are 6
 * types of statement, each of which is handled differently, but in each case
 * a Statement object is returned.
 */
Statement *parse_statement(LinkedList *tokens) {

	// Retrieve the next token, and check that it's valid, i.e. if it's in the
	// list: ['print', 'for', 'return', 'if', 'while', 'ID']
	Token *next_token = (Token *)LinkedList_pop(tokens);
	check_valid(STMTSTART, STMTSTART_SIZE, next_token->type);

	// If the token is a print, parse the corresponding expression and return it
	// in a Print Statement object
	if(str_equal(next_token->type, *PRINT)) {
	
		// Grab the expression from the token list, create an Print Statement
		// object with it
		Statement *print_statement = Print_init(parse_expression(tokens));

		// Return the generated Statement object
		return print_statement;
	}

	// A return token is handled the same way as a print token, except the
	// object returned is a Return Statement object as opposed to a Print
	else if(str_equal(next_token->type, *RETURN)) {

		// Grab the expression from the token list, create an Return Statement
		// object with it
		Statement *return_statement =  Return_init(parse_expression(tokens));

		// Return the generated Statement object
		return return_statement;
	}

	// An if-statement requires a boolean expression of the form 'E COMP E' to
	// be parsed, followed by a statement block, which a call to 
	// parse_statement_block() handles, after which an 'else' token should be
	// parsed, and finally a futher statement block.
	else if(str_equal(next_token->type, *IF)) {
		
		// Parse the BoolExpression, assert that it is a BoolExpression
		Expression *bool_expr = parse_expression(tokens);
		if(bool_expr->type != expr_BooleanExpr) {
			printf("Parse error on if-statement: boolean expression expected, \
				other expression type found\n");
			exit(EXIT_FAILURE);
		}

		// Create a list for the if's statements
		LinkedList *true_stmts = parse_statement_block(tokens);

		// Retrieve the next token, check it's an 'else'
		next_token = (Token *)LinkedList_pop(tokens);
		check_valid(ELSE, ELSE_SIZE, next_token->type);

		// Create a list for the else's statements
		LinkedList *false_stmts = parse_statement_block(tokens);

		// Return an If Statement object representing the parsed if-statement
		return If_init(bool_expr, true_stmts, false_stmts);
	}

	// While loops are simpler than if-statements to parse: just a boolean and
	// a single statement block to handle.
	else if(str_equal(next_token->type, *WHILE)) {
			
		// Parse the BoolExpression, assert that it is a BoolExpression
		Expression *bool_expr = parse_expression(tokens);
		if(bool_expr->type != expr_BooleanExpr) {
			printf("Parse error on while-loop: boolean expression expected, \
				other expression type found\n");
			exit(EXIT_FAILURE);
		}

		// Create a list for the while-loop's statements
		LinkedList *statements = parse_statement_block(tokens);

		// Return a While Statement object that corresponds to the parsed
		// while-loop
		return While_init(bool_expr, statements);
	}

	// For loops require parsing an assignment, a boolean expression, and a
	// further assignment statement, separated by commas, and finally the
	// statement block.
	else if(str_equal(next_token->type, *FOR)) {

		// Parse the assignment statement of the for loop, checking that it is
		// in fact an assignment (parse_statement can return other stuff)
		Statement *assignment = parse_statement(tokens);
		if(assignment->type != stmt_Assignment) {
			printf("Parse error on for-loop: assignment statement expected, \
				other statement type found\n");
			exit(EXIT_FAILURE);
		}

		// Pop the comma, making sure that it is a comma
		next_token = (Token *)LinkedList_pop(tokens);
		check_valid(COMMA, COMMA_SIZE, next_token->type);

		// Parse the BooleanExpr, assert that it is a BooleanExpr
		Expression *bool_expr = parse_expression(tokens);
		if(bool_expr->type != expr_BooleanExpr) {
			printf("Parse error on for-loop: boolean expression expected, \
				other expression type found\n");
			exit(EXIT_FAILURE);
		}

		// Pop the second comma, making sure that it is a comma
		next_token = (Token *)LinkedList_pop(tokens);
		check_valid(COMMA, COMMA_SIZE, next_token->type);

		// Parse the 'incrementor' - an Assignment - from the token list
		Statement *incrementor = parse_statement(tokens);
		if(incrementor->type != stmt_Assignment) {
			printf("Parse error on for-loop: two assignment statements \
				expected, the first was found but unable to parse the \
				second\n");
			exit(EXIT_FAILURE);
		}

		// Create a list for the for-loop's statements
		LinkedList *statements = parse_statement_block(tokens);

		// Return a For Statement object representing the parsed for-loop
		return For_init(assignment, bool_expr, incrementor, statements);
	}

	// If the token is an identifier, this indicates an assignment statement
	// - something with a ++, --, +=, -=, or an explicit ID <- E.
	else if(str_equal(next_token->type, *ID)) {
	
		// Record the name of the ID
		char *assignee = safe_strdup(next_token->info);

		// Grab the next token - will be one of the 5 assignment operators
		// if the syntax is valid
		next_token = (Token *)LinkedList_pop(tokens);

		// Switch on the assignment operator type so as to build the
		// appropriate syntax tree
		if(str_equal(next_token->type, *PLPL)) return // ++ operator
			Assignment_init(
				assignee,
				ArithmeticExpr_init(
					Identifier_init(assignee),
					*PLUS,
					IntegerLiteral_init(1)));

		else if(str_equal(next_token->type, *MIMI)) return // -- operator
			Assignment_init(
				assignee,
				ArithmeticExpr_init(
					Identifier_init(assignee),
					*MINUS,
					IntegerLiteral_init(1)));

		else if(str_equal(next_token->type, *PLEQ)) return // += operator
			Assignment_init(
				assignee,
				ArithmeticExpr_init(
					Identifier_init(assignee),
					*PLUS,
					parse_expression(tokens)));

		else if(str_equal(next_token->type, *MIEQ)) return // -= operator
			Assignment_init(
				assignee,
				ArithmeticExpr_init(
					Identifier_init(assignee),
					*MINUS,
					parse_expression(tokens)));

		else if(str_equal(next_token->type, *ASSIGN)) return // <- operator
			Assignment_init(
				assignee,
				parse_expression(tokens));

		else {
			printf("'++', '--', '+=', '-=' or '<-' operator expected after \
				identifier, none found\n");
			exit(EXIT_FAILURE);
		}
	}

	else {
		printf("'ID', 'print', 'for', 'if', 'while' or 'return' token expected \
			within statement block, none found\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Parses a block of statements encased in curly brackets. Returns a list that
 * contains a Statement object for each statement in the block, by calling
 * parse_statement for each of them.
 */
LinkedList *parse_statement_block(LinkedList *tokens) {

	// Create a list for the block's statements
	LinkedList *statements = LinkedList_init();

	// Retrieve the opening curly-bracket
	Token *next_token = (Token *)LinkedList_pop(tokens);
	check_valid(OBRACE, OBRACE_SIZE, next_token->type);

	// Peek at the next token - otherwise we will enter the loop on an empty
	// statement block which will cause an exception
	next_token = (Token *)LinkedList_get(tokens, 0);

	// Repeatedly parse statements until a '}' is seen (note that '}'s within
	// the block that do not end this loop will be handled by a recursive call
	// to parse_statement_block, and will not cause the iteration to end early)
	while(!str_equal(next_token->type, *CBRACE)) {
	
		// See if we need to process a semicolon after the statement
		bool expecting_semicolon =
			str_equal(next_token->type, *PRINT) ||
			str_equal(next_token->type, *RETURN) ||
			str_equal(next_token->type, *ID);

		LinkedList_append(statements, (void *)parse_statement(tokens));

		// Parse the semicolon if necessary
		if(expecting_semicolon) {
			next_token = (Token *)LinkedList_pop(tokens);
			check_valid(SEMICO, SEMICO_SIZE, next_token->type);
		}

		// Peek at the next token for purposes of loop-control
		next_token = LinkedList_get(tokens, 0);
	}
	// Eat the final '}'. No need to check that it is a '}' because we would
	// never have left the loop if it wasn't
	LinkedList_pop(tokens);

	return statements;
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
	check_valid(FN, FN_SIZE, next_token->type);

	// Retrieve the next token, check that it's an ID
	next_token = (Token *)LinkedList_pop(tokens);
	check_valid(ID, ID_SIZE, next_token->type);

	// We know it's an ID now, so store its name
	char *function_name = safe_strdup(next_token->info);

	// Retrieve the next token, check it's an open-bracket
	next_token = (Token *)LinkedList_pop(tokens);
	check_valid(OPAREN, OPAREN_SIZE, next_token->type);

	// Create a list for the argument names to be stored in
	LinkedList *arg_names = LinkedList_init();

	// Grab the first token in the argument list
	next_token = (Token *)LinkedList_pop(tokens);

	// Keep looking for argument names until we hit a ')' or an exit occurs due
	// to a call to check_valid
	while(!str_equal(next_token->type, *CPAREN)) {
	
		// The only valid tokens at this point are IDs and commas to delimit
		// them
		check_valid(ARGLIST, ARGLIST_SIZE, next_token->type);

		// Add the ID to the list of argument names
		if(str_equal(next_token->type, *ID))
			LinkedList_append(arg_names, safe_strdup(next_token->info));

		// Retrieve the next token
		next_token = (Token *)LinkedList_pop(tokens);
	}
	// Create a list for the function's statements
	LinkedList *statements = parse_statement_block(tokens);

	// Return the function object
	return FNDecl_init(function_name, arg_names, statements);
}

/*
 * This function is the entry point for the parser. It is responsible for making
 * calls to parse_function for each function in the program, putting each
 * function into a list, wrapping that list into a Program object and returning
 * that Program object.
 */
Program *parse_program(LinkedList *tokens) {
	LinkedList *function_list = LinkedList_init();

	while(LinkedList_length(tokens) > 0)
		LinkedList_append(function_list, (void *)parse_function(tokens));

	return Program_init(function_list);
}