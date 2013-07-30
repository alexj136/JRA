#include <stdio.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"

int tests_run = 0;

char *test_LinkedList_remove() {

	// Test goes here

	return NULL;
}

char *all_tests() {
	mu_suite_start();
	
	mu_run_test(test_LinkedList_remove);
	return NULL;
}

RUN_TESTS(all_tests);