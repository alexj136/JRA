#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"


/*
 * Struct used by FNDecl_generate_offsets(), Statement_generate_offsets(),
 * stmt_list_generate_offsets() and Expression_generate_offsets()
 * to record mappings between variable names and stack base offsets
 */
typedef struct {
	char *name;
	int offset;
} NameToOffsetMapping;

/*
 * Free function for NameToOffsetMappings
 */
void NameToOffsetMapping_free(NameToOffsetMapping *ntom) {
	free(ntom->name);
	free(ntom);
}

/*
 * Constructor for BooleanExpr Expressions
 */
Expression *BooleanExpr_init(Expression *lhs, token_type op, Expression *rhs) {
	
	// Ensure that the passed token type is a boolean operation
	if(op != EQUAL && op != NOT_EQUAL && op != LESS_THAN &&
		op != GREATER_THAN && op != LESS_OR_EQUAL && op != GREATER_OR_EQUAL) {

		printf("BooleanExpr_init given non-boolean token type\n");
		exit(EXIT_FAILURE);
	}

 	// First create the underlying BooleanExpr struct
 	BooleanExpr *booleanexpr_expr = safe_alloc(sizeof(BooleanExpr));
 	booleanexpr_expr->lhs = lhs;
 	booleanexpr_expr->op = op;
 	booleanexpr_expr->rhs = rhs;
 	
 	// Create a union object to point at the BooleanExpr struct
	u_expr *u_booleanexpr = safe_alloc(sizeof(u_expr));
	u_booleanexpr->blean = booleanexpr_expr;

 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_BooleanExpr;
 	the_exp->expr = u_booleanexpr;

 	// Return the wrapped Expression
 	return the_exp;
}

/*
 * Constructor for ArithmeticExpr Expressions
 */
Expression *ArithmeticExpr_init(
	Expression *lhs, token_type op, Expression *rhs) {
 	
	// Ensure that the passed token type is a arithmetic operation
	if(op != PLUS && op != MINUS && op != MULTIPLY &&
		op != DIVIDE && op != MODULO) {

		printf("BooleanExpr_init given non-boolean token type\n");
		exit(EXIT_FAILURE);
	}

 	// First create the underlying ArithmeticExpr struct
 	ArithmeticExpr *arithmeticexpr_expr = safe_alloc(sizeof(ArithmeticExpr));
 	arithmeticexpr_expr->lhs = lhs;
 	arithmeticexpr_expr->op = op;
 	arithmeticexpr_expr->rhs = rhs;

 	// Create a union object to point at the ArithmeticExpr struct
	u_expr *u_arithmeticexpr = safe_alloc(sizeof(u_expr));
	u_arithmeticexpr->arith = arithmeticexpr_expr;
 	
 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_ArithmeticExpr;
 	the_exp->expr = u_arithmeticexpr;

 	// Return the wrapped Expression
 	return the_exp;
}

/*
 * Constructor for Identifier Expressions
 */
Expression *Identifier_init(char *ident) {

	// First create the underlying Identifier struct
	Identifier *identifier_expr = safe_alloc(sizeof(Identifier));
	identifier_expr->name = ident;

	// -1 is used as the 'null' value for stack_offset. If the stack offset is
	// -1, it is assumed as not having been set.
	identifier_expr->stack_offset = -1;

	// Create a union object to point at the Identifier name string
	u_expr *u_identifier = safe_alloc(sizeof(u_expr));
	u_identifier->ident = identifier_expr;

	// Then create the Expression struct wrapper
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_Identifier;
	the_exp->expr = u_identifier;
	return the_exp;
}

/*
 * Constructor for IntegerLiteral Expressions
 */
Expression *IntegerLiteral_init(int intgr) {
	// Create a union object to point at the IntegerLiteral int value
	u_expr *u_expr_int = safe_alloc(sizeof(u_expr));
	u_expr_int->intgr = intgr;

	// Then create the Expression struct wrapper
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_IntegerLiteral;
	the_exp->expr = u_expr_int;
	return the_exp;
}

/*
 * Constructor for FNCall Expressions
 */
