#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"
#include "jitcode.h"

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

/*
 * Writes an integer into a byte buffer in such a way that it can be used by
 * machine code.
 * 
 * buffer: the buffer to write into
 * offset: the offset from the start of the buffer at which the first byte of
 *         integer should be written (the other bytes follow immediately)
 * value:  The value to be written
 */
void put_int_as_bytes(byte *buffer, int offset, int value) {
	
	*(buffer + (sizeof(byte) * (0 + offset))) =
		(byte)((value & 0x000000FF) >>  0);

	*(buffer + (sizeof(byte) * (1 + offset))) =
		(byte)((value & 0x0000FF00) >>  8);

	*(buffer + (sizeof(byte) * (2 + offset))) =
		(byte)((value & 0x00FF0000) >> 16);

	*(buffer + (sizeof(byte) * (3 + offset))) =
		(byte)((value & 0xFF000000) >> 24);
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

			byte instr2[13] = {

				// popl %ebx
				0x5B,

				// movl $0, %ecx
				0xB9, 0x00, 0x00, 0x00, 0x00,

				// movl $1, %edx
				0xBA, 0x01, 0x00, 0x00, 0x00, 
				
				// cmpl %ebx, %eax
				0x39, 0xD8

			};
			ArrLen *arrlen_instr2 = ArrLen_init(&(instr2[0]), 13);

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
			free(lhs->arr);
			free(lhs);
			free(arrlen_instr1);
			free(rhs->arr);
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
			free(lhs->arr);
			free(lhs);
			free(arrlen_instr1);
			free(rhs->arr);
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

			byte *opcode = malloc(sizeof(char) * 5);

			// movl <value>, %eax
			opcode[0] = (byte) 0xB8;
			put_int_as_bytes(opcode, 1, expr->expr->intgr);

			return ArrLen_init(opcode, sizeof(char) * 5);
		}

		case expr_FNCall: {
			printf("Function call jit not yet implemented");
			exit(EXIT_FAILURE);
		}

		case expr_Ternary: {


			// Generate jitcode for the boolean, true, and false expressions
			ArrLen *b_exp = jitcode_expression(
				expr->expr->trnry->bool_expr, prog);
			ArrLen *t_exp = jitcode_expression(
				expr->expr->trnry->true_expr, prog);
			ArrLen *f_exp = jitcode_expression(
				expr->expr->trnry->false_expr, prog);

			/*
			 * The following byte arrays encode the machine code operations
			 * required to compute the given expression. The assembly code
			 * comments express the necessary jumps using labels. In reality,
			 * machine code has no labels, so the machine code shown actually
			 * encodes relative jumps, which trigger a jump over the specified
			 * number of bytes.
			 *     The 0xE9 opcode is an unconditional relative jump, that takes
			 * a long argument, for example, the instruction:
			 * { 0xE9, 0x09, 0x00, 0x00, 0x00 } will trigger a jump over the
			 * next 9 bytes of code. It is important the number of bytes to jump
			 * is exactly correct, or a misaligned read will likely occur, and
			 * cause a nonsense instruction to be executed. The information
			 * about how far to jump is contained in the ArrLen objects obtained
			 * from each expression.
			 *     The 0x0F, 0x84 opcode is the conditional 'if-equal' jump. It
			 * also takes a long argument.
			 */
			
			// b_exp goes here
			
			byte instr1[9] = {

				// cmpl $0, %eax
				0x83, 0xF8, 0x00,

				// je ternary_false
				0x0F, 0x84, 0x00, 0x00, 0x00, 0x00

			};
			
			// The 4 0x00s in the line 'je ternary_false' above are overwritten
			// with the length of the jump which is derived from the size of the
			// true expression. We must also jump over the 'jmp ternary_end'
			// below the true expession, hence the extra 5 bytes in the 
			// put_int_as_bytes() call.
			put_int_as_bytes(instr1, 5, t_exp->len + (sizeof(byte) * 5));
			ArrLen *arrlen_instr1 = ArrLen_init(&(instr1[0]), 9);
			
			// t_exp goes here
			
			// jmp ternary_end
			byte instr2[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };

			// As with the first jump, the appropriate jump distance is written
			// over the initial zero. However there is no extra code to jump, so
			// no 'sizeof(byte) * X' term.
			put_int_as_bytes(instr2, 1, f_exp->len);
			ArrLen *arrlen_instr2 = ArrLen_init(&(instr2[0]), 5);

			// ternary_false:
			// f_exp goes here

			// ternary_end:
			// No code required to represent this label as relative jumps are
			// used

			// Concatenate everything into one ArrLen object
			ArrLen *out = ArrLen_concat(5,
				b_exp,
				arrlen_instr1,
				t_exp,
				arrlen_instr2,
				f_exp
			);

			free(b_exp->arr);
			free(b_exp);
			free(arrlen_instr1);
			free(t_exp->arr);
			free(t_exp);
			free(arrlen_instr2);
			free(f_exp->arr);
			free(f_exp);

			return out;
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
	munmap(jit_memory, expr_code->len + sizeof(byte));

	return result;
}