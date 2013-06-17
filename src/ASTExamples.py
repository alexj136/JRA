from Interpreter import *
from AST import *

fib = Program([FNDecl('main', [], Identifier('result'), [Assignment('result', FNCall('fib', [4]))]), FNDecl('fib', ['num'], Identifier('result'), [Assignment('result', FNCall('fib', [10]))])])

interpret_program(fib)