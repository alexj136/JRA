from Lexer import *
from Parser import *
from Interpreter import *
from sys import argv

if not len(argv) < 2: print 'Incorrect number file arguments supplied'

src_file = open(argv[1], 'r')
src_code = src_file.read()

prog_ast = parse_program(lex(src_code))
print interpret_program(prog_ast, argv[2:])