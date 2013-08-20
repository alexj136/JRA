#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../AST.h"
#include "../runjit.h"

int tests_run = 0;

char *test_ArrLen_concat_2() {

	// Create a couple of ArrLen objects
	byte mem[] = { 0x40, 0x10, 0xD4, 0xAA };
	byte *mmem = malloc(sizeof(char) * 4);
	memcpy(mmem, mem, sizeof(char) * 4);
	ArrLen *a = ArrLen_init(mmem, sizeof(char) * 4);

	byte mem2[] = { 0x43, 0x20, 0xB4, 0xCA };
	byte *mmem2 = malloc(sizeof(char) * 4);
	memcpy(mmem2, mem2, sizeof(char) * 4);
	ArrLen *b = ArrLen_init(mmem2, sizeof(char) * 4);

	// Concatenate them to create a third
	ArrLen *c = ArrLen_concat_2(a, b);

	// Check that the third has the appropriate values
	int i;
	for(i = 0; i < a->len; i++) {
		mu_assert(*((c->arr) + i) == mem[i],
			"test_ArrLen_concat_2 failed");
	}
	for(i = 0; i < b->len; i++) {
		mu_assert(*((c->arr) + i + a->len) == mem2[i],
			"test_ArrLen_concat_2 failed");
	}
	
	// Check that the third is the correct length
	mu_assert(c->len == 8, "test_ArrLen_concat_2 failed");

	free(a->arr);
	free(a);
	free(b->arr);
	free(b);
	free(c->arr);
	free(c);

	return NULL;
}


char *test_ArrLen_concat() {

	// Create an ArrLen object
	byte mem[] = { 0x40, 0x10, 0xD4, 0xAA };
	byte *mmem = malloc(sizeof(char) * 4);
	memcpy(mmem, mem, sizeof(char) * 4);
	ArrLen *a = ArrLen_init(mmem, sizeof(char) * 4);

	// Concatenate it to itself 10 times
	ArrLen *b = ArrLen_concat(10, a, a, a, a, a, a, a, a, a, a);

	int i, j, bpos = 0;
	for(i = 0; i < 10; i++) {
		for(j = 0; j < a->len; j++) {
			mu_assert((b->arr)[bpos] == mem[j],
				"test_ArrLen_concat failed");
			bpos++;
		}
	}

	return NULL;
}

char *all_tests() {

	mu_run_test(test_ArrLen_concat_2);
	mu_run_test(test_ArrLen_concat);

	return NULL;
}

RUN_TESTS(all_tests);