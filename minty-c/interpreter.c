#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "AST.h"
#include "interpreter.h"

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
 * Creates a new Variable object with the given name and value. Variable expects
 * ownership of the given name string, so copy it before passing it in if it is
 * needed elsewhere after the Variable is freed. This function should only be
 * called by functions on Scope objects.
 */
static Variable *Variable_init(char *name, int value) {
	// Allocate space for the Variable object
	Variable *var = (Variable *)safe_alloc(sizeof(Variable));

	// Point the Variable's name pointer at the given name
	var->name = name;

	// Store the given value
	var->value = value;

	// New Variables have a nesting depth of 0
	var->nesting_depth = 0;

	return var;
}

/*
 * Frees the memory allocated to a Variable struct and its name string
 */
static void Variable_free(Variable *var) {
	free(var->name);
	free(var);
}

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
 * Update the scope with the given variable. If a variable under the given name
 * already exists, replace its value with the given one. Otherwise, create a new
 * Variable object entry.
 */
void Scope_update(Scope *scope, char *name, int value) {

	// Sequential-search the current scope for the given variable
	bool found_existing = false;
	int list_size = LinkedList_length(scope->variables);
	int i = 0;
	
	while((!found_existing) && i < list_size) {

		// If we find the variable, stop searching and update the found variable
		// with the new value
		if(str_equal(name,
			((Variable *)LinkedList_get(scope->variables, i))->name)) {

			found_existing = true;
			((Variable *)LinkedList_get(scope->variables, i))->value = value;
		}

		i++;
	}

	// If we didn't find the variable, create it
	if(!found_existing) {
		LinkedList_append(scope->variables, (void *)Variable_init(name, value));
	}
}

/*
 * Retrieves the value of the given variable name in the given scope. If no such
 * variable is found, an error is raised.
 */
int Scope_get(Scope *scope, char *name) {

	// Sequential-search the current scope for the given variable
	bool found = false;
	int list_size = LinkedList_length(scope->variables);
	int i = 0;
	
	while((!found) && i < list_size) {

		// If we find the variable, stop searching - i now has the index of the
		// variable
		if(str_equal(name,
			((Variable *)LinkedList_get(scope->variables, i))->name)) {

			found = true;
		}

		else {
			i++;
		}
	}

	// If we never found the variable, an error has occurred, so print an error
	// message and exit
	if(!found) {
		printf("Variable: '%s' not in scope\n", name);
		exit(EXIT_FAILURE);
	}

	// Otherwise return the variable's value
	else {
		return ((Variable *)LinkedList_get(scope->variables, i))->value;
	}
}

/*
 * Changes the state of a scope to represent entering a new nesting level in
 * source code. Increases the nesting depth of each variable by 1.
 */
void Scope_proliferate(Scope *scope) {
	// Add one to the nesting depth of the variables
	int i;
	for(i = 0; i < LinkedList_length(scope->variables); i++) {
		((Variable *)LinkedList_get(scope->variables, i))->nesting_depth++;
	}
}

/*
 * 'Recedes' the current scope, that is, changes the scope to represent
 * a decrease in scope depth - variables that were added in the previous scope
 * are removed, and pre-existing variables depths are updated to reflect the
 * scope change
 */
void Scope_recede(Scope *scope) {
	// For each variable, remove it if it was created in this scope (i.e. has
	// a nesting depth of zero - it is new and thus has become unreachable)
	// otherwise decrement its nesting depth. We must iterate over it backwards
	// or removals will disturb indices for subsequent iterations.
	int i;
	for(i = LinkedList_length(scope->variables) - 1; i >= 0; i--) {

		if((((Variable *)
			LinkedList_get(scope->variables, i))->nesting_depth) < 1) {
			
			// Remove the variable from the list and free all space it is using
			Variable_free(LinkedList_get(scope->variables, i));
			LinkedList_remove(scope->variables, i);
		}

		else {
			// Decrement its nesting depth
			((Variable *)LinkedList_get(scope->variables, i))->nesting_depth--;
		}
	}
}

/*
 * Checks whether the given scope contains the given variable. Used mostly for
 * testing.
 */
bool Scope_has(Scope *scope, char *name) {
	// Have we found the variable?
	bool found = false;

	// Search through the list looking for a variable with the given name
	int i = 0;
	while(i < LinkedList_length(scope->variables) && !found) {
		if(str_equal(((Variable *)LinkedList_get(scope->variables, i))->name,
			name)) {

			// Set found to true if we find it. This will stop the iteration
			found = true;
		}
		i++;
	}

	// Return the result of the search
	return found;
}

/*
 * Frees a given scope object, including all variables and their names
 */
void Scope_free(Scope *scope) {
	// Free every contained variable
	int i;
	for(i = 0; i < LinkedList_length(scope->variables); i++) {
		Variable_free((Variable *)LinkedList_get(scope->variables, i));
	}

	// Free the variable list itself
	LinkedList_free(scope->variables);
	
	// Finally free the scope object itself
	free(scope);
}

int interpret_expression(Expression *expr, Scope *scope) {
	switch(expr->type) {
		case expr_BooleanExpr: {
			
		}
		case expr_ArithmeticExpr: {

		}
		case expr_Identifier: {
			// Retrieve the value associated with the identifier from the scope
			// and return it
			return Scope_get(scope, expr->expr->ident);
		}
		case expr_IntegerLiteral: {
			// Simply return the value of the IntegerLiteral
			return expr->expr->intgr;
		}
		case expr_FNCall: {
			// First we must evaluate the passed arguments, so create a list to
			// store them in
			LinkedList *evaluated_args = LinkedList_init();
			
			// Iterate over each given argument (an expression), evaluating them
			// and adding them to the list
			int i; 
			for(i = 0; i < LinkedList_length(expr->expr->fncall->args); i++) {
				
				LinkedList_append(evaluated_args, (void *)interpret_expression(
					(Expression *)LinkedList_get(expr->expr->fncall->args, i),
						scope));
			}

			// Return the result of interpreting the function with the given
			// arguments
			return interpret_function(expr->expr->fncall->name, evaluated_args);
		}
		case expr_Ternary: {

		}
	}
	expr->exec_count++;
	return (int) NULL;
}

void interpret_statement(Statement *stmt) {

}

int interpret_function(char *name, LinkedList *args) {
	// ASSERT LENGTH OF PASSED ARGS == THAT OF DECLARED ARGS
	return (int) NULL;
}

int interpret_program(Program *prog, LinkedList *args) {
	return (int) NULL;
}