Expression *FNCall_init(char *name, LinkedList *args) {
	// First create the underlying FNCall struct
 	FNCall *fncall_expr = safe_alloc(sizeof(FNCall));
 	fncall_expr->name = name;
 	fncall_expr->args = args;
 	
	// Create a union object to point at the FNCall struct
	u_expr *u_fncall = safe_alloc(sizeof(u_expr));
	u_fncall->fncall = fncall_expr;

 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_FNCall;
 	the_exp->expr = u_fncall;	

 	// Return the wrapped Expression
 	return the_exp;
}

/*
 * Constructor for Ternary Expressions
 */
Expression *Ternary_init(Expression *bool_expr,
	Expression *true_expr, Expression *false_expr) {

	// Ensure that the boolean expression given is in fact a boolean expression
	// (cannot guarantee this at the struct level - everything must be an
	// expression for parsing purposes)
	assert(bool_expr->type == expr_BooleanExpr);

	// First create the underlying Ternary struct
	Ternary *ternary_expr = safe_alloc(sizeof(Ternary));
	ternary_expr->bool_expr = bool_expr;
	ternary_expr->true_expr = true_expr;
	ternary_expr->false_expr = false_expr;

	// Create a union object to point at the Ternary struct
	u_expr *u_ternary = safe_alloc(sizeof(u_expr));
	u_ternary->trnry = ternary_expr;

	// Then creare the Ternary Expression struct wrapper
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_Ternary;
	the_exp->expr = u_ternary;

	// Return the wrapped Expression
	return the_exp;
}

/*
 * Returns a string representation of the given expression
 */
char *Expression_str(Expression *expr) {
	char *expr_str;

	switch(expr->type) {
		case expr_BooleanExpr: {
			// Get the left hand side & right hand side strings
			char *str_lhs = Expression_str(expr->expr->blean->lhs);
			char *str_rhs = Expression_str(expr->expr->blean->rhs);

			// Concat everything together
			expr_str = str_concat(7,
				"(", str_lhs, " ",
				Token_str(expr->expr->blean->op),
				" ", str_rhs, ")");

			// Free the intermediates
			free(str_lhs);
			free(str_rhs);

			break;
		}

		case expr_ArithmeticExpr: {
			// Get the left hand side & right hand side strings
			char *str_lhs = Expression_str(expr->expr->arith->lhs);
			char *str_rhs = Expression_str(expr->expr->arith->rhs);

			// Concat everything together
			expr_str = str_concat(7,
				"(", str_lhs, " ",
				Token_str(expr->expr->arith->op),
				" ", str_rhs, ")");

			// Free the intermediates
			free(str_lhs);
			free(str_rhs);

			break;
		}

		case expr_Identifier: {
			// Copy the identifier name
			expr_str = safe_strdup(expr->expr->ident->name);
			break;
		}

		case expr_IntegerLiteral: {
			// Allocate a new string, long enough to hold 10 chars + null
			// terminator, as 10 is the number of digits of the maximum integer
			// in c
			char *str = safe_alloc(11 * sizeof(char));

			// Convert the int into a string. sprintf returns negative when it
			// fails, so assert that the call returns greater than zero
			assert(sprintf(str, "%d", expr->expr->intgr) > 0);

			// Copy the string into a new array and point expr_str at it
			expr_str = safe_strdup(str);

			// Free the old array
			free(str);

			break;
		}

		case expr_FNCall: {
			// Record the length of the args list so we don't have to evaluate
			// it repeatedly
			int num_args = LinkedList_length(expr->expr->fncall->args);

			// Concatenate the name and the '('
			char *str_builder = str_concat_2(expr->expr->fncall->name, "(");

			// If there are no arguments, just add the ')'
			if(num_args < 1) expr_str = str_concat_2(str_builder, ")");

			// If there are arguments...
			else {
				// Add the first argument
				char *first_arg = Expression_str((Expression *)
					LinkedList_get(expr->expr->fncall->args, 0));
				char *temp = str_concat_2(str_builder, first_arg);
				free(str_builder);
				free(first_arg);
				str_builder = str_concat_2(temp, first_arg);
				free(temp);

				// Then repeatedly add the ', ' and the next arg name, if any
				int i;
				for(i = 1; i < num_args; i++) {
					char *next_arg = Expression_str((Expression *)
						LinkedList_get(expr->expr->fncall->args, i));
					char *temp = str_concat(3, str_builder, ", ", next_arg);
					free(str_builder);
					free(next_arg);
					str_builder = temp;
					free(temp);
				}

				// Then add the ')'
				expr_str = str_concat_2(str_builder, ")");
			}
			
			free(str_builder);

			break;
		}

		case expr_Ternary: {
			// Stringify all the sub-expressions
			char *b_expr = Expression_str(expr->expr->trnry->bool_expr);
			char *t_expr = Expression_str(expr->expr->trnry->true_expr);
			char *f_expr = Expression_str(expr->expr->trnry->false_expr);

			// Concatenate all the sub expressions together with the '?' and ':'
			// characters, bracketed
			expr_str = str_concat(7,
				"(", b_expr, " ? ", t_expr, " : ", f_expr, ")");

			// Free the intermediate strings
			free(b_expr);
			free(t_expr);
			free(f_expr);

			break;
		}
	}
	return expr_str;
}

