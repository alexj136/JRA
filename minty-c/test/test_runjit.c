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

char *test_jit_nothing() {

	// Empty ArrLen object
	ArrLen *jitcode = ArrLen_init(NULL, 0);

	// Run the empty machine code. No assertions to make, but simply checking
	// that we can jump to some mmaped region and return without causing a
	// segfault.
	int i; for(i = 0; i < 10; i++) jitexec_expression(jitcode);

	free(jitcode);

	return NULL;
}

char *test_jit_integer() {
	int i;
	for(i = -2000000000; i < 2000000000; i += 10000000) {

		// Expression to jit
		Expression *integer = IntegerLiteral_init(i);

		// Convert it to machine code
		ArrLen *jitcode = jitcode_expression(integer, NULL);

		// Run the machine code
		int result = jitexec_expression(jitcode);

		// Assert the correct result
		mu_assert(result == i, "test_jit_integer failed");

		Expression_free(integer);
		free(jitcode->arr);
		free(jitcode);
	}

	return NULL;
}

char *test_jit_add() {
	// Create an arithmetic expression
	Expression *one_plus_two = ArithmeticExpr_init(
		IntegerLiteral_init(1),
		PLUS,
		IntegerLiteral_init(2)
	);

	// Get its jit code
	ArrLen *jitcode = jitcode_expression(one_plus_two, NULL);
	
	// Execute its jit code and store the result
	int result = jitexec_expression(jitcode);

	// Assert that the correct result was obtained
	mu_assert(result == 3, "test_jit_add failed");

	Expression_free(one_plus_two);
	free(jitcode->arr);
	free(jitcode);

	return NULL;
}

char *all_tests() {

	mu_run_test(test_ArrLen_concat_2);
	mu_run_test(test_ArrLen_concat);
	mu_run_test(test_jit_nothing);
	mu_run_test(test_jit_integer);
	mu_run_test(test_jit_add);

	return NULL;
}

RUN_TESTS(all_tests);