#include <stdio.h>
#include <malloc.h>
#include "minty_util.h"
#include "AST.h"

/*
 * Struct representing scope (the execution context for a particuar part of a
 * program)
 */
typedef struct {
	// The variables in the scope
	LinkedList *variables;

	// Has a return statement been executed?
	bool has_return;

	// The value that has been returned if a return has been executed
	int return_value;

} Scope;

/*
 * Initiate a brand new Scope
 */
Scope *Scope_init() {
	// Allocate space for the Scope object
	Scope *scope = (Scope *)safe_alloc(sizeof(Scope));
	
	// Allocate a new LinkedList for the variables in the scope
	scope->variables = LinkedList_init();

	// Indicate that the scope does not have a return value
	scope->has_return = false;

	return scope;
}

/*
 * Changes the state of a scope to represent entering a new nesting level in
 * source code. Increases the nesting depth of each variable by 1.
 */
void Scope_proliferate(Scope *scope) {
	// Add one to the nesting depth of the variables
	int i;
	for(i = 0; i < LinkedList_length(scope->variables); i++)
		(LinkedList_get(scope->variables, i))->nesting_depth++;
}

void Scope_receed(Scope *scope) {
	// For each variable, remove it if it was created in this scope (i.e. has
	// a nesting depth of zero - it is new and thus has become unreachable)
	// otherwise decrement its nesting depth
	int i;
	for(i = 0; i < LinkedList_length(scope->variables); i++) {
		if(((LinkedList_get(scope->variables, i))->nesting_depth) > 0) {
			// Remove the variable from the list and free all space it is using
		}
		else {
			// Decrement its nesting depth
			(LinkedList_get(scope->variables, i))->nesting_depth--;
		}
	}
}

/*
 * Struct representing a single variable
 */
typedef struct {
	// The Variable's name
	char *name;

	// The Variable's value
	int value;

	// The depth of nesting within which the variable is accessible
	int nesting_depth;

} Variable;

/*
 * Creates a new Variable object with the given name and value
 */
Variable *Variable_init(char *name, int value) {
	// Allocate space for the Variable object
	Variable *var = (Variable *)safe_alloc(sizeof(Variable));

	// Point the Variable's name pointer at the given name
	var->name = name;

	// Store the given value
	var->value = value;

	var->nesting_depth = 0;

	return var;
}

int interpret_expression(Expression *expr) {
	switch(expr->type) {
		case expr_BooleanExpr: {
			
		}
		case expr_ArithmeticExpr: {

		}
		case expr_Identifier: {
			
		}
		case expr_IntegerLiteral: {
			return expr->expr->intgr;
		}
		case expr_FNCall: {
			
		}
		case expr_Ternary: {

		}
	}
	return (int) NULL;
}

void interpret_statement(Statement *stmt) {

}