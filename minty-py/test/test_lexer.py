import sys, random as rnd, unittest
sys.path.append("..")
from lexer import *

class TestLexer(unittest.TestCase):
	
	def test_valid_sequence(self):
		"""
		Test that the lexer produces the tokens that it should for a simple
		valid program
		"""

		# A simple program
		prog = """
		fn main() {
			return hundred();
		}

		fn hundred() {
			return 100;
		}
		"""

		# Tokens that the simple program should produce when lexed
		prog_tokens = [
			Token('fn', None),     Token('ID', 'main'),  Token('(', None),
			Token(')', None),      Token('{', None),     Token('return', None),
			Token('ID', 'hundred'),Token('(', None),     Token(')', None),
			Token(';', None),      Token('}', None),     Token('fn', None),
			Token('ID', 'hundred'),Token('(', None),     Token(')', None),
			Token('{', None),      Token('return', None),Token('INT', '100'),
			Token(';', None),      Token('}', None)]

		lexed_prog = lex(prog)

		# Assert that the two token lists are of the same length, and have the
		# same values
		self.assertEqual(len(lexed_prog), len(prog_tokens))
		for x in range(len(prog_tokens)):
			self.assertEqual(lexed_prog[x].type, prog_tokens[x].type)
			self.assertEqual(lexed_prog[x].info, prog_tokens[x].info)

	def test_random_sequence(self):
		"""
		Generates random strings of valid characters. It will then spoil the
		valid sequence with invalid characters with probability 0.25. This
		function tests that the spoiled strings return only LexerErrors and the
		unspoiled ones return only tokens
		"""
		bad_strs = ['$', '^', '#']
		good_strs = valueless_tokens + ['foobar', '123', 'qwerty']

		# Run 10 random tests
		for iterations in range(100):
			
			test_str = ''

			# Bad tokens included with 25% probability
			bad = rnd.randint(0, 3) == 3
			
			# Create a random test string
			for x in range(100):
				test_str = test_str + \
					good_strs[rnd.randint(0, len(good_strs) - 1)]

			# If we're making this string bad, drop 5 bad characters into it at
			# random locations
			if bad:
				for x in range(5):
					loc = rnd.randint(0, len(test_str) - 1)
					char = rnd.randint(0, 2)
					test_str = test_str[:loc] + bad_strs[char] + test_str[loc:]

			# Lex the random string
			tokens = lex(test_str)

			# If the string was bad, check that only LexerErrors were returned.
			# If not, check that only tokens were returned.
			for item in tokens:
				self.assertTrue(issubclass(item.__class__, LexerError) \
					if bad else issubclass(item.__class__, Token))

	def test_rejects_exclamation(self):
		"""
		The exclamation mark is the only character in the alphabet of the
		language that is not a valid token by itself. This function tests that
		it produces a LexerError.
		"""
		# Try to lex an '!'
		exclamation = lex('!')

		# Check that a single LexerError object is returned
		self.assertEqual(len(exclamation), 1)
		self.assertTrue(issubclass(exclamation[0].__class__, LexerError))

	def test_correct_tokens(self):
		"""
		Test that each lexically valid string produces the appropriate token
		"""
		for item in valueless_tokens:
			self.assertEqual('T_' + item, str(lex(item)[0]))

		ident = 'foobar'
		self.assertEqual('T_ID_' + ident, str(lex(ident)[0]))

		integer = '12345'
		self.assertEqual('T_INT_' + integer, str(lex(integer)[0]))

# Run the tests
if __name__ == '__main__':
	unittest.main()