#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minty_util.h"
#include "token.h"
#include "AST.h"

/*
 * Macros for boolean operations
 */

// Equal: if the values aren't equal, overwrite the placed one with a zero
#define EQU "cmove %edx, %ecx\n"

// Not equal: if the values are equal, overwrite the placed one with a zero
#define NEQ "cmovne %edx, %ecx\n"

// Less than: if lhs isn't less than rhs, overwrite the placed one with a zero
#define LSS "cmovl %edx, %ecx\n"

// Less or equal: if lhs isn't less or equal to rhs, overwrite the placed one
// with a zero
#define LSE "cmovle %edx, %ecx\n"

// Greater: if lhs isn't greater than rhs, overwrite the placed one with a zero
#define GTR "cmovg %edx, %ecx\n"

// Greater or equal: if lhs isn't greater or equal to rhs, overwrite the placed
// one with a zero
#define GTE "cmovge %edx, %ecx\n"

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

/*
 * The following code provides a solution to the problem of naming labels
 * without duplicate names. To use, call label_number() with the ADDRESS OF one
 * of the global variables below. If it is passed by value, not by reference, it
 * will not be incremented and label clashes will occur.
 */

/*
 * Given a label type, returns a string that can be used with a label to keep
 * labels used more than once unique
 */
static char *label_number(int *label) {
	// Allocate space for the number string
	char *str = safe_alloc(sizeof(char) * 20);

	// Generate the number string from the current number
	sprintf(str, "%d", *label);

	// Increment the label number to prevent clashes
	(*label)++;

	// Return the generated string
	return str;
}

static int label_ternary    = 0;
static int label_arithmetic = 0;
static int label_boolean    = 0;
static int label_if         = 0;
static int label_else       = 0;
static int label_while      = 0;
static int label_for        = 0;
static int label_etcetera   = 0;

/*
 * Generate code for a given expression
 */
char *codegen_expression(Expression *expr) {

	switch(expr->type) {

		case expr_BooleanExpr: {

			// Get the label number for this boolean expression (only used in
			// generated comments, there are no jumps in boolean expressions)
			char *label_no = label_number(&label_boolean);

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
			out = str_concat(11,
				"# BEGIN BOOLEAN EXPRESSION ", label_no, "\n",
				rhs,
				"pushl %eax\n",
				lhs,
				"popl %ebx\n",
				"movl $0, %ecx\n",
				"movl $1, %edx\n",
				"cmpl %ebx, %eax\n",
				opcode,
				"movl %ecx, %eax\n",
				"# END BOOLEAN EXPRESSION ", label_no, "\n");

			// We can free lhs and rhs as their contents are copied into out by
			// str_concat_five
			free(lhs);
			free(rhs);
			free(label_no);

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

			// Get the label number for this arithmetic expression (only used in
			// generated comments, there are no jumps in arithmetic expressions)
			char *label_no = label_number(&label_arithmetic);

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
			out = str_concat(11,
				"# BEGIN ARITHMETIC EXPRESSION ", label_no, "\n",
				rhs,
				"pushl %eax\n",
				lhs,
				"popl %ebx\n",
				opcode,
				"# END ARITHMETIC EXPRESSION ", label_no, "\n");

			// We can free lhs and rhs as their contents are copied into the out
			// string by str_concat_five
			free(lhs);
			free(rhs);
			free(label_no);

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
			
			// Generate a label number for the jumps and comments
			char *label_no = label_number(&label_ternary);

			// Codegen each sub-expression
			char *b_exp = codegen_expression(expr->expr->trnry->bool_expr);
			char *t_exp = codegen_expression(expr->expr->trnry->true_expr);
			char *f_exp = codegen_expression(expr->expr->trnry->false_expr);

			char *out = str_concat(22,
				"# BEGIN TERNARY EXPRESSION ", label_no, "\n",
				b_exp,
				"cmpl $0, %eax\n",
				"je ternary_false_", label_no, "\n",
				t_exp,
				"jmp ternary_end_", label_no, "\n",
				"ternary_false_", label_no, ":\n",
				f_exp,
				"ternary_end_", label_no, ":\n",
				"# END TERNARY EXPRESSION ", label_no, "\n");

			free(b_exp);
			free(t_exp);
			free(f_exp);
			free(label_no);

			return out;
		}
	}
	printf("Invalid expression type in AST\n");
	exit(EXIT_FAILURE);
	return NULL;
}

/*
 * Generate code for a given statement
 */
char *codegen_statement(Statement *stmt) {
	
	switch(stmt->type) {
		
		case stmt_For: {

		}
		
		case stmt_While: {

		}
		
		case stmt_If: {

		}
		
		case stmt_Print: {

		}
		
		case stmt_Assignment: {

		}
		
		case stmt_Return: {

		}
	}
	printf("Invalid statement type in AST\n");
	exit(EXIT_FAILURE);
	return NULL;
}

int main() {
	// (4 * 5) % (4 + (4 < 5 ? (10 != 9 ? 3 : 123) : 500)) should equal 6
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
					Ternary_init(
						BooleanExpr_init(
							IntegerLiteral_init(4),
							LESS_THAN,
							IntegerLiteral_init(5)
						),
						Ternary_init(
							BooleanExpr_init(
								IntegerLiteral_init(10),
								NOT_EQUAL,
								IntegerLiteral_init(9)
							),
							IntegerLiteral_init(3),
							IntegerLiteral_init(123)
						),
						IntegerLiteral_init(500)
					)
				)
			)
		)
	);
	return 0;
}