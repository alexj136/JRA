# Sets the command line flags used to -Wall (show all warnings for 'normal'
# programs) and -g (produce debugging info)
CFLAGS = -Wall -g

SOURCES = minty_util.c lexer.c AST.c parser.c minty.c

OUTFILES = minty_util.o lexer.o AST.o parser.o minty

# Adding this line means you can just run 'make' and lexer, AST & minty_util
# will all be built
all: lexer AST minty_util

# Command that is run when 'make clean' is run
clean:
	rm -f $(OUTFILES)