/*
 * Function to deep-compare two expressions
 */
bool Expression_equals(Expression *expr1, Expression *expr2) {

	// If the expressions are of different type, they cannot be equal
	if(expr1->type != expr2->type) return false;

	// If they are of the same type, (recursively) inspect them to see if their
	// properties are the same
	bool same = false;
	switch(expr1->type) {

		// With a boolean expression, we check the lhs and rhs are the same, and
		// that the op is the same
		case expr_BooleanExpr:	
			same = (
				Expression_equals(
					expr1->expr->blean->lhs,
					expr2->expr->blean->lhs) &&
				(expr1->expr->blean->op ==
				expr2->expr->blean->op) &&
				Expression_equals(
					expr1->expr->blean->rhs,
					expr2->expr->blean->rhs));
			break;

		// The procedure with an arithmetic expression is the same as with a
		// boolean expression
		case expr_ArithmeticExpr:
			same = (
				Expression_equals(
					expr1->expr->arith->lhs,
					expr2->expr->arith->lhs) &&
				(expr1->expr->arith->op ==
				expr2->expr->arith->op) &&
				Expression_equals(
					expr1->expr->arith->rhs,
					expr2->expr->arith->rhs));
			break;

		// With an identifier, we check if the ID names are the same. We do not
		// check if the stack offsets are the same, because they may not have
		// been assigned. Even if they have, it is still pointless to do so,
		// because if they are assigned correctly, they will have the same
		// offset for the same name and different offsets if they have a
		// different name.
		case expr_Identifier:
			same = str_equal(
				expr1->expr->ident->name,
				expr2->expr->ident->name);
			break;

		// With an integer literal, check that the literal values are the same
		case expr_IntegerLiteral:
			same = expr1->expr->intgr == expr2->expr->intgr;
			break;

		// With an FNCall, ...
		case expr_FNCall:

			// Check that the names are the same, and that they have the same
			// number of arguments
			if(!(str_equal(
					expr1->expr->fncall->name,
					expr2->expr->fncall->name) && (
				LinkedList_length(expr1->expr->fncall->args) ==
				LinkedList_length(expr2->expr->fncall->args))))

				// If not, we know they differ without checking the arguments
				// themselves
				same = false;

			// If they do match in name and arg count, we must compare the
			// expressions that comprise the arguments themselves, with a simple
			// while loop
			else {
				bool diff_not_found = true;
				int i = 0;
				while(diff_not_found &&
					i < LinkedList_length(expr1->expr->fncall->args)) {

					if(!Expression_equals(
						(Expression *)LinkedList_get(
							expr1->expr->fncall->args, i),
						(Expression *)LinkedList_get(
							expr2->expr->fncall->args, i)))

						diff_not_found = false;

					else i++;
				}
				same = diff_not_found;
			}
			break;

		// With a ternary, we simply check that the three expressions that
		// comprise them are equal
		case expr_Ternary:
			same = (
				Expression_equals(
					expr1->expr->trnry->bool_expr,
					expr2->expr->trnry->bool_expr) &&
				Expression_equals(
					expr1->expr->trnry->true_expr,
					expr2->expr->trnry->true_expr) &&
				Expression_equals(
					expr1->expr->trnry->false_expr,
					expr2->expr->trnry->false_expr));
			break;
	}
	return same;
}

/*
 * Walks over an expression and sets the stack offsets for every identifier
 * found in that expression
 */
