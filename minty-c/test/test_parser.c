#include <stdio.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../lexer.h"
#include "../parser.h"
#include "../AST.h"

int tests_run = 0;

char *test_tiny_prog() {
	Program *parsed_prog = parse_program(lex("fn main() {return 0;}"));

	LinkedList *stmts = LinkedList_init();
	LinkedList_append(stmts, Return_init(IntegerLiteral_init(0)));
	
	LinkedList *fns = LinkedList_init();
	LinkedList_append(fns, FNDecl_init(safe_strdup("main"),
		LinkedList_init(), stmts));

	Program *ast = Program_init(fns);

	mu_assert(Program_equals(parsed_prog, ast), "test_tiny_prog failed!");

	return NULL;
}

char *test_func2() {

	return NULL;
}

char *all_tests() {

	mu_suite_start();
	
	mu_run_test(test_tiny_prog);
	mu_run_test(test_func2);

	return NULL;
}

RUN_TESTS(all_tests);