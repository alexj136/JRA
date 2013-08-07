#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"

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

			char *out;

			// The add operation being used has the form: addl %rg1, %rg2. addl
			// refers to 'add long', i.e. 32 bit operands. %rg1 is the source
			// register, %r2 is the destination. the result is %r1 + %r2 and the
			// result is left in %r2.
			if(expr->expr->arith->op == PLUS) {
				out = str_concat_five(
				lhs,
				"push %eax\n",
				rhs,
				"pop %ebx\n",
				"addl %ebx, %eax\n");
			}

			// Subtraction works in almost exactly the same way: the result is
			// %r2 - %r1, stored in %r2.
			else if(expr->expr->arith->op == MINUS) {
				out = str_concat_five(
				lhs,
				"push %eax\n",
				rhs,
				"pop %ebx\n",
				"subl %ebx, %eax\n");
			}
			else if(expr->expr->arith->op == MULTIPLY) {
				out = str_concat_five(
				rhs,
				"push %eax\n",
				lhs,
				"pop %ebx\n",
				"imul %ebx\n");
			}
			// else if(expr->expr->arith->op == DIVIDE) op = DIV_EBX_TO_EAX;
			// else if(expr->expr->arith->op == MODULO) op = MOD_EBX_TO_EAX;
			else {
				printf("Invalid operation type in AST\n");
				exit(EXIT_FAILURE);
			}

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
	// (4 * 5) - (6 + 7) should equal 7
	printf("%s\n",
		codegen_expression(
			ArithmeticExpr_init(
				ArithmeticExpr_init(
					IntegerLiteral_init(4),
					MULTIPLY,
					IntegerLiteral_init(5)),
				MINUS,
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