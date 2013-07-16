/*
 * Header file for AST.c
 * Contains struct definitions for AST objects
 */

#include "minty_util.h"

/*
 * FNDecl type - contains the function name, a list of argument names, and a
 * list of statements
 */
typedef struct {
	char *name;
	LinkedList *arg_names;
	LinkedList *stmts;
} FNDecl;

/*
 * Program type - just a LinkedList of functions
 */
 typedef struct {
 	LinkedList *functions;
 } Program;

/*
 * The following data structures are used to represent statements. Statement
 * objects have the following structure:
 * 	STATEMENT INSTANCE:
 * 		ENUM VALUE:
 * 			STATEMENT TYPE NAME
 * 		UNION VALUE:
 * 			OBJECT OF TYPE [ FOR | WHILE | IF | PRINT | ASSIGNMENT | RETURN ]
 */
typedef enum {
	stmt_For,
	stmt_While,
	stmt_If,
	stmt_Print,
	stmt_Assignment,
	stmt_Return
} stmt_type;

typedef union {
	For *_for;
	If *_if;
	While *_while;
	Print *_print;
	Assignment *_assignment;
	Return *_return;
} u_stmt;

typedef struct {
	stmt_type type;
	u_stmt stmt;
} Statement;

typedef struct {
	struct Statement *assignment;
	struct Expression *bool_expr;
	struct Statement *incrementor;
	LinkedList *stmts;
} For;

typedef struct {
	struct Expression *bool_expr;
	struct StmtList *stmts;
} While;

typedef struct {
	struct Expression *bool_expr;
	LinkedList *true_stmts;
	LinkedList *false_stmts;
} If;

typedef struct {
	struct Expression *expr;
} Print;

typedef struct {
	char *name;
	struct Expression *expr;
} Assignment;

typedef struct {
	struct Expression *expr;
} Return;

/*
 * Enumeration of the types an expression can have
 */
typedef enum {
	expr_BooleanExpr,
	expr_ArithmeticExpr,
	expr_Identifier,
	expr_IntegerLiteral,
	expr_FNCall,
	expr_Ternary
} expr_type;

typedef union {
	BooleanExpr *_bool;
	ArithmeticExpr *_arith;
	char *_ident;
	int _int;
	FNCall *_call;
	Ternary *_tern;
} u_expr;

/*
 * An actual expression object. Since C does not have inheritance, we cannot
 * define functions that take an abstract type, passing in one of many concrete
 * subtypes. Instead, we must create a wrapper object that contains exactly one
 * of said 'subtypes', and an enum variable that indicates which of those types
 * the contained object belongs to.
 */
typedef struct {
	expr_type type;
	u_expr expr;
} Expression;

typedef struct {
	struct Expression *lhs;
	char *op;
	struct Expression *rhs;
} BooleanExpr;

typedef struct {
	struct Expression *lhs;
	char *op;
	struct Expression *rhs;
} ArithmeticExpr;

typedef struct {
	char *name;
	LinkedList *args;
} FNCall;

/*
 * A Ternary has three components: a boolean expression, a true-expression, and
 * a false-expression
 */
typedef struct {
	struct Expression *bool_expr;
	struct Expression *true_expr;
	struct Expression *false_expr;
} Ternary;