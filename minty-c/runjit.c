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
			if(expr->expr->blean->op == EQUAL) {
				// cmove %edx, %ecx
				byte opc[3] = { 0x0F, 0x44, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->blean->op == NOT_EQUAL) {
				// cmovne %edx, %ecx
				byte opc[3] = { 0x0F, 0x45, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->blean->op == LESS_THAN) {
				// cmovl %edx, %ecx
				byte opc[3] = { 0x0F, 0x4C, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->blean->op == LESS_OR_EQUAL) {
				// cmovle %edx, %ecx
				byte opc[3] = { 0x0F, 0x4E, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->blean->op == GREATER_THAN) {
				// cmovg %edx, %ecx
				byte opc[3] = { 0x0F, 0x4F, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else if(expr->expr->blean->op == GREATER_OR_EQUAL) {
				// cmovge %edx, %ecx
				byte opc[3] = { 0x0F, 0x4D, 0xCA };
				opcode = ArrLen_init(&(opc[0]), 3);
			}
			else {
				printf("Invalid boolean operation type in AST\n");
				exit(EXIT_FAILURE);
			}

			// The following byte arrays encode the machine code operations
			// required to compute the given expression

			// rhs goes here

			// pushl %eax
			byte instr1[1] = { 0x50 };
			ArrLen *arrlen_instr1 = ArrLen_init(&(instr1[0]), 1);

			// lhs goes here

			byte instr2[15] = {

				// popl %ebx
				0x5B,

				// movl $0, %ecx
				0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,

				// movl $1, %edx
				0x8B, 0x15, 0x01, 0x00, 0x00, 0x00, 
				
				// cmpl %ebx, %eax
				0x39, 0xD8

			};
			ArrLen *arrlen_instr2 = ArrLen_init(&(instr2[0]), 15);

			// opcode goes here

			// movl %ecx, %eax
			byte instr3[2] = { 0x89, 0xC8 };
			ArrLen *arrlen_instr3 = ArrLen_init(&(instr3[0]), 2);

			// Concatenate everything into one ArrLen object
			ArrLen *out = ArrLen_concat(6,
				rhs,
				arrlen_instr1,
				lhs,
				arrlen_instr2,
				opcode,
				arrlen_instr3
			);


			// We can free lhs and rhs as their contents are copied into out by
			// str_concat
			free(lhs);
			free(arrlen_instr1);
			free(rhs);
			free(arrlen_instr2);
			free(opcode);
			free(arrlen_instr3);

			// Return the generated string
			return out;
		}

		case expr_ArithmeticExpr: {

			// Generate jitcode for the left & right hand sides
			ArrLen *lhs = jitcode_expression(expr->expr->arith->lhs, prog);
			ArrLen *rhs = jitcode_expression(expr->expr->arith->rhs, prog);

			ArrLen *opcode;

			// Store the appropriate opcode
			if(expr->expr->arith->op == PLUS) {
				// addl %ebx, %eax
				byte opc[2] = { 0x01, 0xD8 };
				opcode = ArrLen_init(&(opc[0]), 2);
			}
			else if(expr->expr->arith->op == MINUS) {
				// subl %ebx, %eax
				byte opc[2] = { 0x29, 0xD8 };
				opcode = ArrLen_init(&(opc[0]), 2);
			}
			else if(expr->expr->arith->op == MULTIPLY) {
				// imull %ebx
				byte opc[2] = { 0xF7, 0xEB };
				opcode = ArrLen_init(&(opc[0]), 2);
			}
			else if(expr->expr->arith->op == DIVIDE) {
				// idivl %ebx
				byte opc[2] = { 0xF7, 0xFB };
				opcode = ArrLen_init(&(opc[0]), 2);
			}
			else if(expr->expr->arith->op == MODULO) {
				
				byte opc[4] = { 
					
					// idivl %ebx
					0xF7, 0xFB, 

					// movl %edx, %eax
					0x89, 0xD0 };

				opcode = ArrLen_init(&(opc[0]), 4);
			}
			else {
				printf("Invalid arithmetic operation type in AST\n");
				exit(EXIT_FAILURE);
			}


			// The following byte arrays encode the machine code operations
			// required to compute the given expression

			// rhs goes here

			// pushl %eax
			byte instr1[1] = { 0x50 };
			ArrLen *arrlen_instr1 = ArrLen_init(&(instr1[0]), 1);

			// lhs goes here

			// popl %ebx
			byte instr2[1] = { 0x5B };
			ArrLen *arrlen_instr2 = ArrLen_init(&(instr2[0]), 1);

			// opcode goes here

			// Concatenate everything into one ArrLen object
			ArrLen *out = ArrLen_concat(5,
				rhs,
				arrlen_instr1,
				lhs,
				arrlen_instr2,
				opcode
			);


			// We can free lhs and rhs as their contents are copied into out by
			// str_concat
			free(lhs);
			free(arrlen_instr1);
			free(rhs);
			free(arrlen_instr2);
			free(opcode);

			// Return the generated string
			return out;
		}

		case expr_Identifier: {
			printf("Identifier jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_IntegerLiteral: {

			int the_int = expr->expr->intgr;
			byte *opcode = malloc(sizeof(char) * 5);

			// movl <value>, %eax
			opcode[0] = (byte) 0xB8;
			opcode[1] = (byte)((the_int & 0x000000FF) >>  0);
			opcode[2] = (byte)((the_int & 0x0000FF00) >>  8);
			opcode[3] = (byte)((the_int & 0x00FF0000) >> 16);
			opcode[4] = (byte)((the_int & 0xFF000000) >> 24);

			return ArrLen_init(opcode, sizeof(char) * 5);
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

int jitexec_expression(ArrLen *expr_code) {

	// Map the appropriate amount of writeable, executable memory, so we can
	// write the code in place and jump to it. We ask for space 1 greater than
	// the length of the given code so that we can include the return operation,
	// which is one byte long
	void *jit_memory = mmap(NULL, expr_code->len + sizeof(byte),
		PROT_WRITE | PROT_EXEC,	MAP_ANON | MAP_PRIVATE, -1, 0);

	// Write the code in place
	memcpy(jit_memory, expr_code->arr, expr_code->len);

	// The return operation
	byte ret = (byte) 0xC3; // ret

	// Write the return operation in place
	memcpy(jit_memory + expr_code->len, &ret, sizeof(byte));

	// Declare the mapped memory as a function so we can jump to it
	int (*jitexec)() = jit_memory;
	
	// Call the function we have created
	int result = (*jitexec)();

	// Free the memory that we mapped
	munmap(jit_memory, expr_code->len);

	return result;
}