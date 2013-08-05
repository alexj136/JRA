/* file: minunit.h */
#ifndef MINUNIT
#define MINUNIT

#include <stdio.h>

#define mu_assert(test, message) \
	if (!(test)) { \
		return message; \
	}

#define mu_run_test(test) \
	do {\
		char *message = test(); \
		tests_run++; \
		if (message) return message;\
	} while (0)

#define RUN_TESTS(tests) \
int main() { \
	char *result = tests(); \
	if (result != NULL) { \
		printf("%s\n", result); \
	} \
	else { \
		printf("ALL TESTS PASSED\n"); \
	} \
	printf("Tests run: %d\n", tests_run); \
	return result != NULL; \
}

extern int tests_run;

#endif // MINUNIT