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
	struct BooleanExpr *blean;
	struct ArithmeticExpr *arith;
	char *ident;
	int intgr;
	struct FNCall *fncall;
	struct Ternary *trnry;
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
	int exec_count;
} Expression;

typedef struct BooleanExpr BooleanExpr;
struct BooleanExpr {
	Expression *lhs;
	char *op;
	Expression *rhs;
};

typedef struct ArithmeticExpr ArithmeticExpr;
struct ArithmeticExpr {
	Expression *lhs;
	char *op;
	Expression *rhs;
};

typedef struct FNCall FNCall;
struct FNCall {
	char *name;
	LinkedList *args;
};

/*
 * A Ternary has three components: a boolean expression, a true-expression, and
 * a false-expression
 */
typedef struct Ternary Ternary;
struct Ternary {
	Expression *bool_expr;
	Expression *true_expr;
	Expression *false_expr;
};

/*
 * The following data structures are used to represent statements. Statement
 * objects have the following structure:
 * 	STATEMENT INSTANCE:
 * 		ENUM VALUE:
 * 			STATEMENT TYPE NAME
 * 		UNION VALUE:
 * 			POINTER TO OBJECT OF TYPE:
 * 				[ FOR | WHILE | IF | PRINT | ASSIGNMENT | RETURN ]
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
	int exec_count;
} Statement;

typedef struct For For;
struct For {
	Statement *assignment;
	Expression *bool_expr;
	Statement *incrementor;
	LinkedList *stmts;
};

typedef struct While While;
struct While {
	Expression *bool_expr;
	LinkedList *stmts;
};

typedef struct If If;
struct If {
	Expression *bool_expr;
	LinkedList *true_stmts;
	LinkedList *false_stmts;
};

typedef struct Print Print;
struct Print {
	Expression *expr;
};

typedef struct Assignment Assignment;
struct Assignment {
	char *name;
	Expression *expr;
};

typedef struct Return Return;
struct Return {
	Expression *expr;
};

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

Expression *Identifier_init(char *ident);

Expression *IntegerLiteral_init(int intgr);

Expression *FNCall_init(char *name, LinkedList *args);

Expression *Ternary_init(Expression *bool_expr,
	Expression *true_expr, Expression *false_expr);

char *Expression_str(Expression *expr);

bool Expression_equals(Expression *expr1, Expression *expr2);

void Expression_free(Expression *expr);

Statement *For_init(Statement *assignment, Expression *bool_expr,
	Statement *incrementor, LinkedList *stmts);

Statement *While_init(Expression *bool_expr, LinkedList *stmts);

Statement *If_init(Expression *bool_expr,
	LinkedList *true_stmts, LinkedList *false_stmts);

Statement *Print_init(Expression *expr);

Statement *Assignment_init(char *name, Expression *expr);

Statement *Return_init(Expression *expr);

bool Statement_equals(Statement *stmt1, Statement *stmt2);

void Statement_free(Statement *stmt);

FNDecl *FNDecl_init(char *name, LinkedList *arg_names, LinkedList *stmts);

bool FNDecl_equals(FNDecl *f1, FNDecl *f2);

void FNDecl_free(FNDecl *func);

Program *Program_init(LinkedList *function_list);

bool Program_equals(Program *p1, Program *p2);

void Program_free(Program *prog);

#endif // AST