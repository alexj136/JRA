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
static int label_print      = 0;
static int label_for        = 0;
static int label_while      = 0;
static int label_if         = 0;
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
			// str_concat
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
			// string by str_concat
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

			// Obtain a string that represents an integer which is the amount of
			// space which the arguments being passed to the function will
			// occupy on the stack
			char *stack_space = malloc(sizeof(char) * 12);
			sprintf(stack_space, "%d",
				4 * LinkedList_length(expr->expr->fncall->args));

			char *push_args = "";

			for(i = 0; i < LinkedList_length(expr->expr->fncall->args); i++) {
				char *arg = codegen_expression(
					(Expression *)LinkedList_get(expr->expr->fncall->args, i));
			}

			char *out = str_concat(20
				"subl $", stack_space, ", %esp\n",


				"pushl %ebp\n",
				"movl %esp, %ebp\n",
				push_args,
				"call ", expr->expr->fncall->name, "\n",
				"movl %ebp, %esp\n"
				"popl %ebp\n");

			free(push_args);

			return out;
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
 * Generate code for a given list of statements
 */
char *codegen_statement_list(LinkedList *stmts) {

	// If the list is empty, this is valid, but produces no output, so return
	// the empty string, rather than null
	if(LinkedList_length(stmts) == 0) {
		return "";
	}

	// Generate the code for the first expression
	char *out = codegen_statement((Statement *)LinkedList_get(stmts, 0));

	// Append the code for any subsequent expressions
	int i;
	for(i = 1; i < LinkedList_length(stmts); i++) {
		char *temp = codegen_statement((Statement *)LinkedList_get(stmts, i));
		char *temp2 = str_concat_2(out, temp);

		free(out);
		free(temp);

		out = temp2;
	}

	return out;
}

/*
 * Generate code for a given statement
 */
char *codegen_statement(Statement *stmt) {
	
	switch(stmt->type) {
		
		case stmt_For: {

		}
		
		case stmt_While: {

			// Get the label number for this if-statement
			char *label_no = label_number(&label_while);

			// Genrate code for the boolean expression, and the true & false
			// statement lists
			char *b_exp = codegen_expression(stmt->stmt->_while->bool_expr);
			char *stmts = codegen_statement_list(stmt->stmt->_while->stmts);

			char *out = str_concat(21,
				"# BEGIN WHILE STATEMENT ", label_no, "\n",

				"while_begin_", label_no, ":\n",

				// Evaluate the boolean expression and put the result in %eax
				b_exp,

				// Compare the boolean expression with 0
				"cmpl $0, %eax\n",

				// If the boolean is 0, i.e. if the statement is false, jump out
				// of the loop
				"je while_end_", label_no, "\n",

				// If the jump to the end of the loop was not taken, the boolean
				// expression was true, so execute the statement block
				t_stmts,

				// Then jump unconditionally back to the comparison
				"jmp while_begin", label_no, "\n",

				// Jump here after when the boolean evaluates to true, exiting
				// the loop
				"while_end_", label_no, ":\n",

				"# END WHILE STATEMENT ", label_no, "\n");

			free(b_exp);
			free(stmts);
			free(label_no);

			return out;
		}
		
		case stmt_If: {

			// Get the label number for this if-statement
			char *label_no = label_number(&label_if);

			// Genrate code for the boolean expression, and the true & false
			// statement lists
			char *b_exp = codegen_expression(stmt->stmt->_if->bool_expr);
			char *t_stmts = codegen_statement_list(
				stmt->stmt->_if->true_stmts);
			char *f_stmts = codegen_statement_list(
				stmt->stmt->_if->false_stmts);

			char *out = str_concat(22,
				"# BEGIN IF STATEMENT ", label_no, "\n",

				// Evaluate the boolean expression and put the result in %eax
				b_exp,

				// Compare the boolean expression with 0
				"cmpl $0, %eax\n",

				// If the boolean is 0, i.e. if the expression is false, jump to
				// the else statement label
				"je else_branch_", label_no, "\n",

				// If the jump to the else label was not taken, the expression
				// was true, so execute the true statement block
				t_stmts,

				// Then jump unconditionally over the else statements to the end
				// of the else statements
				"jmp if_end_", label_no, "\n",

				// Jump here when the statement is false
				"else_branch_", label_no, ":\n",

				// Evaluate the false statement list
				f_stmts,

				// Jump here after executing the true statements list, skipping
				// the false statement list
				"if_end_", label_no, ":\n",

				"# END IF STATEMENT ", label_no, "\n");

			free(b_exp);
			free(t_stmts);
			free(f_stmts);
			free(label_no);

			return out;
		}
		
		case stmt_Print: {
			
			// Get the label number for this print statement (only used in
			// generated comments, there are no jumps in print statements)
			char *label_no = label_number(&label_print);

			// Generate the code for the expression being printed
			char *expr = codegen_expression(stmt->stmt->_print->expr);

			// codegen_program() declares printf_str as "%d\n", which can be
			// used to print any integer in the manner below
			char *out = str_concat(10,
				"# BEGIN PRINT STATEMENT ", label_no, "\n",
				
				// Evaluate the expression and put is value in %eax
				expr,

				// Push %eax on the stack (second argument to printf())
				"pushl %eax\n",

				// Push the address of the format string on the stack(first
				// argument to printf())
				"pushl $printf_str\n",

				// And call printf
				"call printf\n",

				"# END PRINT STATEMENT ", label_no, "\n");

			free(expr);

			return out;
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
	Statement *stmt = Print_init(
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
	);
	printf("%s\n", codegen_statement(stmt));
	return 0;
}