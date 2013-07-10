/*
 * Header file for AST.c
 * Contains struct definitions for AST objects
 */

/*
 * Statement types
 */
typedef enum {
	stmt_For,
	stmt_While,
	stmt_If,
	stmt_Print,
	stmt_Assignment,
	stmt_Return
} stmt_type;

typedef struct {
	struct Statement *assignment;
	struct Expression *bool_expr;
	struct Statement *incrementor;
	struct StmtList *stmts;
} For;

typedef struct {
	struct Expression *bool_expr;
	struct StmtList *stmts;
} While;

typedef struct {
	struct Expression *bool_expr;
	struct StmtList *true_stmts;
	struct StmtList *false_stmts;
} If;

typedef struct {
	struct Expression *_exp;
} Print;

typedef struct {
	char *name;
	struct Expression *_exp;
} Assignment;

typedef struct {
	struct Expression *_exp;
} Return;

typedef struct {
	stmt_type type;
	union {
		For *_for;
		If *_if;
		While *_while;
		Print *_print;
		Assignment *_assignment;
		Return *_return;
	} stmt;
} Statement;

typedef struct {
	Expression *exps;
} ExpList;

/*
 * Expression types
 */
typedef enum {
	exp_BooleanExpr,
	exp_ArithmeticExpr,
	exp_Identifier,
	exp_IntegerLiteral,
	exp_FNCall,
	exp_Ternary
} exp_type;

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
	struct ExpList *args;
} FNCall;

typedef struct {
	struct BooleanExpr *bool_exp;
	struct Expression *true_exp;
	struct Expression *false_exp;
} Ternary;

typedef struct {
	exp_type type;
	union {
		BooleanExpr *_bool;
		ArithmeticExpr *_arith;
		char *_ident;
		int _int;
		FNCall *_call;
		Ternary *_tern;
	} exp;
} Expression;

/*
 * FNDecl type
 */
typedef struct {
	char *name;
	char **arg_names;
	struct StmtList *stmts;
} FNDecl;