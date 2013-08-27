#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../token.h"
#include "../AST.h"
#include "../parser.h"
#include "../codegen.h"

/*
 * Macro that writes the given string to a file with a given name
 */
#define WRITE(name, string) \
	do { \
		FILE *fp = fopen(name, "w"); \
		if(!fp) { \
			printf("Could not open file: '"#name"'\n"); \
			exit(EXIT_FAILURE); \
		} \
		fprintf(fp, "%s\n", string); \
		if(fclose(fp)) { \
			printf("Error closing file: '"#name"'\n"); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

/*
 * Macro that removes a file with the given name
 */
#define REMOVE(name) \
	do { \
		if(system("rm "#name"") == -1) { \
			printf("Failed to remove '%s'\n", name); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

/*
 * Function that builds a source file with the given name, and gives the output
 * file a separate name that is also given as a parameter
 */
void build(char *src, char *out) {
	
	char *command = (char *)safe_alloc(sizeof(char) * 100);
	sprintf(command, "gcc %s -o %s", src, out);
	
	int success = system(command) == 0;
	free(command);

	if(!success) {
		printf("Failed to build '%s' as '%s'\n", src, out);
		exit(EXIT_FAILURE);
	}
}

/*
 * Function that runs an executable of the given name and checks that the result
 * of its execution was appropriate. Since the function used to execute shell
 * commands returns -1 to indicate errors, the expected result cannot be -1, or
 * we would not know if -1 was returned as the correct output of the program or
 * if -1 was returned because an error occured
 */
bool checked_run(char *file, int expected_result) {
	if(expected_result == -1) {
		printf("Cannot execute '%s' with expected result of -1\n", file);
		exit(EXIT_FAILURE);
	}
	
	char *command = (char *)safe_alloc(sizeof(char) * 100);
	sprintf(command, "./%s", file);
	
	int command_result = system(command);
	if(command_result == -1) {
		printf("Warning: checked_run(%s, %d) call returned -1\n",
			file, expected_result);
	}

	free(command);
	return command_result == expected_result;
}

int tests_run = 0;

char *test_file_io() {

	WRITE("twelve.s", " .globl main \n main: \n movl $12, %eax \n ret \n");
	build("twelve.s", "twelve");
	int success = checked_run("twelve", 12);
	REMOVE("twelve");
	REMOVE("twelve.s");
	mu_assert(success, "test_file_io failed");

	return NULL;
}

char *test_ternary() {

	// AST for the large expression
	// (10 = 10) ? 5 : 1
	Expression *expr = Ternary_init(
		BooleanExpr_init(
			IntegerLiteral_init(10),
			EQUAL,
			IntegerLiteral_init(10)
		),
		IntegerLiteral_init(5),
		IntegerLiteral_init(1)
	);

	char *code = codegen_expression(expr, NULL);
	
	// Assert that code evaluates to  5, but how?

	free(code);
	Expression_free(expr);

	return NULL;
}

char *test_large_expression() {

	// AST for the large expression
	// (4 * 5) % (4 + (4 < 5 ? (10 != 9 ? 3 : 123) : 500)) should equal 6
	Expression *expr = ArithmeticExpr_init(
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
	);

	// int result;
	char *code = codegen_expression(expr, NULL);
	// asm(code
	// 	: "=r"(result)
	// 	:  // no input 
	// 	: "%eax", "%ebx", "%esp", "%ebp");

	// Assert that the result indeed equals 6
	// mu_assert(result == 6, "test_large_expression failed");

	free(code);
	Expression_free(expr);

	return NULL;
}

char *test_fibonacci() {
	LinkedList *tokens = lex("fn main() { return fibonacci(6); }\
	fn fibonacci(x) {\
		if x = 0 {\
			return 0;\
		}\
		else {} if x = 1 {\
			return 1;\
		}\
		else {\
			return fibonacci(x - 1) + fibonacci(x - 2);\
		}\
	}");

	Program *ast = parse_program(tokens);
	char *asm_prog = codegen_program(ast);
	
	WRITE("test/fib.s", asm_prog);
	build("test/fib.s", "test/fib");
	int success = checked_run("test/fib", 8);
	REMOVE("test/fib");
	REMOVE("test/fib.s");
	mu_assert(success, "test_fibonacci failed");

	free(asm_prog);
	Program_free(ast);
	LLMAP(tokens, Token *, Token_free);
	LinkedList_free(tokens);

	return NULL;
}

char *all_tests() {
	mu_run_test(test_file_io);
	// mu_run_test(test_ternary);
	// mu_run_test(test_large_expression);
	mu_run_test(test_fibonacci);

	return NULL;
}

RUN_TESTS(all_tests);