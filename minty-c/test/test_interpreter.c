#include <stdio.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../lexer.h"
#include "../AST.h"
#include "../parser.h"
#include "../interpreter.h"

int tests_run = 0;

/*
 * Tests that Scope state stays correct after a proliferate and recede
 */
char *test_Scope() {

	LinkedList *no_names = LinkedList_init();
	LinkedList *no_values = LinkedList_init();
	Scope *scope = Scope_init(no_names, no_values);

	// Add some variables
	Scope_update(scope, safe_strdup("x"), 1);
	Scope_update(scope, safe_strdup("y"), 2);
	Scope_update(scope, safe_strdup("z"), 3);

	// Check those variables were added correctly
	mu_assert(Scope_get(scope, "x") == 1, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "y") == 2, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "z") == 3, "Scope retrieval failed!");

	Scope_proliferate(scope);

	// Add a new variable at the proliferated scope
	Scope_update(scope, safe_strdup("a"), 0);

	// Test that all variables are accessible
	mu_assert(Scope_get(scope, "a") == 0, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "x") == 1, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "y") == 2, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "z") == 3, "Scope retrieval failed!");

	Scope_recede(scope);

	// Variable a should no longer be accessible after recession
	mu_assert(!Scope_has(scope, "a"), "Scope recession failed!");

	// But x, y and z should be
	mu_assert(Scope_get(scope, "x") == 1, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "y") == 2, "Scope retrieval failed!");
	mu_assert(Scope_get(scope, "z") == 3, "Scope retrieval failed!");

	Scope_recede(scope);

	// After the further recession, none of the variables should be accessible
	mu_assert(!Scope_has(scope, "a"), "Scope recession failed!");
	mu_assert(!Scope_has(scope, "x"), "Scope recession failed!");
	mu_assert(!Scope_has(scope, "y"), "Scope recession failed!");
	mu_assert(!Scope_has(scope, "z"), "Scope recession failed!");

	Scope_free(scope);
	free(no_names);
	free(no_values);

	return NULL;
}

char *test_hundred() {

	LinkedList *prog_tokens = lex("\
		fn main() {                \
			return hundred();      \
		}                          \
		fn hundred() {             \
			return 100;            \
		}");
	Program *prog = parse_program(prog_tokens);
	LinkedList *args = LinkedList_init();

	mu_assert(interpret_program(prog, args) == 100, "test_hundred failed!");

	// Free things
	int i;
	for(i = 0; i < LinkedList_length(prog_tokens); i++)
		Token_free(LinkedList_get(prog_tokens, i));
	LinkedList_free(args);
	LinkedList_free(prog_tokens);
	Program_free(prog);

	return NULL;
}

char *test_binary_add() {

	LinkedList *prog_tokens = lex("  \
		fn main() {                  \
			return binary_add(4, 5); \
		}                            \
		fn binary_add(num1, num2) {  \
			return num1 + num2;      \
		}");
	Program *prog = parse_program(prog_tokens);
	LinkedList *args = LinkedList_init();

	mu_assert(interpret_program(prog, args) == 9, "test_binary_add failed!");

	// Free things
	int i;
	for(i = 0; i < LinkedList_length(prog_tokens); i++)
		Token_free(LinkedList_get(prog_tokens, i));
	LinkedList_free(args);
	LinkedList_free(prog_tokens);
	Program_free(prog);

	return NULL;
}

char *test_print() {

	LinkedList *prog_tokens = lex("\
		fn main(num) {             \
			print num;             \
			return num + 1;        \
		}");
	Program *prog = parse_program(prog_tokens);
	LinkedList *args = LinkedList_init_with((void *) 1);

	printf("PRINT TEST SHOULD PRINT '1' HERE: ");
	mu_assert(interpret_program(prog, args) == 2, "test_print failed!");

	// Free things
	int i;
	for(i = 0; i < LinkedList_length(prog_tokens); i++)
		Token_free(LinkedList_get(prog_tokens, i));
	LinkedList_free(args);
	LinkedList_free(prog_tokens);
	Program_free(prog);

	return NULL;
}

char *test_fibonacci() {

	LinkedList *prog_tokens = lex("                         \
		fn main(x) { return fibonacci(x); }                 \
		fn fibonacci(x) {                                   \
			if x = 0 {                                      \
				return 0;                                   \
			}                                               \
			else {} if x = 1 {                              \
				return 1;                                   \
			}                                               \
			else {                                          \
				return fibonacci(x - 1) + fibonacci(x - 2); \
			}                                               \
		}");
	Program *prog = parse_program(prog_tokens);
	LinkedList *arg0 = LinkedList_init_with((void *) 0);
	LinkedList *arg1 = LinkedList_init_with((void *) 1);
	LinkedList *arg2 = LinkedList_init_with((void *) 2);
	LinkedList *arg3 = LinkedList_init_with((void *) 3);
	LinkedList *arg4 = LinkedList_init_with((void *) 4);
	LinkedList *arg5 = LinkedList_init_with((void *) 5);
	LinkedList *arg20 = LinkedList_init_with((void *) 20);

	mu_assert(interpret_program(prog, arg0) == 0, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg1) == 1, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg2) == 1, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg3) == 2, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg4) == 3, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg5) == 5, "test_fibonacci failed!");
	mu_assert(interpret_program(prog, arg20) == 6765, "test_fibonacci failed!");

	// Free things
	int i;
	for(i = 0; i < LinkedList_length(prog_tokens); i++)
		Token_free(LinkedList_get(prog_tokens, i));
	LinkedList_free(prog_tokens);
	LinkedList_free(arg0);
	LinkedList_free(arg1);
	LinkedList_free(arg2);
	LinkedList_free(arg3);
	LinkedList_free(arg4);
	LinkedList_free(arg5);
	LinkedList_free(arg20);
	Program_free(prog);

	return NULL;
}

char *test_while() {

	LinkedList *prog_tokens = lex("\
		fn main() {                \
			i <- 0;                \
			while i < 10 {         \
				i++;               \
			}                      \
			return i;              \
		}");
	Program *prog = parse_program(prog_tokens);
	LinkedList *args = LinkedList_init();

	mu_assert(interpret_program(prog, args) == 10, "test_while failed!");

	// Free things
	int i;
	for(i = 0; i < LinkedList_length(prog_tokens); i++)
		Token_free(LinkedList_get(prog_tokens, i));
	LinkedList_free(args);
	LinkedList_free(prog_tokens);
	Program_free(prog);

	return NULL;
}

char *all_tests() {
	
	mu_run_test(test_Scope);
	mu_run_test(test_hundred);
	mu_run_test(test_binary_add);
	mu_run_test(test_print);
	mu_run_test(test_fibonacci);
	mu_run_test(test_while);
	
	return NULL;
}

RUN_TESTS(all_tests);