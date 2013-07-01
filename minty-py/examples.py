from lexer import lex, valueless_tokens
from interpreter import *
from AST import *



print '================================================='
print 'TEST OF LEXER USING RANDOM TOKENS'

import random
more = valueless_tokens + ['foobar', 'asdf', 'qwerty', '!', '^', '##']
for iterations in range(10):
	print 'NEXT 10 TOKENS:'
	boop = ''
	for x in range(10):
		boop = boop + more[random.randint(0, len(more)-1)]
	tokens = lex(boop)
	for item in tokens:
		print item