/*
 * Header file for AST.c
 * Contains struct definitions for AST objects
 */

#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef AST
#define AST

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
	struct BooleanExpr *_bool;
	struct ArithmeticExpr *_arith;
	char *_ident;
	int _int;
	struct FNCall *_call;
	struct Ternary *_tern;
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
	u_expr *expr;
} Expression;

typedef struct {
	Expression *lhs;
	char *op;
	Expression *rhs;
} BooleanExpr;

typedef struct {
	Expression *lhs;
	char *op;
	Expression *rhs;
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
	Expression *bool_expr;
	Expression *true_expr;
	Expression *false_expr;
} Ternary;

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
	struct For *_for;
	struct If *_if;
	struct While *_while;
	struct Print *_print;
	struct Assignment *_assignment;
	struct Return *_return;
} u_stmt;

typedef struct {
	stmt_type type;
	u_stmt *stmt;
} Statement;

typedef struct {
	Statement *assignment;
	Expression *bool_expr;
	Statement *incrementor;
	LinkedList *stmts;
} For;

typedef struct {
	Expression *bool_expr;
	LinkedList *stmts;
} While;

typedef struct {
	Expression *bool_expr;
	LinkedList *true_stmts;
	LinkedList *false_stmts;
} If;

typedef struct {
	Expression *expr;
} Print;

typedef struct {
	char *name;
	Expression *expr;
} Assignment;

typedef struct {
	Expression *expr;
} Return;

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
 	LinkedList *function_list;
 } Program;

/*
 * Forward declarations for constructors for the above structs, defined in AST.c
 */

Expression *BooleanExpr_init(Expression *lhs, char *op, Expression *rhs);

Expression *ArithmeticExpr_init(Expression *lhs, char *op, Expression *rhs);

Expression *Identifier_init(char *_ident);

Expression *IntegerLiteral_init(int _int);

Expression *FNCall_init(char *name, LinkedList *args);

Expression *Ternary_init(Expression *bool_expr,
	Expression *true_expr, Expression *false_expr);

void Expression_free(Expression *expr);

Statement *For_init(Statement *assignment,
	Expression *bool_expr, Statement *incrementor);

Statement *While_init(Expression *bool_expr, LinkedList *stmts);

Statement *If_init(Expression *bool_expr,
	LinkedList *true_stmts, LinkedList *false_stmts);

Statement *Print_init(Expression *expr);

Statement *Assignment_init(char *name, Expression *expr);

Statement *Return_init(Expression *expr);

void Statement_free(Statement *stmt);

#endif // AST