static void Expression_generate_offsets(
	Expression *expr, LinkedList *mappings) {
	
	switch(expr->type) {

		case expr_BooleanExpr:
			Expression_generate_offsets(
				expr->expr->blean->lhs,
				mappings);
			Expression_generate_offsets(
				expr->expr->blean->rhs,
				mappings);
			break;

		case expr_ArithmeticExpr:
			Expression_generate_offsets(
				expr->expr->arith->lhs,
				mappings);
			Expression_generate_offsets(
				expr->expr->arith->rhs,
				mappings);
			break;
		
		case expr_Identifier: {
			// First we mush seach the mappings list to see if the mapping for
			// this variable already exists. If so, record the index of the
			// matching mapping.
			bool mapping_exists = false;
			int mapping_index = -1;
			LLIterator *mapping_iter = LLIterator_init(mappings);
			while((!LLIterator_ended(mapping_iter)) && (!mapping_exists)) {

				if(str_equal(expr->expr->ident->name, ((NameToOffsetMapping *)
						LLIterator_get_current(mapping_iter))->name)) {

					mapping_exists = true;
					mapping_index = LLIterator_current_index(mapping_iter);
				}

				LLIterator_advance(mapping_iter);
			}
			free(mapping_iter);

			// If the mapping already exists, set the stack offset of this
			// variable as the stack offset of the matching mapping.
			if(mapping_exists) {
				expr->expr->ident->stack_offset = ((NameToOffsetMapping *)
					LinkedList_get(mappings, mapping_index))->offset;
			}

			// Otherwise, determine the appropriate stack offset for this
			// variable, and set it for this Identifier object. Also add a
			// mapping for this variable name and determined stack offset to the
			// mappings list.
			else {
				// Calculate the offset - note that the offset is 4 times the
				// length of the list because we want to allocat 4 bytes worth
				// of stack space to each variable.
				int offset = LinkedList_length(mappings) * 4;

				// Set the offset for this Identifier object
				expr->expr->ident->stack_offset = offset;

				// Add a NameToOffsetMapping to the mapping list to represent
				// the mapping we just determined
				NameToOffsetMapping *ntom =
					(NameToOffsetMapping *)malloc(sizeof(NameToOffsetMapping));
				ntom->name = safe_strdup(expr->expr->ident->name);
				ntom->offset = offset;
				LinkedList_append(mappings, (void *)ntom);
			}

			// Debugging print statement
			// printf("name: %s, offset: %d\n", expr->expr->ident->name,
			// 	expr->expr->ident->stack_offset);

			break;
		}

		case expr_IntegerLiteral:
			/* Do nothing */
			break;

		case expr_FNCall:
			LLMAP_PARAM(
				expr->expr->fncall->args,
				Expression *,
				Expression_generate_offsets,
				mappings);
			break;

		case expr_Ternary:
			Expression_generate_offsets(
				expr->expr->trnry->bool_expr,
				mappings);
			Expression_generate_offsets(
				expr->expr->trnry->true_expr,
				mappings);
			Expression_generate_offsets(
				expr->expr->trnry->false_expr,
				mappings);
			break;
	}
}

/*
 * Destructor for all Expression objects
 */
void Expression_free(Expression *expr) {
	switch(expr->type) {

		case expr_BooleanExpr:
			// Free lhs, op, rhs
			Expression_free(expr->expr->blean->lhs);
			Expression_free(expr->expr->blean->rhs);

			// Free BooleanExpr object itself
			free(expr->expr->blean);

			// Free the union object
			free(expr->expr);
			break;

		case expr_ArithmeticExpr:
			// Free lhs, op, rhs
			Expression_free(expr->expr->arith->lhs);
			Expression_free(expr->expr->arith->rhs);

			// Free ArithmeticExpr object itself
			free(expr->expr->arith);

			// Free the union object
			free(expr->expr);
			break;

		case expr_Identifier:
			// Free Identifier name
			free(expr->expr->ident->name);

			// Free the Identifier struct
			free(expr->expr->ident);

			// Free the union object
			free(expr->expr);
			break;

		case expr_IntegerLiteral:
			// Free the union object
			free(expr->expr);
			break;

		case expr_FNCall:
			// Free the function name
			free(expr->expr->fncall->name);

			// Free the arguments
			LLMAP(expr->expr->fncall->args, Expression *, Expression_free);

			// Free the list that contained the arguments
			LinkedList_free(expr->expr->fncall->args);

			// Free the FNCall object itelf
			free(expr->expr->fncall);

			// Free the union object
			free(expr->expr);
			break;

		case expr_Ternary:
			// Free the expressions in the ternary
			Expression_free(expr->expr->trnry->bool_expr);
			Expression_free(expr->expr->trnry->true_expr);
			Expression_free(expr->expr->trnry->false_expr);

			// Free the Ternary object itself
			free(expr->expr->trnry);

			// Free the union object
			free(expr->expr);
			break;
	}
	// Free the Expression container
	free(expr);
}

