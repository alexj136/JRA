#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
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
 * Initiate a brand new Scope - creates variables with the given names and
 * values
 */
Scope *Scope_init(LinkedList *arg_names, LinkedList *args_given) {
	// Allocate space for the Scope object
	Scope *scope = (Scope *)safe_alloc(sizeof(Scope));
	
	// Allocate a new LinkedList for the variables in the scope
	scope->variables = LinkedList_init();

	// Indicate that the scope does not have a return value
	scope->has_return = false;

	// Initialise the arguments to the function
	int i;
	for(i = 0; i < LinkedList_length(arg_names); i++) {
		Scope_update(scope,
			(char *)LinkedList_get(arg_names, i),
			(int)LinkedList_get(args_given, i));
	}

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

/*
 * Evaluates and returns the Expression expr in the context of the given scope
 * and functions available in the given program.
 */
int interpret_expression(Expression *expr, Scope *scope, Program *prog) {

	// Increment the counter for number of executions
	expr->exec_count++;

	// Interpret the expression in the appropriate way
	switch(expr->type) {
	
		case expr_BooleanExpr: {
			// Interpret the left and right hand side
			int lhs = interpret_expression(expr->expr->blean->lhs, scope, prog);
			int rhs = interpret_expression(expr->expr->blean->rhs, scope, prog);

			// Return the results with the appropriate operation applied
			if(EQUAL == expr->expr->blean->op) return lhs = rhs;
			if(NOT_EQUAL == expr->expr->blean->op) return lhs != rhs;
			if(LESS_THAN == expr->expr->blean->op) return lhs < rhs;
			if(GREATER_THAN == expr->expr->blean->op) return lhs > rhs;
			if(LESS_OR_EQUAL == expr->expr->blean->op) return lhs <= rhs;
			if(GREATER_OR_EQUAL == expr->expr->blean->op) return lhs >= rhs;
			else {
				printf("Invalid boolean operation\n");
				exit(EXIT_FAILURE);
			}
		}
		case expr_ArithmeticExpr: {
			// Interpret the left and right hand side
			int lhs = interpret_expression(expr->expr->arith->lhs, scope, prog);
			int rhs = interpret_expression(expr->expr->arith->rhs, scope, prog);

			// Return the results with the appropriate operation applied
			if(PLUS == expr->expr->arith->op) return lhs + rhs;
			if(MINUS == expr->expr->arith->op) return lhs - rhs;
			if(MULTIPLY == expr->expr->arith->op) return lhs * rhs;
			if(DIVIDE == expr->expr->arith->op) return lhs / rhs;
			if(MODULO == expr->expr->arith->op) return lhs % rhs;
			else {
				printf("Invalid arithmetic operation\n");
				exit(EXIT_FAILURE);
			}
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
						scope, prog));
			}

			// Return the result of interpreting the function with the given
			// arguments
			return interpret_function(Program_get_FNDecl(prog,
				expr->expr->fncall->name), evaluated_args, prog);
		}
		case expr_Ternary: {
			// If the boolean expression evaluates to true...
			if(interpret_expression(
				expr->expr->trnry->bool_expr, scope, prog)) {

				// Return the result of evaluating the true expression
				return interpret_expression(
					expr->expr->trnry->true_expr, scope, prog);
			}
			else {
				// Else return the result of evaluating the false expression
				return interpret_expression(
					expr->expr->trnry->false_expr, scope, prog);
			}
		}
	}
	// If we exit the switch, an error has occurred because the individual cases
	// are supposed to return something, so raise an error
	printf("Cannot interpret expression: invalid expression type\n");
	exit(EXIT_FAILURE);
	return (int) NULL;
}

/*
 * Evaluates the Statement stmt in the context of the given scope and functions
 * available in the given program.
 */
