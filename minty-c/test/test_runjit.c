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

	free(a->arr);
	free(a);
	free(b->arr);
	free(b);

	return NULL;
}

char *test_jit_nothing() {

	// Empty ArrLen object
	ArrLen *jitcode = ArrLen_init(NULL, 0);

	// Run the empty machine code. No assertions to make, but simply checking
	// that we can jump to some mmap-ed region and return without causing a
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

	int i, j;

	for(i = -6378623; i < 3827493; i += 20394)
	for(j = -4839723; j < 8438758; j += 78347) {

		// Create an arithmetic expression
		Expression *one_plus_two = ArithmeticExpr_init(
			IntegerLiteral_init(i),
			PLUS,
			IntegerLiteral_init(j)
		);

		// Get its jit code
		ArrLen *jitcode = jitcode_expression(one_plus_two, NULL);

		// Execute its jit code and store the result
		int result = jitexec_expression(jitcode);

		// Assert that the correct result was obtained
		mu_assert(result == i + j, "test_jit_add failed");

		Expression_free(one_plus_two);
		free(jitcode->arr);
		free(jitcode);
	}
	return NULL;
}

char *test_jit_boolean() {

	int i;
	for(i = 0; i < 10000; i++) {
		
		int a = rand() % 100;
		int b = rand() % 100;

		// Create an ternary expression
		Expression *blean = BooleanExpr_init(
			IntegerLiteral_init(a),
			GREATER_THAN,
			IntegerLiteral_init(b)
		);

		// Get its jit code
		ArrLen *jitcode = jitcode_expression(blean, NULL);

		// Execute its jit code and store the result
		int result = jitexec_expression(jitcode);

		// Assert that the correct result was obtained
		mu_assert(result == (a > b), "test_jit_boolean failed");

		Expression_free(blean);
		free(jitcode->arr);
		free(jitcode);
	}
	return NULL;
}

char *test_jit_ternary() {

	int i;
	for(i = 0; i < 10000; i++) {
		
		// Random parameters for use in ternary expressions
		int a = rand() % ((i * 200000) + 1);
		int b = rand() % ((i * 200000) + 1);
		int c = rand() % ((i * 200000) + 1);
		int d = rand() % ((i * 200000) + 1);

		// Create an ternary expression
		Expression *tern = Ternary_init(
			BooleanExpr_init(
				IntegerLiteral_init(a),
				GREATER_THAN,
				IntegerLiteral_init(b)
			),
			IntegerLiteral_init(c),
			IntegerLiteral_init(d)
		);

		// Get its jit code
		ArrLen *jitcode = jitcode_expression(tern, NULL);

		// Execute its jit code and store the result
		int result = jitexec_expression(jitcode);

		// Assert that the correct result was obtained
		mu_assert(result == ((a > b)? c : d), "test_jit_ternary failed");

		Expression_free(tern);
		free(jitcode->arr);
		free(jitcode);
	}
	return NULL;
}

char *all_tests() {

	mu_run_test(test_ArrLen_concat_2);
	mu_run_test(test_ArrLen_concat);
	mu_run_test(test_jit_nothing);
	mu_run_test(test_jit_integer);
	mu_run_test(test_jit_add);
	mu_run_test(test_jit_boolean);
	mu_run_test(test_jit_ternary);

	return NULL;
}

RUN_TESTS(all_tests);