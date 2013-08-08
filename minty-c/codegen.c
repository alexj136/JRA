#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"

/*
 * Macros for boolean operations
 */
#define EQU "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmove %edx, %ecx\n\
movl %ecx, %eax\n\
"

#define NEQ "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmovne %edx, %ecx\n\
movl %ecx, %eax\n\
"

#define LSS "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmovl %edx, %ecx\n\
movl %ecx, %eax\n\
"

#define LSE "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmovle %edx, %ecx\n\
movl %ecx, %eax\n\
"

#define GTR "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmovg %edx, %ecx\n\
movl %ecx, %eax\n\
"

#define GTE "\
movl $0, %ecx\n\
movl $1, %edx\n\
cmpl %ebx, %eax\n\
cmovge %edx, %ecx\n\
movl %ecx, %eax\n\
"

/*
 * Macros for arithmetic operations
 */

// Addition: add the contents of %ebx and %eax, result in %eax
#define ADD "addl %ebx, %eax\n"

// Subtraction: subtract the contents of %ebx from %eax, result in %eax
#define SUB "subl %ebx, %eax\n"

// Multiplucation: multiply the contents of %ebx and %eax, result in %eax
#define MUL "imull %ebx\n"

// Division: divide the contents of %eax by %ebx, result in %eax
#define DIV "idivl %ebx\n"

// Modulo: divide the contents of %eax by %ebx, result in %eax, remainder in
// %edx. Move %edx into %eax because we want the remainder.
#define MOD "\
idivl %ebx\n\
movl %edx, %eax\n\
"

char *codegen_expression(Expression *expr) {

	switch(expr->type) {

		case expr_BooleanExpr: {

			// Generate strings for the left & right hand sides
			char *lhs = codegen_expression(expr->expr->blean->lhs);
			char *rhs = codegen_expression(expr->expr->blean->rhs);

			// opcode will store the operation specified by the AST
			char *opcode;

			// out is the return variable to point to out output string
			char *out;

			// Store the appropriate opcode - see the #defined macros for each
			// at the top of this file
			     if(expr->expr->arith->op ==            EQUAL) opcode = EQU;
			else if(expr->expr->arith->op ==        NOT_EQUAL) opcode = NEQ;
			else if(expr->expr->arith->op ==        LESS_THAN) opcode = LSS;
			else if(expr->expr->arith->op ==    LESS_OR_EQUAL) opcode = LSE;
			else if(expr->expr->arith->op ==     GREATER_THAN) opcode = GTR;
			else if(expr->expr->arith->op == GREATER_OR_EQUAL) opcode = GTE;
			else {
				printf("Invalid boolean operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			// Concatenate everything together to get the compiled AST
			out = str_concat_five(
				rhs,
				"pushl %eax\n",
				lhs,
				"popl %ebx\n",
				opcode);

			// We can free lhs and rhs as their contents are copied into out by
			// str_concat_five
			free(lhs);
			free(rhs);

			// Return the generated string
			return out;
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
			     if(expr->expr->arith->op ==     PLUS) opcode = ADD;
			else if(expr->expr->arith->op ==    MINUS) opcode = SUB;
			else if(expr->expr->arith->op == MULTIPLY) opcode = MUL;
			else if(expr->expr->arith->op ==   DIVIDE) opcode = DIV;
			else if(expr->expr->arith->op ==   MODULO) opcode = MOD;
			else {
				printf("Invalid arithmetic operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			// Concatenate everything together to get the compiled AST
			out = str_concat_five(
				rhs,
				"pushl %eax\n",
				lhs,
				"popl %ebx\n",
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
			char *b_exp = codegen_expression(expr->expr->trnry->bool_expr);
			char *t_exp = codegen_expression(expr->expr->trnry->true_expr);
			char *f_exp = codegen_expression(expr->expr->trnry->false_expr);
			
			return str_concat_eight(
				"B_EXPR\n"
				"cmpl $0, %eax\n"
				"je trn_false\n"
				"T_EXPR\n"
				"jmp trn_end\n"
				"trn_false:\n"
				"F_EXPR\n"
				"trn_end:\n");
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