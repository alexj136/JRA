#include <stdio.h>
#include <sys/mman.h>

char *jitcode_expression(Expression *expr, Program *prog) {

	switch(expr->type) {

		case expr_BooleanExpr: {

			// Generate jitcode for the left & right hand sides
			int *lhs = jitcode_expression(expr->expr->blean->lhs, prog);
			int *rhs = jitcode_expression(expr->expr->blean->rhs, prog);

			char *opcode;

			// Store the appropriate opcode
			if(expr->expr->arith->op == EQUAL) {
				opcode = { 0x0F, 0x44, 0xCA };
			}
			else if(expr->expr->arith->op == NOT_EQUAL) {
				opcode = { 0x0F, 0x45, 0xCA };
			}
			else if(expr->expr->arith->op == LESS_THAN) {
				opcode = { 0x0F, 0x4C, 0xCA };
			}
			else if(expr->expr->arith->op == LESS_OR_EQUAL) {
				opcode = { 0x0F, 0x4E, 0xCA };
			}
			else if(expr->expr->arith->op == GREATER_THAN) {
				opcode = { 0x0F, 0x4F, 0xCA };
			}
			else if(expr->expr->arith->op == GREATER_OR_EQUAL) {
				opcode = { 0x0F, 0x4D, 0xCA };
			}
			else {
				printf("Invalid boolean operation type in AST\n");
				exit(EXIT_FAILURE);int
			}

			// Concatenate everything together to get the compiled AST
			char *out = {

				rhs,

				// pushl %eax
				0x50,

				lhs,

				// popl %ebx
				0x5B,

				// movl $0, %ecx
				0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,

				// movl $1, %edx
				0x8B, 0x15, 0x01, 0x00, 0x00, 0x00, 
				
				// cmpl %ebx, %eax
				0x39, 0xD8,

				opcode,

				// movl %ecx, %eax
				0x89C8
			};

			// We can free lhs and rhs as their contents are copied into out by
			// str_concat
			free(lhs);
			free(rhs);

			// Return the generated string
			return out;
		}

		case expr_ArithmeticExpr: {
			printf("Arithmetic jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_Identifier: {
			printf("Identifier jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_IntegerLiteral: {
			printf("Integer jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_FNCall: {
			printf("Function call jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_Ternary: {
			printf("Ternary jit not yet implemented");
			exit(EXIT_FAILURE);
		}
	}
	printf("Invalid expression type in AST\n");
	exit(EXIT_FAILURE);
	return NULL;
}