/*
 * Constructor for For Statements
 */
Statement *For_init(Statement *assignment, Expression *bool_expr,
	Statement *incrementor, LinkedList *stmts) {

	For *for_stmt = safe_alloc(sizeof(For));
	for_stmt->assignment = assignment;
	for_stmt->bool_expr = bool_expr;
	for_stmt->incrementor = incrementor;
	for_stmt->stmts = stmts;

	u_stmt *u_for = safe_alloc(sizeof(u_stmt));
	u_for->_for = for_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_For;
	the_stmt->stmt = u_for;
	the_stmt->exec_count = 0;

	return the_stmt;
}

/*
 * Constructor for While Statements
 */
Statement *While_init(Expression *bool_expr, LinkedList *stmts) {

	While *while_stmt = safe_alloc(sizeof(While));
	while_stmt->bool_expr = bool_expr;
	while_stmt->stmts = stmts;

	u_stmt *u_while = safe_alloc(sizeof(u_stmt));
	u_while->_while = while_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_While;
	the_stmt->stmt = u_while;
	the_stmt->exec_count = 0;

	return the_stmt;
}

/*
 * Constructor for If Statements
 */
Statement *If_init(Expression *bool_expr,
	LinkedList *true_stmts, LinkedList *false_stmts) {

	If *if_stmt = safe_alloc(sizeof(If));
	if_stmt->bool_expr = bool_expr;
	if_stmt->true_stmts = true_stmts;
	if_stmt->false_stmts = false_stmts;

	u_stmt *u_if = safe_alloc(sizeof(u_stmt));
	u_if->_if = if_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_If;
	the_stmt->stmt = u_if;
	the_stmt->exec_count = 0;

	return the_stmt;
}

/*
 * Constructor for Print Statements
 */
Statement *Print_init(Expression *expr) {

	Print *print_stmt = safe_alloc(sizeof(Print));
	print_stmt->expr = expr;

	u_stmt *u_print = safe_alloc(sizeof(u_stmt));
	u_print->_print = print_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Print;
	the_stmt->stmt = u_print;
	the_stmt->exec_count = 0;

	return the_stmt;
}

/*
 * Constructor for Assignment Statements
 */
Statement *Assignment_init(char *name, Expression *expr) {

	Assignment *assignment_stmt = safe_alloc(sizeof(Assignment));
	assignment_stmt->ident = Identifier_init(name);
	assignment_stmt->expr = expr;

	u_stmt *u_assignment = safe_alloc(sizeof(u_stmt));
	u_assignment->_assignment = assignment_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Assignment;
	the_stmt->stmt = u_assignment;
	the_stmt->exec_count = 0;

	return the_stmt;
}

/*
 * Constructor for Return Statements
 */
Statement *Return_init(Expression *expr) {

	Return *return_stmt = safe_alloc(sizeof(Return));
	return_stmt->expr = expr;

	u_stmt *u_return = safe_alloc(sizeof(u_stmt));
	u_return->_return = return_stmt;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Return;
	the_stmt->stmt = u_return;
	the_stmt->exec_count = 0;
	return the_stmt;
}

/*
 * Function that determines whether or not two lists of statements are equal.
 * Will cause a runtime error if the passed lists contain any elements that are
 * not statements.
 */
