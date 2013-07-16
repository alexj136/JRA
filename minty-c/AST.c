#include <stdio.h>
#include "AST.h"
#include "minty_util.h"

/*
 * Constructor for For Statements
 */
Statement *For_init(Statement *assignment,
	Expression *bool_expr, Statement *incrementor) {

	For *_for = safe_alloc(sizeof(For));
	_for->assignment = assignment;
	_for->bool_expr = bool_expr;
	_for->incrementor = incrementor;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_For;
	the_stmt->stmt->_for = _for;

	return the_stmt;
}

/*
 * Constructor for While Statements
 */
Statement *While_init(Expression *bool_expr, LinkedList *stmts) {

	While *_while = safe_alloc(sizeof(While));
	_while->bool_expr = bool_expr;
	_while->stmts = stmts;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_While;
	the_stmt->stmt->_while = _while;

	return the_stmt;
}

/*
 * Constructor for If Statements
 */
Statement *If_init(Expression *bool_expr,
	LinkedList *true_stmts, LinkedList *false_stmts) {

	If *_if = safe_alloc(sizeof(If));
	_if->bool_expr = bool_expr;
	_if->true_stmts = true_stmts;
	_if->false_stmts = false_stmts;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_If;
	the_stmt->stmt->_if = _if;

	return the_stmt;
}

/*
 * Constructor for Print Statements
 */
Statement *Print_init(Expression *expr) {

	Print *_print = safe_alloc(sizeof(Print));
	_print->expr = expr;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Print;
	the_stmt->stmt->_print = _print;

	return the_stmt;
}

/*
 * Constructor for Assignment Statements
 */
Statement *Assignment_init(char *name, Expression *expr) {

	Assignment *_assignment = safe_alloc(sizeof(Assignment));
	_assignment->name = name;
	_assignment->expr = expr;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Assignment;
	the_stmt->stmt->_assignment = _assignment;

	return the_stmt;
}

/*
 * Constructor for Return Statements
 */
Statement *Return_init(Expression *expr) {

	Return *_return = safe_alloc(sizeof(Return));
	_return->expr = expr;

	Statement *the_stmt = safe_alloc(sizeof(Statement));
	the_stmt->type = stmt_Return;
	the_stmt->stmt->_return = _return;

	return the_stmt;
}

/*
 * Destructor for all Statement objects
 */
void Statement_free(Statement *stmt) {
	switch(stmt->type) {

		case stmt_For:
			// Free the Statements & Expression in the for loop declaration
			Statement_free(stmt->_for->assignment);
			Expression_free(stmt->_for->bool_expr);
			Statement_free(stmt->_for->incrementor);

			// Free the Statements in the statement list
			int i;
			for(i = 0; i < LinkedList_length(stmt->_for->stmts); i++)
				Statement_free(LinkedList_get(stmt->_for->stmts, i));

			// Free the list itself
			LinkedList_free(stmt->_for->stmts);

			// Free the For object
			free(stmt->_for);
			break;

		case stmt_While:
			// Free the Expression in the while loop declaration
			Expression_free(stmt->_while->bool_expr);

			// Free the Statements in the statement list
			int i;
			for(i = 0; i < LinkedList_length(stmt->_while->stmts); i++)
				Statement_free(LinkedList_get(stmt->_while->stmts, i));

			// Free the list itself
			LinkedList_free(stmt->_while->stmts);

			// Free the While object
			free(stmt->_while);
			break;

		case stmt_If:
		// Free the Expression in the if statement declaration
			Expression_free(stmt->_for->bool_expr);

			// Free the Statements in the statement lists
			int i;
			for(i = 0; i < LinkedList_length(stmt->_if->true_stmts); i++)
				Statement_free(LinkedList_get(stmt->_if->true_stmts, i));

			for(i = 0; i < LinkedList_length(stmt->_if->false_stmts); i++)
				Statement_free(LinkedList_get(stmt->_if->false_stmts, i));

			// Free the list itself
			LinkedList_free(stmt->_if->stmts);

			// Free the If object
			free(stmt->_if);
			break;

		case stmt_Print:
			Expression_free(stmt->_print->expr);
			free(stmt->_print);
			break;

		case stmt_Assignment:
			Expression_free(stmt->_assignment->expr);
			free(stmt->_assignment->name)
			free(stmt->_assignment);
			break;

		case stmt_Return:
			Expression_free(stmt->_return->expr);
			free(stmt->_return);
			break;
	}
	// Free the Statement container
	free(stmt);
}

