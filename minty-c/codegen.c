#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"

/*
 * Macros for arithmetic operations
 */
#define ADD "addl %ebx, %eax\n"
#define SUB "subl %ebx, %eax\n"
#define MUL "imull %ebx\n"
#define DIV "idivl %ebx\n"
#define MOD "idivl %ebx\nmovl %edx, %eax"

char *codegen_expression(Expression *expr) {

	switch(expr->type) {

		case expr_BooleanExpr: {

			// Generate strings for the left & right hand sides
			char *lhs = codegen_expression(expr->expr->blean->lhs);
			char *rhs = codegen_expression(expr->expr->blean->rhs);

			
		}

		case expr_ArithmeticExpr: {

			// The general strategy for generating arithmetic expressions is to
			// generate the right hand side, such that the rhs result is in the
			// accumulator (%eax), push it onto the stack, the generate the left
			// hand side, so the lhs is on the accumulator. Then we pop the rhs
			// into a secondary register (%ebx) and do the operation, storing
			// the result in the accumulator (%eax)

			// Generate strings for the left & right hand sides
			char *lhs = codegen_expression(expr->expr->arith->lhs);
			char *rhs = codegen_expression(expr->expr->arith->rhs);

			// opcode will store the operation specified by the AST
			char *opcode;

			// out is the return variable to point to out output string
			char *out;

			// Store the appropriate opcode - see the #defined macros for each
			// at the top of this file
			if(expr->expr->arith->op == PLUS) opcode = ADD;
			else if(expr->expr->arith->op == MINUS) opcode = SUB;
			else if(expr->expr->arith->op == MULTIPLY) opcode = MUL;
			else if(expr->expr->arith->op == DIVIDE) opcode = DIV;
			else if(expr->expr->arith->op == MODULO) opcode = MOD;
			else {
				printf("Invalid operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			// Concatenate everything together to get the compiled AST
			out = str_concat_five(
				rhs,
				"push %eax\n",
				lhs,
				"pop %ebx\n",
				opcode);

			// We can free lhs and rhs as their contents are copied into out by
			// str_concat_five
			free(lhs);
			free(rhs);

			// Return the generated string
			return out;
		}

		case expr_Identifier: {

		}
		case expr_IntegerLiteral: {
			// Allocate a new character buffer on the heap
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
				MODULO,
				ArithmeticExpr_init(
					IntegerLiteral_init(4),
					PLUS,
					IntegerLiteral_init(3)
				)
			)
		)
	);
	return 0;
}