void interpret_statement(Statement *stmt, Scope *scope, Program *prog) {
	
	// Increment the counter for number of executions
	stmt->exec_count++;

	// Interpret the expression in the appropriate way
	switch(stmt->type) {

		case stmt_For: {
			// Interpret the assignment in the for loop declaration
			interpret_statement(stmt->stmt->_for->assignment, scope, prog);

			// Repeatedly execute each sub-statement, and do the specified
			// incrementation after each iteration
			while(interpret_expression(
				stmt->stmt->_for->bool_expr, scope, prog)) {

				// Proliferate the scope for this iteration of the for-loop body
				Scope_proliferate(scope);

				// Interpret each statement for this iteration
				int i;
				for(i = 0; i < LinkedList_length(
					stmt->stmt->_for->stmts); i++) {
					
					interpret_statement(LinkedList_get(
						stmt->stmt->_for->stmts, i), scope, prog);

					// If a return value is found after any statement, we must
					// break, not interpreting any more statements in the
					// function
					if(scope->has_return) break;
				}

				// Recede the scope now that this loop iteration has finished
				Scope_recede(scope);

				if(scope->has_return) break;

				// If we reach this statement, we have not reached a return, so
				// increment the loop control variable for the next iteration
				interpret_statement(stmt->stmt->_for->incrementor, scope, prog);
			}
		}
		case stmt_While: {
			// Repeatedly execute each sub-statement, and do the specified
			// incrementation after each iteration
			while(interpret_expression(
				stmt->stmt->_while->bool_expr, scope, prog)) {

				// Proliferate the scope for this iteration of the for-loop body
				Scope_proliferate(scope);

				// Interpret each statement for this iteration
				int i;
				for(i = 0; i < LinkedList_length(
					stmt->stmt->_while->stmts); i++) {
					
					interpret_statement(LinkedList_get(
						stmt->stmt->_while->stmts, i), scope, prog);

					// If a return value is found after any statement, we must
					// break, not interpreting any more statements in the
					// function
					if(scope->has_return) break;
				}

				// Recede the scope now that this loop iteration has finished
				Scope_recede(scope);

				if(scope->has_return) break;
			}
		}
		case stmt_If: {
			// Determine which statement list will be interpreted
			LinkedList *stmts_to_interpret = 
				interpret_expression(stmt->stmt->_if->bool_expr, scope, prog) ?
					stmt->stmt->_if->true_stmts : stmt->stmt->_if->false_stmts;

			// Proliferate the scope for the if/else statements
			Scope_proliferate(scope);

			// Interpret each statement
			int i;
			for(i = 0; i < LinkedList_length(stmts_to_interpret); i++) {
				
				interpret_statement(LinkedList_get(
					stmts_to_interpret, i), scope, prog);

				// If a return value is found after any statement, we must
				// break, not interpreting any more statements in the function
				if(scope->has_return) break;
			}

			// Recede the scope now that this loop iteration has finished
			Scope_recede(scope);
		}
		case stmt_Print: {
			// Just print the given expression
			printf("%d\n", interpret_expression(
				stmt->stmt->_print->expr, scope, prog));
		}
		case stmt_Assignment: {
			// Update the scope with a value for the variable name
			Scope_update(scope, stmt->stmt->_assignment->name, 
				interpret_expression(
					stmt->stmt->_assignment->expr, scope, prog));
		}
		case stmt_Return: {
			// Record the return value as the interpreted expression
			scope->return_value = interpret_expression(
				stmt->stmt->_return->expr, scope, prog);

			// Mark the scope has having evaluated a return statement
			scope->has_return = true;
		}
	}
}

/*
 * Interpret and return the result of the given function with the given
 * arguments in the context of the given program
 */
int interpret_function(FNDecl *function, LinkedList *args, Program *prog) {

	// Check that the function has been called with the appropriate number of
	// arguments
	if(LinkedList_length(args) != LinkedList_length(function->arg_names)) {

		printf("Function: '%s' takes %d arguments, %d given",
			function->name,
			LinkedList_length(function->arg_names),
			LinkedList_length(args));
		exit(EXIT_FAILURE);
	}

	// Create the VariableScope for this function
	Scope *scope = Scope_init(function->arg_names, args);

	// Interpret each statement
	int i;
	for(i = 0; i < LinkedList_length(function->stmts); i++) {
		
		interpret_statement(LinkedList_get(
			function->stmts, i), scope, prog);

		// If a return value is found after any statement, we must
		// break, not interpreting any more statements in the function
		if(scope->has_return) break;
	}

	// If the statement interpretation loop finishes without returning, an error
	// has occurred in that the function had no return statement, so raise an
	// error
	if(!(scope->has_return)) {
		printf("Reached end of function '%s' without return statement\n",
			function->name);
		exit(EXIT_FAILURE);
		return (int) NULL;
	}
	// Otherwise return the return value
	else {
		int return_value = scope->return_value;
		Scope_free(scope);
		return return_value;
	}
}

int interpret_program(Program *prog, LinkedList *args) {

	int i;
	for(i = 0; i < LinkedList_length(prog->function_list); i++) {

	}
	return (int) NULL;
}