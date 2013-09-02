#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/wait.h>
#include "minunit.h"
#include "../minty_util.h"
#include "../token.h"
#include "../AST.h"
#include "../parser.h"
#include "../codegen.h"

/*
 * Macro that writes the given string to a file with a given name
 */
#define WRITE(name, string) \
	do { \
		FILE *fp = fopen(name, "w"); \
		if(!fp) { \
			printf("Could not open file: '"#name"'\n"); \
			exit(EXIT_FAILURE); \
		} \
		fprintf(fp, "%s\n", string); \
		if(fclose(fp)) { \
			printf("Error closing file: '"#name"'\n"); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

/*
 * Macro that removes a file with the given name
 */
#define REMOVE(name) \
	do { \
		if(system("rm "#name"") == -1) { \
			printf("Failed to remove '%s'\n", name); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

/*
 * Function that builds a source file with the given name, and gives the output
 * file a separate name that is also given as a parameter
 */
void build(char *src, char *out) {
	
	char *command = (char *)safe_alloc(sizeof(char) * 100);
	sprintf(command, "gcc %s -o %s", src, out);
	
	int success = system(command) == 0;
	free(command);

	if(!success) {
		printf("Failed to build '%s' as '%s'\n", src, out);
		exit(EXIT_FAILURE);
	}
}

/*
 * Function that runs an executable of the given name and checks that the result
 * of its execution was appropriate. Since the function used to execute shell
 * commands returns -1 to indicate errors, the expected result cannot be -1, or
 * we would not know if -1 was returned as the correct output of the program or
 * if -1 was returned because an error occured
 */
bool checked_run(char *file, int expected_result) {

	// Fork a child process
	pid_t child_pid = fork();
	int status = 0, exec_res = 0;

	// Child process
	if(child_pid == 0) {
		char *path = (char *)safe_alloc(sizeof(char) * 100);
		sprintf(path, "./%s", file);

		execl(path, "0", NULL);

		// If the exec fails, print a warning and exit the child process (does
		// not kill the parent)
		free(path);
		printf("Warning: execl() failed in checked_run(%s, %d)\n",
			file, expected_result);
		exit(EXIT_FAILURE);
	}

	// Parent Process
	else {
		// Wait for the child to finish. When the child finishes, its exit
		// status will be put into the status variable (hence the pass by
		// reference - there is no way for it to be passed to us by value)
		wait(&status);

		// If the process exited successfully, get its exit code 
		if(WIFEXITED(status)) exec_res = WEXITSTATUS(status);
		else {
			// Otherwise print an error message and return false
			printf("execl() did not return a valid exit "
				"code in checked_run(%s, %d)\n", file, expected_result);
			return false;
		}
	}

	// If the exit code matched our expected result, return true, otherwise
	// return false
	return exec_res == expected_result;
}

int tests_run = 0;

/*
 * Tests that the file io concept for the tests in this file work correctly.
 * writes a small assembly program to disk, assembles it, runs it, and checks
 * that it gives the correct result. The files (source/binary) are then removed.
 */
char *test_file_io() {

	char *prog =
		".globl main"    "\n"
		"main:"          "\n"
		"movl $1, %eax"  "\n"
		"movl $12, %ebx" "\n"
		"int $0x80"      "\n";

	WRITE("test/twelve.s", prog);

	build("test/twelve.s", "test/twelve");
	bool success = checked_run("test/twelve", 12);
	REMOVE("test/twelve");
	REMOVE("test/twelve.s");
	mu_assert(success, "test_file_io failed");

	return NULL;
}

/*
 * Test that ternary expressions give the correct result
 */
char *test_ternary() {

	// AST for the large expression
	// (10 = 10) ? 5 : 1
	Expression *expr = Ternary_init(
		BooleanExpr_init(
			IntegerLiteral_init(10),
			EQUAL,
			IntegerLiteral_init(10)
		),
		IntegerLiteral_init(5),
		IntegerLiteral_init(1)
	);

	// Generate assembly code to 'call' the ternary expression
	char *code = codegen_expression(expr, NULL);
	char *testable_code = str_concat(3,
		".globl main\n"
		"main:\n",
		code,
		"movl %eax, %ebx\n"
		"movl $1, %eax\n"
		"int $0x80\n"
	);
	
	// Write the assembly file to disk, assemble it, test that it gives the
	// correct result, and remove the files
	WRITE("test/tern.s", testable_code);
	build("test/tern.s", "test/tern");
	bool success = checked_run("test/tern", 5);
	REMOVE("test/tern");
	REMOVE("test/tern.s");

	// Free things
	free(code);
	free(testable_code);
	Expression_free(expr);

	// Assert that the program gave the correct result
	mu_assert(success, "test_ternary failed")

	return NULL;
}

char *test_large_expression() {

	int *randoms = (int *)malloc(sizeof(int) * 10);

	srand(time(NULL));
	int j;
	for(j = 0; j < 10; j++) {
		randoms[j] = (rand() % 1000) - (rand() % 300);
		if(randoms[j] == 0) randoms[j] = 1;
	}

	int expected_value = (randoms[0] * randoms[1]) % (randoms[2] +
			((randoms[3] < randoms[4])?
				((randoms[5] !=	randoms[6])?
					randoms[7] :
					randoms[8]) :
				randoms[9]));

	char *src = (char *)malloc(sizeof(char) * 200);
	sprintf(src, "(%d*%d)%%(%d + ( (%d<%d) ? ( (%d!=%d)?%d:%d) : %d) );",
		randoms[0], randoms[1], randoms[2], randoms[3], randoms[4],
		randoms[5], randoms[6], randoms[7], randoms[8], randoms[9]
	);

	LinkedList *tokens = lex(src);
	Expression *expr = parse_expression(tokens);

	// Generate the code
	char *expr_code = codegen_expression(expr, NULL);
	char *testable_code = str_concat(3,
		".globl main\n"
		"main:\n",
		expr_code,
		"movl %eax, %ebx\n"
		"movl $1, %eax\n"
		"int $0x80\n"
	);

	// Test the code in the way used previously
	WRITE("test/large_expr.s", testable_code);
	build("test/large_expr.s", "test/large_expr");
	bool success = checked_run("test/large_expr", expected_value);
	REMOVE("test/large_expr");
	REMOVE("test/large_expr.s");

	// Free stuff
	free(src);
	LLMAP(tokens, Token *, Token_free);
	LinkedList_free(tokens);
	free(expr_code);
	free(testable_code);
	free(randoms);
	Expression_free(expr);

	// Assert that the test succeeded
	mu_assert(success, "test_large_expression failed");

	return NULL;
}

char *test_fibonacci() {
	LinkedList *tokens = lex(
		"fn fibonacci(x) {"                                  "\n"
		"	if x = 0 {"                                      "\n"
		"		return 0;"                                   "\n"
		"	}"                                               "\n"
		"	else {} if x = 1 {"                              "\n"
		"		return 1;"                                   "\n"
		"	}"                                               "\n"
		"	else {"                                          "\n"
		"		return fibonacci(x - 1) + fibonacci(x - 2);" "\n"
		"	}"                                               "\n"
		"}"
	);

	FNDecl *fn = parse_function(tokens);
	Program *prog = Program_init(LinkedList_init_with(fn));
	Program_generate_offsets(prog);
	char *asm_function = codegen_function(fn, prog);
	
	char *asm_prog = str_concat_2(
		
		".text\n"
		
		"printf_str: .asciz \"%d\"\n"
		"error_str: .asciz \"Error: reached end of function without return\"\n"
		
		".globl main\n"
		"main:\n"
			
			// Set up stack to call fibonacci
			"movl $6, %eax\n"
			"pushl %eax\n"
			"addl $4, %esp\n"
			"movl %esp, %ebp\n"

			"call fibonacci\n"
			
			// Exit system call with result as exit status
			"movl %eax, %ebx\n"
			"movl $1, %eax\n"
			"int $0x80\n",

		asm_function
	);

	WRITE("test/fib.s", asm_prog);
	build("test/fib.s", "test/fib");
	int success = checked_run("test/fib", 8);
	REMOVE("test/fib");
	REMOVE("test/fib.s");

	free(asm_prog);
	free(asm_function);
	Program_free(prog);
	LLMAP(tokens, Token *, Token_free);
	LinkedList_free(tokens);

	mu_assert(success, "test_fibonacci failed");
	
	return NULL;
}

char *all_tests() {
	mu_run_test(test_file_io);
	mu_run_test(test_ternary);
	mu_run_test(test_large_expression);
	mu_run_test(test_fibonacci);

	return NULL;
}

RUN_TESTS(all_tests);
