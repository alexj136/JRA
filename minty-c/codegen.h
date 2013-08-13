#ifndef MINTY_UTIL
#include "minty_util.h"
#endif // MINTY_UTIL

#ifndef TOKEN
#include "token.h"
#endif // TOKEN

#ifndef AST
#include "AST.h"
#endif // AST

#ifndef CODEGEN
#define CODEGEN

char *codegen_expression(Expression *expr);

char *codegen_statement_list(LinkedList *stmts);

char *codegen_statement(Statement *stmt);

#endif // CODEGEN