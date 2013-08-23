#include <stdio.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../token.h"
#include "../AST.h"
#include "../parser.h"
#include "../codegen.h"

int tests_run = 0;

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

char *all_tests() {
	// mu_run_test(test_ternary);
	// mu_run_test(test_large_expression);

	return NULL;
}

RUN_TESTS(all_tests);

/*
int main() {
	LinkedList *tokens = lex("fn main(x) { return fibonacci(x); }\
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
	// printf("%s\n", asm_prog);
	free(asm_prog);
	Program_free(ast);
	LLMAP(tokens, Token *, Token_free);
	LinkedList_free(tokens);
	return 0;
}
*/