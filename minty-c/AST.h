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
 * The following enumerations are used by AST structures to represent operators
 * in expressions where multiple operator types are valid.
 */

/*
 * op_arithmetic is used to denote + (addition), - (subtraction), *
 * (mulitplucation), / (integer division) and % (modulo/remainder after integer
 * division)
 */
typedef enum {
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MODULO
} op_arithmetic;

/*
 * op_boolean is used to denote = (equality), != (inequality), < (less-than),
 * > (greater-than), <= (less or equal) and >= (greater or equal)
 */
typedef enum {
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_OR_EQUAL,
	GREATER_THAN,
	GREATER_OR_EQUAL
} op_boolean;

/*
 * The following data structures are used to represent expressions. Expression
 * objects have the following structure:
 * 	EXPRESSION INSTANCE:
 * 		ENUM VALUE:
 * 			EXPRESSION TYPE NAME
 * 		UNION VALUE:
 * 			POINTER TO OBJECT OF TYPE:
 * 				[ BOOLEANEXPR | ARITHMETICEXPR | IDENTIFIER |
 *											INTEGERLITERAL | FNCALL | TERNARY ]
 */

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

/*
 * u_expr unions can store one object of the expression types. This is used to
 * implement OO-inheritance like behaviour - any expression object, regardless
 * of subtype, can be stored. They are all stored with pointers, except for
 * integer literals, which simply have a literal rather than a pointer to an
 * object containing the literal.
 */
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

/*
 * Boolean expressions have three fields: the left hand side expression, the
 * right hand side expression and the boolean operation between them
 */
typedef struct BooleanExpr BooleanExpr;
struct BooleanExpr {
	Expression *lhs;
	char *op;
	Expression *rhs;
};

/*
 * Arithmetic expressions have three fields: the left hand side expression, the
 * right hand side expression and the arithmetic operation between them
 */
typedef struct ArithmeticExpr ArithmeticExpr;
struct ArithmeticExpr {
	Expression *lhs;
	char *op;
	Expression *rhs;
};

/*
 * FNCall expressions have a name, and a list of arguments, which are
 * expressions themselves.
 */
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

/*
 * The stmt_type enum is included in a Statement object so that handling code
 * can easily tell what kind of Statement it has
 */
typedef enum {
	stmt_For,
	stmt_While,
	stmt_If,
	stmt_Print,
	stmt_Assignment,
	stmt_Return
} stmt_type;

/*
 * u_stmt unions can store one object of the statement types. This is used to
 * implement OO-inheritance like behaviour - any statement object, regardless of
 * subtype, can be stored
 */
typedef union {
	struct For *_for;
	struct If *_if;
	struct While *_while;
	struct Print *_print;
	struct Assignment *_assignment;
	struct Return *_return;
} u_stmt;

/*
 * The Statement struct has a type field, so that any code handling it can find
 * out what value the u_stmt field has, without segfaulting due to null
 * pointers. The exec_count field is used by the interpreter/JIT compiler to
 * determine whether or not the statement should be compiled
 */
typedef struct {
	stmt_type type;
	u_stmt *stmt;
	int exec_count;
} Statement;

/*
 * For Statements have an assignment, a boolean expression, and an
 * incrementation operation. It also has a list of sub-statements.
 */
typedef struct For For;
struct For {
	Statement *assignment;
	Expression *bool_expr;
	Statement *incrementor;
	LinkedList *stmts;
};

/*
 * While Statements are simpler than For Statements - they simply have a boolean
 * expression and a list of statements
 */
typedef struct While While;
struct While {
	Expression *bool_expr;
	LinkedList *stmts;
};

/*
 * If Statements have the boolean expression, and the statement list that is
 * evaluated when the boolean expression evaluates to true (true_stmts) and the
 * list that is evaluated when the boolean expression evaluates to false 
 * (false_stmts)
 */
typedef struct If If;
struct If {
	Expression *bool_expr;
	LinkedList *true_stmts;
	LinkedList *false_stmts;
};

/*
 * Print statements simply store the expression that they are printing
 */
typedef struct Print Print;
struct Print {
	Expression *expr;
};

/*
 * Assignment statements store the name of the variable being assigned to, and
 * the expression to be assigned to it
 */
typedef struct Assignment Assignment;
struct Assignment {
	char *name;
	Expression *expr;
};

/*
 * Return statements only store the expression being returned
 */
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