#include <stdio.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../token.h"
#include "../AST.h"
#include "../codegen.h"

int tests_run = 0;

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

	int result;
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

	mu_run_test(test_large_expression);

	return NULL;
}

RUN_TESTS(all_tests);