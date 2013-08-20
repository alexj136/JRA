#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"
#include "runjit.h"

ArrLen *ArrLen_init(byte *arr, int len) {
	ArrLen *al = (ArrLen *)malloc(sizeof(ArrLen));
	al->arr = arr;
	al->len = len;
	return al;
}

ArrLen *ArrLen_copy(ArrLen *original) {
	void *new_arr = malloc(original->len);
	memcpy(new_arr, original->arr, original->len);
	return ArrLen_init(new_arr, original->len);
}

ArrLen *ArrLen_concat_2(ArrLen *al1, ArrLen *al2) {
	void *new_mem = malloc(al1->len + al2->len);
	memcpy(new_mem, al1->arr, al1->len);
	memcpy(new_mem + al1->len, al2->arr, al2->len);
	return ArrLen_init(new_mem, al1->len + al2->len);
}

ArrLen *ArrLen_concat(int count, ...) {

	// If count is zero, there is nothing to do, return NULL as specified.
	if(count == 0) return NULL;

	// Prepare varargs for usage
	va_list args;
	va_start(args, count);

	// Copy the first argument. If we don't use a copy, the for loop would free
	// the passed ArrLen, which would be very, very bad.
	ArrLen *next_arrlen = ArrLen_copy(va_arg(args, ArrLen *));


	// If count is one, copy the string using safe_strdup and return a pointer
	// to it (we must also call va_end to clean up the varargs)
	if(count == 1) {
		va_end(args);
		return next_arrlen;
	}

	// Repeatedly get the next ArrLen, concatenate it to the previous one, and
	// free the previous one.
	int i;
	for(i = 1; i < count; i++) {
		ArrLen *temp = va_arg(args, ArrLen *);
		ArrLen *temp_2 = ArrLen_concat_2(next_arrlen, temp);

		free(next_arrlen->arr);
		free(next_arrlen);
		next_arrlen = temp_2;
	}

	// Clean up varargs and return
	va_end(args);
	return next_arrlen;
}

ArrLen *jitcode_expression(Expression *expr, Program *prog) {

	switch(expr->type) {

		case expr_BooleanExpr: {

			// Generate jitcode for the left & right hand sides
			ArrLen *lhs = jitcode_expression(expr->expr->blean->lhs, prog);
			ArrLen *rhs = jitcode_expression(expr->expr->blean->rhs, prog);

			ArrLen *opcode;

			// Store the appropriate opcode
			if(expr->expr->arith->op == EQUAL) {
				// cmove %edx, %ecx
				byte opc[3] = { 0x0F, 0x44, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->arith->op == NOT_EQUAL) {
				// cmovne %edx, %ecx
				byte opc[3] = { 0x0F, 0x45, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->arith->op == LESS_THAN) {
				// cmovl %edx, %ecx
				byte opc[3] = { 0x0F, 0x4C, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->arith->op == LESS_OR_EQUAL) {
				// cmovle %edx, %ecx
				byte opc[3] = { 0x0F, 0x4E, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->arith->op == GREATER_THAN) {
				// cmovg %edx, %ecx
				byte opc[3] = { 0x0F, 0x4F, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->arith->op == GREATER_OR_EQUAL) {
				// cmovge %edx, %ecx
				byte opc[3] = { 0x0F, 0x4D, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else {
				printf("Invalid boolean operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			// Concatenate everything together to get the compiled AST
			ArrLen * out[18] = ArrLen_concat(

				rhs,

				// pushl %eax
				ArrLen_init({0x50}, 1),

				lhs,

				ArrLen_init({

					// popl %ebx
					0x5B,

					// movl $0, %ecx
					0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,

					// movl $1, %edx
					0x8B, 0x15, 0x01, 0x00, 0x00, 0x00, 
					
					// cmpl %ebx, %eax
					0x39, 0xD8

				}, 15),

				opcode,

				// movl %ecx, %eax
				ArrLen_init({ 0x89, 0xC8 }, 2)
			);

			// We can free lhs and rhs as their contents are copied into out by
			// str_concat
			free(lhs);
			free(rhs);
			free(opcode);

			// Return the generated string
			return NULL;
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