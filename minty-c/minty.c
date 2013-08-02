#include <stdio.h>
#include <malloc.h>
#include "minty_util.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "interpreter.h"

int main() {
	
	Program *prog  = parse_program(lex("fn main(){return 1 + 2;}"));
	printf("%d\n", interpret_program(prog, LinkedList_init()));

	return 0;
}