bool stmt_list_equals(LinkedList *stmt_list_1, LinkedList *stmt_list_2) {
	// If the list lengths differ, they cannot be equal, so immediately return
	// false
	if(LinkedList_length(stmt_list_1) != LinkedList_length(stmt_list_2)) {
		return false;
	}

	// Otherwise, we can iterate over the lists without worrying that they might
	// have different lengths
	else {
		bool diff_not_found = true;
		int i = 0;
		while(diff_not_found && i < LinkedList_length(stmt_list_1)) {

			if(!Statement_equals(
				(Statement *)LinkedList_get(stmt_list_1, i),
				(Statement *)LinkedList_get(stmt_list_2, i))) {

				diff_not_found = false;
			}

			else {
				i++;
			}
		}
		return diff_not_found;
	}
}

/*
 * Function that determines whether or not two statements are equal
 */
bool Statement_equals(Statement *stmt1, Statement *stmt2) {

	// If the expressions are of different type, they cannot be equal
	if(stmt1->type != stmt2->type) return false;

	// If they are of the same type, (recursively) inspect them to see if their
	// properties are the same
	switch(stmt1->type) {

		case stmt_For:
			return Statement_equals(
					stmt1->stmt->_for->assignment,
					stmt2->stmt->_for->assignment) &&
				Expression_equals(
					stmt1->stmt->_for->bool_expr,
					stmt2->stmt->_for->bool_expr) &&
				Statement_equals(
					stmt1->stmt->_for->incrementor,
					stmt2->stmt->_for->incrementor) &&
				stmt_list_equals(
					stmt1->stmt->_for->stmts,
					stmt2->stmt->_for->stmts);

		case stmt_While:
			return Expression_equals(
					stmt1->stmt->_while->bool_expr,
					stmt2->stmt->_while->bool_expr) &&
				stmt_list_equals(
					stmt1->stmt->_while->stmts,
					stmt2->stmt->_while->stmts);

		case stmt_If:
			return Expression_equals(
					stmt1->stmt->_if->bool_expr,
					stmt2->stmt->_if->bool_expr) &&
				stmt_list_equals(
					stmt1->stmt->_if->true_stmts,
					stmt2->stmt->_if->true_stmts) &&
				stmt_list_equals(
					stmt1->stmt->_if->false_stmts,
					stmt2->stmt->_if->false_stmts);

		case stmt_Print:
			return Expression_equals(
				stmt1->stmt->_print->expr,
				stmt2->stmt->_print->expr);

		case stmt_Assignment:
			return Expression_equals(
					stmt1->stmt->_assignment->expr,
					stmt2->stmt->_assignment->expr) &&
				Expression_equals(
					stmt1->stmt->_assignment->ident,
					stmt2->stmt->_assignment->ident);

		case stmt_Return:
			return Expression_equals(
				stmt1->stmt->_return->expr,
				stmt2->stmt->_return->expr);
	}
	printf("Error: control reached end of Statement_equals() function\n");
	exit(EXIT_FAILURE);
	return (bool) NULL;
}

/*
 * Walks over the AST and sets the stack offsets for every identifier found in
 * the AST
 */
static void Statement_generate_offsets(Statement *stmt, LinkedList *mappings) {
	
	switch(stmt->type) {

		case stmt_For:
			Statement_generate_offsets(
				stmt->stmt->_for->assignment,
				mappings);
			Expression_generate_offsets(
				stmt->stmt->_for->bool_expr,
				mappings);
			Statement_generate_offsets(
				stmt->stmt->_for->incrementor,
				mappings);
			LLMAP_PARAM(
				stmt->stmt->_for->stmts,
				Statement *,
				Statement_generate_offsets,
				mappings);
			break;

		case stmt_While:
			Expression_generate_offsets(
				stmt->stmt->_while->bool_expr,
				mappings);
			LLMAP_PARAM(
				stmt->stmt->_while->stmts,
				Statement *,
				Statement_generate_offsets,
				mappings);
			break;
		
		case stmt_If:
			Expression_generate_offsets(
				stmt->stmt->_if->bool_expr,
				mappings);
			LLMAP_PARAM(
				stmt->stmt->_if->true_stmts,
				Statement *,
				Statement_generate_offsets,
				mappings);
			LLMAP_PARAM(
				stmt->stmt->_if->false_stmts,
				Statement *,
				Statement_generate_offsets,
				mappings);
			break;

		case stmt_Print:
			Expression_generate_offsets(
				stmt->stmt->_print->expr,
				mappings);
			break;

		case stmt_Assignment:
			Expression_generate_offsets(
				stmt->stmt->_assignment->expr,
				mappings);
			Expression_generate_offsets(
				stmt->stmt->_assignment->ident,
				mappings);
			break;

		case stmt_Return:
			Expression_generate_offsets(
				stmt->stmt->_return->expr,
				mappings);
			break;
	}
}

