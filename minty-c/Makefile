# Commands for compiling/linking
COMPILER = gcc
LINK = $(COMPILER) -Wall -g
COMPILE = $(COMPILER) -Wall -g -c

# File lists
SOURCES = minty_util.c token.c lexer.c AST.c parser.c interpreter.c codegen.c \
	minty.c
TESTSRC = test/test_parser.c test/test_minty_util.c test/test_interpreter.c \
	test/test_codegen.c

OBJECTS = minty_util.o token.o lexer.o AST.o parser.o interpreter.o codegen.o
TESTS = test/test_parser test/test_minty_util test/test_interpreter \
	test/test_codegen
OUTPUTS = $(OBJECTS) $(TESTS) minty

# Adding this line means you can just run 'make' and everything than needs
# building will be built
all: $(OUTPUTS)
	@echo BUILD SUCCESSFUL

# Command that is run when 'make clean' is run
clean:
	rm -f $(OUTPUTS)

test: $(TESTS)
	test/test_minty_util
	test/test_parser
	test/test_interpreter
	test/test_codegen

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

codegen.o: codegen.c
	$(COMPILE) codegen.c -o codegen.o

# Compile, link & run tests:
test/test_minty_util: test/test_minty_util.c
	$(LINK) test/test_minty_util.c minty_util.o -o test/test_minty_util
	@test/test_minty_util

test/test_parser: test/test_parser.c minty_util.o token.o lexer.o AST.o parser.o
	$(LINK) test/test_parser.c minty_util.o token.o lexer.o AST.o parser.o \
		-o test/test_parser
	@test/test_parser

test/test_interpreter: test/test_interpreter.c minty_util.o token.o AST.o \
	parser.o interpreter.o
	$(LINK) test/test_interpreter.c minty_util.o token.o lexer.o AST.o \
		parser.o interpreter.o -o test/test_interpreter
	@test/test_interpreter

test/test_codegen: test/test_codegen.c minty_util.o token.o AST.o parser.o \
	codegen.o
	$(LINK) test/test_codegen.c minty_util.o token.o lexer.o AST.o parser.o \
		codegen.o -o test/test_codegen
	@test/test_codegen

.PRECIOUS: $(TESTS)