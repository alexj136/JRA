#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"

#define ADD_EBX_TO_EAX "addl %ebx, %eax\n"
#define SUB_EBX_TO_EAX "subl %ebx, %eax\n"
#define MUL_EBX_TO_EAX "???? %ebx, %eax\n"
#define DIV_EBX_TO_EAX "???? %ebx, %eax\n"
#define MOD_EBX_TO_EAX "???? %ebx, %eax\n"

char *codegen_expression(Expression *expr) {
	switch(expr->type) {
		case expr_BooleanExpr: {

		}
		case expr_ArithmeticExpr: {
			char *lhs = codegen_expression(expr->expr->arith->lhs);
			char *rhs = codegen_expression(expr->expr->arith->rhs);

			char *op;
			if(expr->expr->arith->op == PLUS) op = ADD_EBX_TO_EAX;
			else if(expr->expr->arith->op == MINUS) op = SUB_EBX_TO_EAX;
			else if(expr->expr->arith->op == MULTIPLY) op = MUL_EBX_TO_EAX;
			else if(expr->expr->arith->op == DIVIDE) op = DIV_EBX_TO_EAX;
			else if(expr->expr->arith->op == MODULO) op = MOD_EBX_TO_EAX;
			else {
				printf("Invalid operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			char *out = str_concat_five(
				lhs,
				"push %eax\n",
				rhs,
				"pop %ebx\n",
				op);

			free(lhs);
			free(rhs);

			return out;
		}
		case expr_Identifier: {

		}
		case expr_IntegerLiteral: {
			// Allcoate a new character buffer on the heap
			char *buffer = safe_alloc(sizeof(char) * 30);

			// Generate the correct string in the buffer
			sprintf(buffer, "movl $%d, %%eax\n", expr->expr->intgr);

			// Return a pointer to the buffer
			return buffer;
		}
		case expr_FNCall: {

		}
		case expr_Ternary: {

		}
	}
	return NULL;
}

int main() {
	printf("%s\n",
		codegen_expression(
			ArithmeticExpr_init(
				ArithmeticExpr_init(
					IntegerLiteral_init(4),
					PLUS,
					IntegerLiteral_init(5)),
				PLUS,
				ArithmeticExpr_init(
					IntegerLiteral_init(6),
					PLUS,
					IntegerLiteral_init(7)
				)
			)
		)
	);
	return 0;
}