/*
 * Destructor for all Statement objects
 */
void Statement_free(Statement *stmt) {
	switch(stmt->type) {

		case stmt_For:
			// Free the Statements & Expression in the for loop declaration
			Statement_free(stmt->stmt->_for->assignment);
			Expression_free(stmt->stmt->_for->bool_expr);
			Statement_free(stmt->stmt->_for->incrementor);

			// Free the Statements in the statement list
			int i;
			for(i = 0; i < LinkedList_length(stmt->stmt->_for->stmts); i++)
				Statement_free((Statement *)LinkedList_get(
					stmt->stmt->_for->stmts, i));

			// Free the list itself
			LinkedList_free(stmt->stmt->_for->stmts);

			// Free the For object
			free(stmt->stmt->_for);

			// Free the union object
			free(stmt->stmt);
			break;

		case stmt_While:
			// Free the Expression in the while loop declaration
			Expression_free(stmt->stmt->_while->bool_expr);

			// Free the Statements in the statement list
			int j;
			for(j = 0; j < LinkedList_length(stmt->stmt->_while->stmts); j++)
				Statement_free((Statement *)LinkedList_get(
					stmt->stmt->_while->stmts, j));

			// Free the list itself
			LinkedList_free(stmt->stmt->_while->stmts);

			// Free the While object
			free(stmt->stmt->_while);

			// Free the union object
			free(stmt->stmt);

			break;

		case stmt_If:
		// Free the Expression in the if statement declaration
			Expression_free(stmt->stmt->_if->bool_expr);

			// Free the Statements in the statement lists
			int k;
			for(k = 0; k < LinkedList_length(stmt->stmt->_if->true_stmts); k++)
				Statement_free((Statement *)LinkedList_get(
					stmt->stmt->_if->true_stmts, k));

			for(k = 0; k < LinkedList_length(stmt->stmt->_if->false_stmts); k++)
				Statement_free((Statement *)LinkedList_get(
					stmt->stmt->_if->false_stmts, k));

			// Free the lists themselves
			LinkedList_free(stmt->stmt->_if->true_stmts);
			LinkedList_free(stmt->stmt->_if->false_stmts);

			// Free the If object
			free(stmt->stmt->_if);

			// Free the union object
			free(stmt->stmt);

			break;

		case stmt_Print:
			Expression_free(stmt->stmt->_print->expr);
			free(stmt->stmt->_print);

			// Free the union object
			free(stmt->stmt);
			break;

		case stmt_Assignment:
			Expression_free(stmt->stmt->_assignment->expr);
			Expression_free(stmt->stmt->_assignment->ident);
			free(stmt->stmt->_assignment);

			// Free the union object
			free(stmt->stmt);

			break;

		case stmt_Return:
			Expression_free(stmt->stmt->_return->expr);
			free(stmt->stmt->_return);

			// Free the union object
			free(stmt->stmt);

			break;
	}
	// Free the Statement container
	free(stmt);
}

/*
 * Constructor for FNDecl objects
 */
FNDecl *FNDecl_init(char *name, LinkedList *args, LinkedList *stmts) {
	FNDecl *func = safe_alloc(sizeof(FNDecl));
	func->name = name;
	func->args = args;
	func->stmts = stmts;

	// -1 for the variable count indicates that it has not been calculated yet.
	// It is assumed that if the variable count has been calculated, then each
	// identifier in the function has also been assigned its stack offset.
	func->variable_count = -1;
	return func;
}

/*
 * Function that determines whether or not two functions are equal
 */
bool FNDecl_equals(FNDecl *f1, FNDecl *f2) {
	// If they have different names, different statement lists or different
	// numbers of arguments, we can immediately return false
	if(!(
		str_equal(f1->name, f2->name) &&
		stmt_list_equals(f1->stmts, f2->stmts) && (
			LinkedList_length(f1->args) ==
			LinkedList_length(f2->args) ))) {

		return false;
	}

	// Otherwise we must check to see that all the argument identifiers are the
	// same
	else {
		bool diff_not_found = true;
		int i = 0;
		while(diff_not_found && i < LinkedList_length(f1->args)) {

			if(!Expression_equals(
				(Expression *)LinkedList_get(f1->args, i),
				(Expression *)LinkedList_get(f2->args, i))) {

				diff_not_found = false;
			}

			else {
				i++;
			}
		}
		return diff_not_found;
	}
}

