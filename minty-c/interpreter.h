#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef TOKEN
#include "token.h"
#endif // TOKEN

#ifndef AST
#include "AST.h"
#endif // AST

#ifndef INTERPRETER
#define INTERPRETER

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
 * Functions for Scope objects
 */

Scope *Scope_init();

void Scope_update(Scope *scope, char *name, int value);

int Scope_get(Scope *scope, char *name);

void Scope_proliferate(Scope *scope);

void Scope_recede(Scope *scope);

bool Scope_has(Scope *scope, char *name);

void Scope_free(Scope *scope);

/*
 * Interpreter functions
 */
 
int interpret_expression(Expression *expr, Scope *scope);

void interpret_statement(Statement *stmt);

int interpret_function(char *name, LinkedList *args);

int interpret_program(Program *prog, LinkedList *args);

#endif // INTERPRETER