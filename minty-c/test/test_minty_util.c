#include <stdio.h>
#include <malloc.h>
#include "minunit.h"
#include "../minty_util.h"

int tests_run = 0;

char *test_str_concat() {

	// Try some example concatenations and assert that they produce the correct
	// string as output

	char *str = str_concat(0, "Tee Hee");
	mu_assert(str == NULL, "str_concat returned incorrect string");

	str = str_concat(1, "Hello!");
	mu_assert(str_equal(str, "Hello!"), "str_concat returned incorrect string");
	free(str);

	str = str_concat(2, "Hello!", " How are you?");
	mu_assert(str_equal(str, "Hello! How are you?"),
		"str_concat returned incorrect string");
	free(str);

	str = str_concat(3, "Hello!", " How are you?", " You look well!");
	mu_assert(str_equal(str, "Hello! How are you? You look well!"),
		"str_concat returned incorrect string");
	free(str);

	str = str_concat(4, "YOU", "JUST", "WATCH", "YOURSELF");
	mu_assert(str_equal(str, "YOUJUSTWATCHYOURSELF"),
		"str_concat returned incorrect string");
	free(str);

	str = str_concat(5, "YOU ", "JUST ", "WATCH ", "YOURSELF ", "OKAY");
	mu_assert(str_equal(str, "YOU JUST WATCH YOURSELF OKAY"),
		"str_concat returned incorrect string");
	free(str);

	str = str_concat(6, "YOU ", "JUST ", "WATCH ", "YOUR ", "BACK ", "OKAY");
	mu_assert(str_equal(str, "YOU JUST WATCH YOUR BACK OKAY"),
		"str_concat returned incorrect string");
	free(str);

	return NULL;
}

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
	
	mu_run_test(test_str_concat);
	mu_run_test(test_LinkedList_remove);

	return NULL;
}

RUN_TESTS(all_tests);