/*
 * Calculates the number of variables that this function requires space for in
 * its stack frame when compiled, and records it in the variable_count field of
 * the function object. It also determines the stack offset for each identifier
 * and sets the stack_offset field appropriately for each identifier in the
 * function.
 */
void FNDecl_generate_offsets(FNDecl *func) {

	// Create a list to store NameToOffsetMappings
	LinkedList *mappings = LinkedList_init();

	// Assign to each function argument their stack offsets
	LLMAP_PARAM(
		func->args,
		Expression *,
		Expression_generate_offsets,
		mappings);

	// Assign the stack offsets to the other identifiers used in the function
	LLMAP_PARAM(
		func->stmts,
		Statement *,
		Statement_generate_offsets,
		mappings);

	// Set the variable count for the function as the number of mappings that
	// were created
	func->variable_count = LinkedList_length(mappings);

	// Free things
	LLMAP(mappings, NameToOffsetMapping *, NameToOffsetMapping_free);
	LinkedList_free(mappings);
}

/*
 * Destructor for FNDecl objects
 */
void FNDecl_free(FNDecl *func) {
	// Free the name string
	free(func->name);

	// Free the Identifiers in the argument list
	LLMAP(func->args, Expression *, Expression_free);

	// Free the list itself
	LinkedList_free(func->args);

	// Free the Statements in the statement list
	LLMAP(func->stmts, Statement *, Statement_free);

	// Free the list itself
	LinkedList_free(func->stmts);

	// Free the FNDecl object
	free(func);
}

/*
 * Constructor for Program objects
 */
Program *Program_init(LinkedList *function_list) {
	Program *prog = safe_alloc(sizeof(Program));
	prog->function_list = function_list;
	return prog;
}

/*
 * Retrieves a function declaration from a Program object
 */
FNDecl *Program_get_FNDecl(Program *prog, char *name) {

	// Sequential-search the program's functions for a function with the given
	// name
	bool found = false;
	int list_size = LinkedList_length(prog->function_list);
	int i = 0;
	
	while((!found) && i < list_size) {

		// If we find the variable, stop searching - i now has the index of the
		// variable
		if(str_equal(name,
			((FNDecl *)LinkedList_get(prog->function_list, i))->name)) {

			found = true;
		}

		else {
			i++;
		}
	}

	// If we never found the variable, an error has occurred, so print an error
	// message and exit
	if(!found) {
		printf("No function named '%s' in program\n", name);
		exit(EXIT_FAILURE);
	}

	// Otherwise return the variable's value
	else {
		return (FNDecl *)LinkedList_get(prog->function_list, i);
	}
}

/*
 * Checks if two ASTs are equal
 */
bool Program_equals(Program *p1, Program *p2) {
	// If the programs have different numbers of functions, we can immediately
	//return false
	if(LinkedList_length(p1->function_list) !=
		LinkedList_length(p2->function_list)) {

		return false;
	}

	// Otherwise we must check to see that all the functions are the equal
	else {
		bool diff_not_found = true;
		int i = 0;
		while(diff_not_found && i < LinkedList_length(p1->function_list)) {

			if(!FNDecl_equals(
				(FNDecl *)LinkedList_get(p1->function_list, i),
				(FNDecl *)LinkedList_get(p2->function_list, i))) {

				diff_not_found = false;
			}

			else {
				i++;
			}
		}
		return diff_not_found;
	}
}

/*
 * Generates stack offsets for the variables in a program
 */
void Program_generate_offsets(Program *prog) {
	LLMAP(prog->function_list, FNDecl *, FNDecl_generate_offsets);
}

/*
 * Destructor for Program objects
 */
void Program_free(Program *prog) {
	// Free the functions in the list
	LLMAP(prog->function_list, FNDecl *, FNDecl_free);

	// Free the list itself
	LinkedList_free(prog->function_list);

	// Free the Program object
	free(prog);
}