/*
 * Constructor for BooleanExpr Expressions
 */
Expression *BooleanExpr_init(Expression *lhs, char *op, Expression *rhs) {
 	// First create the underlying BooleanExpr struct
 	BooleanExpr *_bool = safe_alloc(sizeof(BooleanExpr));
 	_bool->lhs = lhs;
 	_bool->op = op;
 	_bool->rhs = rhs;
 	
 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_BooleanExpr;
 	the_exp->expr->_bool = _bool;

 	// Return the wrapped Expression
 	return the_exp;
}

 /*
 * Constructor for ArithmeticExpr Expressions
 */
Expression *ArithmeticExpr_init(Expression *lhs, char *op, Expression *rhs) {
 	// First create the underlying ArithmeticExpr struct
 	ArithmeticExpr *_arith = safe_alloc(sizeof(ArithmeticExpr));
 	_arith->lhs = lhs;
 	_arith->op = op;
 	_arith->rhs = rhs;
 	
 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_ArithmeticExpr;
 	the_exp->expr->_arith = _arith;

 	// Return the wrapped Expression
 	return the_exp;
}

/*
 * Constructor for Identifier Expressions
 */
Expression *Identifier_init(char *_ident) {
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_Identifier;
	the_exp->expr->_ident = _ident;
	return the_exp;
}

/*
 * Constructor for IntegerLiteral Expressions
 */
Expression *IntegerLiteral_init(int _int) {
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_IntegerLiteral;
	the_exp->expr->_int = _int;
	return the_exp;
}

/*
 * Constructor for FNCall Expressions
 */
Expression *FNCall_init(char *name, LinkedList *args) {
	// First create the underlying FNCall struct
 	FNCall *_call = safe_alloc(sizeof(FNCall));
 	_call->name = name;
 	_call->args = args;
 	
 	// Then create the Expression struct wrapper
 	Expression *the_exp = safe_alloc(sizeof(Expression));
 	the_exp->type = expr_FNCall;
 	the_exp->expr->_call = _call;

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
	assert(boolean_expr->type == expr_BooleanExpr);

	// First create the underlying Ternary struct
	Ternary *_tern = safe_alloc(sizeof(Ternary));
	_tern->bool_expr = bool_expr;
	_tern->true_expr = true_expr;
	_tern->false_expr = false_expr;

	// Then creare the Expression struct wrapper
	Expression *the_exp = safe_alloc(sizeof(Expression));
	the_exp->type = expr_Ternary;
	the_exp->expr->_tern = _tern;

	// Return the wrapped Expression
	return the_exp;
}

/*
 * Destructor for all Expression objects
 */
void Expression_free(Expression *expr) {
	switch(expr->type) {

		case expr_BooleanExpr:
			// Free lhs, op, rhs
			Expression_free(expr->_bool->lhs);
			free(expr->_bool->op);
			Expression_free(expr->_bool->rhs);

			// Free BooleanExpr object itself
			free(expr->_bool);
			break;

		case expr_ArithmeticExpr:
			// Free lhs, op, rhs
			Expression_free(expr->_arith->lhs);
			free(expr->_arith->op);
			Expression_free(expr->_arith->rhs);

			// Free ArithmeticExpr object itself
			free(expr->_arith);
			break;

		case expr_Identifier:
			// Free Identifier name
			free(expr->_ident);
			break;

		case expr_IntegerLiteral:
			// Nothing to free
			break;

		case expr_FNCall:
			// Free the function name
			free(expr->_call->name);

			// Free the arguments
			int i;
			for(i = 0; i < LinkedList_length(expr->_call->args); i++)
				Expression_free(LinkedList_get(expr->_call->args, i));

			// Free the list that contained the arguments
			LinkedList_free(expr->_call->args);

			// Free the FNCall object itelf
			free(expr->_call);
			break;

		case expr_Ternary:
			// Free the expressions in the ternary
			Expression_free(expr->_tern->bool_expr);
			Expression_free(expr->_tern->true_expr);
			Expression_free(expr->_tern->false_expr);

			// Free the Ternary object itself
			free(expr->_tern);
			break;
	}
	// Free the Expression container
	free(expr);
}