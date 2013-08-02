#include <stdio.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../AST.h"
#include "../interpreter.h"

int tests_run = 0;

/*
 * Tests that Scope state stays correct after a proliferate and recede
 */
char *test_Scope() {

	Scope *scope = Scope_init(LinkedList_init(), LinkedList_init());

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

	return NULL;
}

char *all_tests() {

	mu_suite_start();
	
	mu_run_test(test_Scope);
	
	return NULL;
}

RUN_TESTS(all_tests);