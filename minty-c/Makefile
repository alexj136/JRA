# Commands for compiling/linking
COMPILER = gcc
LINK = $(COMPILER) -Wall -g
COMPILE = $(COMPILER) -Wall -g -c

# File lists
SOURCES = minty_util.c token.c lexer.c AST.c parser.c interpreter.c minty.c
TESTSRC = test/test_parser.c test/test_minty_util.c

OBJECTS = minty_util.o token.o lexer.o AST.o parser.o interpreter.o
TESTS = test/test_parser test/test_minty_util test/test_interpreter
OUTPUTS = $(OBJECTS) $(TESTS) minty

# Adding this line means you can just run 'make' and everything than needs
# building will be built
all: $(OUTPUTS)
	@echo BUILD SUCCESSFUL

# Command that is run when 'make clean' is run
clean:
	rm -f $(OUTPUTS)

test: test/test_minty_util test/test_parser test/test_interpreter
	test/test_minty_util
	test/test_parser
	test/test_interpreter

# Final compilation & linkage:
minty: $(OBJECTS) minty.c
	$(LINK) $(OBJECTS) minty.c -o minty

# Compile sources:
minty_util.o: minty_util.c
	$(COMPILE) minty_util.c -o minty_util.o

token.o: token.c
	$(COMPILE) token.c -o token.o

lexer.o: lexer.c
	$(COMPILE) lexer.c -o lexer.o

AST.o: AST.c 
	$(COMPILE) AST.c -o AST.o

parser.o: parser.c
	$(COMPILE) parser.c -o parser.o

interpreter.o: interpreter.c
	$(COMPILE) interpreter.c -o interpreter.o

# Compile, link & run tests:
test/test_minty_util: test/test_minty_util.c
	$(LINK) test/test_minty_util.c minty_util.o -o test/test_minty_util
	test/test_minty_util

test/test_parser: test/test_parser.c minty_util.o token.o lexer.o AST.o parser.o
	$(LINK) test/test_parser.c minty_util.o token.o lexer.o AST.o parser.o \
		-o test/test_parser

test/test_interpreter: test/test_interpreter.c minty_util.o token.o AST.o \
	interpreter.o
	$(LINK) test/test_interpreter.c minty_util.o token.o AST.o interpreter.o \
		-o test/test_interpreter