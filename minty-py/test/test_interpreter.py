import unittest, sys
sys.path.insert(0, "..")
from lexer import lex
from parser import parse_program
from interpreter import interpret_program
from AST import *

class TestInterpreter(unittest.TestCase):
	
	def test_interpret_program(self):
		"""
		Tests that the interpreter gives correct results for certain programs.
		It assumes that the lexer and parser generate correct ASTs for the input
		strings.
		"""

		# Test interpreting a very simple program that immidiately returns 0
		self.assertEquals(interpret_program(parse_program(lex("""
			fn main() { return 0; }
		""")), []), 0)

		# Test interpreting a program with a function call
		self.assertEquals(interpret_program(parse_program(lex("""
		fn main() {
			return hundred();
		}

		fn hundred() {
			return 100;
		}
		""")), []), 100)

		# Tests interpreting a slightly more complex function call, with
		# multiple arguments and arithmetic expressions
		self.assertEquals(interpret_program(parse_program(lex("""
		fn main() {
			return binary_add(4, 5);
		}

		fn binary_add(num1, num2) {
			return num1 + num2;
		}
		""")), []), 9)

		# Tests that print statements can be interpreted
		self.assertEquals(interpret_program(parse_program(lex("""
		fn main(num) {
			print num;
			return num + 1;
		}
		""")), [999]), 1000)

		# Test that a big expression is interpreted correctly
		self.assertEquals(interpret_program(parse_program(lex("""
		fn main() {
			return 1 + 2 + 3 < 4 ? 1 : 2 * 2 / 7 % 6;
		}
		""")), []), 4)

		# Tests that while-loops are interpreted correctly
		while_prog = parse_program(lex("""
		fn main(num) {
			while(num < 10) {
				num += 3;
			}
			return num;
		}
		"""))
		self.assertEquals(interpret_program(while_prog, [0]), 12)

		# Tests that for-loops are interpreted correctly
		for_prog = parse_program(lex("""
		fn main() {
			x <- 10;
			for i <- 20, i < 30, i++ {
				x -= 1;
			}
			return x;
		}
		"""))
		self.assertEquals(interpret_program(for_prog, []), 0)

		# Tests that if-statements are interpreted correctly
		if_prog = parse_program(lex("""
		fn main(num) {
			if(num < 10) {
				return 1;
			}
			else {
				return 0;
			}
		}
		"""))
		self.assertEquals(interpret_program(if_prog, [9]), 1)
		self.assertEquals(interpret_program(if_prog, [10]), 0)
		self.assertEquals(interpret_program(if_prog, [11]), 0)

		# Test of the interpreted on a fibonacci program
		fib_prog = parse_program(lex("""
		fn main(x) { return fibonacci(x); }

		fn fibonacci(x) {
			if x = 0 {
				return 0;
			}
			else {} if x = 1 {
				return 1;
			}
			else {
				return fibonacci(x - 1) + fibonacci(x - 2);
			}
		}
		"""))
		self.assertEquals(interpret_program(fib_prog, [0]), 0)
		self.assertEquals(interpret_program(fib_prog, [1]), 1)
		self.assertEquals(interpret_program(fib_prog, [2]), 1)
		self.assertEquals(interpret_program(fib_prog, [3]), 2)
		self.assertEquals(interpret_program(fib_prog, [20]), 6765)

# Run the tests
if __name__ == '__main__':
	unittest.main()