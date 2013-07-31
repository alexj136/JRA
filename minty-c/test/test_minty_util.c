#include <stdio.h>
#include "minunit.h"
#include "../minty_util.h"

int tests_run = 0;

char *test_LinkedList_remove() {

	// Create a LinkedList storing integers 0 - 9
	LinkedList *ll = LinkedList_init();
	int i;
	for(i = 0; i < 10; i++) {
		LinkedList_append(ll, (void *)i);
	}

	// Assert that the list has length 10
	mu_assert(LinkedList_length(ll) == 10, "LinkedList length incorrect");

	// Remove the first element, and assert that the list has integers 1 - 9 and
	// length 9
	LinkedList_remove(ll, 0);
	int expected_list_1[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	for(i = 0; i < LinkedList_length(ll); i++) {
		mu_assert(((int)LinkedList_get(ll, i)) == expected_list_1[i],
			"Bad LinkedList removal");
	}
	mu_assert(LinkedList_length(ll) == 9, "LinkedList length incorrect");

	// Remove the second element (the two, at index 1) and assert things are
	// still in order
	LinkedList_remove(ll, 1);
	int expected_list_2[8] = {1, 3, 4, 5, 6, 7, 8, 9};
	for(i = 0; i < LinkedList_length(ll); i++) {
		mu_assert(((int)LinkedList_get(ll, i)) == expected_list_2[i],
			"Bad LinkedList removal");
	}
	mu_assert(LinkedList_length(ll) == 8, "LinkedList length incorrect");

	// Do the same thing with the last element
	LinkedList_remove(ll, 7);
	int expected_list_3[7] = {1, 3, 4, 5, 6, 7, 8};
	for(i = 0; i < LinkedList_length(ll); i++) {
		mu_assert(((int)LinkedList_get(ll, i)) == expected_list_3[i],
			"Bad LinkedList removal");
	}
	mu_assert(LinkedList_length(ll) == 7, "LinkedList length incorrect");

	// And again with the penultimate element
	LinkedList_remove(ll, 5);
	int expected_list_4[7] = {1, 3, 4, 5, 6, 8};
	for(i = 0; i < LinkedList_length(ll); i++) {
		mu_assert(((int)LinkedList_get(ll, i)) == expected_list_4[i],
			"Bad LinkedList removal");
	}
	mu_assert(LinkedList_length(ll) == 6, "LinkedList length incorrect");

	LinkedList_free(ll);

	return NULL;
}

char *all_tests() {

	mu_suite_start();
	
	mu_run_test(test_LinkedList_remove);

	return NULL;
}

RUN_TESTS(all_tests);