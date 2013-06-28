from lexer import *
from parser import *
from interpreter import *
from sys import argv

if len(argv) < 2:
	print 'Incorrect number file arguments supplied'

else:
	src_file = open(argv[1], 'r')
	src_code = src_file.read()

	prog_ast = parse_program(lex(src_code))
	print interpret_program(prog_ast, argv[2:])