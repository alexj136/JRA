import unittest, sys
sys.path.insert(0, "..")
from lexer import lex
from parser import *
from AST import *

class TestParser(unittest.TestCase):
	
	def test_parse_program(self):
		"""
		Tests that the parser gives the correct ASTs for certain programs. It
		assumes that the lexer produces the correct set of tokens for the
		program strings shown.
		"""

		# Test parsing a very simple program that immidiately returns 0
		basic_prog = lex('fn main() { return 0; }')
		basic_AST = Program([FNDecl('main', [], [Return(IntegerLiteral(0))])])
		self.assertTrue(parse_program(basic_prog) == basic_AST)

		# Test parsing a program that has two functions
		fncall_test_prog = lex("""
		fn main() {
			return hundred();
		}

		fn hundred() {
			return 100;
		}
		""")
		fncall_test_AST = Program([
			FNDecl('main', [], [
				Return(FNCall('hundred', []))
			]),
			FNDecl('hundred', [], [
				Return(IntegerLiteral(100))
			])
		])
		self.assertTrue(parse_program(fncall_test_prog) == fncall_test_AST)

		# Tests parsing a slightly more complex function call, with multiple
		# arguments and arithmetic expressions
		add_fn_prog = lex("""
		fn main() {
			return binary_add(4, 5);
		}

		fn binary_add(num1, num2) {
			return num1 + num2;
		}
		""")
		add_fn_AST = Program([
			FNDecl('main', [], [
				Return(FNCall('binary_add',
					[IntegerLiteral(4), IntegerLiteral(5)]))
			]),
			FNDecl('binary_add', ['num1', 'num2'], [
				Return(ArithmeticExpr(
					Identifier('num1'), '+', Identifier('num2')))
			])
		])
		self.assertTrue(parse_program(add_fn_prog) == add_fn_AST)

		# Tests that print statements are parsed correctly
		print_prog = lex("""
		fn main(num) {
			print num;
			return num + 1;
		}
		""")
		print_AST = Program([
			FNDecl('main', ['num'], [
				Print(Identifier('num')),
				Return(ArithmeticExpr(
					Identifier('num'), '+', IntegerLiteral(1)))
			])
		])
		self.assertTrue(parse_program(print_prog) == print_AST)

		# Test that a big expression is parsed correctly
		big_exp_prog=lex("""
		fn main() {
			return 1 + 2 + 3 < 4 ? 1 : 2 * 2 / 7 % 6;
		}
		""")
		big_exp_AST = Program([
			FNDecl('main', [], [
				Return(
					ArithmeticExpr(IntegerLiteral(1), '+',
						ArithmeticExpr(IntegerLiteral(2), '+',
							Ternary(
								BoolExpression(IntegerLiteral(3), '<',
									IntegerLiteral(4)),
								IntegerLiteral(1),
								ArithmeticExpr(IntegerLiteral(2), '*',
									ArithmeticExpr(IntegerLiteral(2), '/',
										ArithmeticExpr(
											IntegerLiteral(7), '%',
											IntegerLiteral(6)
										)
									)
								)
							)
						)
					)
				)
			])
		])
		self.assertTrue(parse_program(big_exp_prog) == big_exp_AST)

		# Tests that while-loops are parsed correctly
		while_prog = lex("""
		fn main(num) {
			while(num < 10) {
				print num;
			}
			return 0;
		}
		""")
		while_AST = Program([
			FNDecl('main', ['num'], [
				While(BoolExpression(
					Identifier('num'), '<', IntegerLiteral(10)), [
					Print(Identifier('num'))
				]),
				Return(IntegerLiteral(0))
			])
		])
		self.assertTrue(parse_program(while_prog) == while_AST)

		# Tests that for-loops are parsed correctly
		for_prog = lex("""
		fn main() {
			for i <- 20, i < 30, i <- i + 1 {
				print i;
			}
			return 999;
		}
		""")
		for_AST = Program([
			FNDecl('main', [], [
				For(Assignment(Identifier('i'), IntegerLiteral(20)),
					BoolExpression(Identifier('i'), '<', IntegerLiteral(30)),
					Assignment(Identifier('i'), ArithmeticExpr(Identifier('i'),
						'+', IntegerLiteral(1))), [
					Print(Identifier('i'))
				]),
				Return(IntegerLiteral(999))
			])
		])
		self.assertTrue(parse_program(for_prog) == for_AST)

		# Tests that if-statements are parsed correctly
		if_prog = lex("""
		fn main(num) {
			if(num < 10) {
				return 1;
			}
			else {
				return 0;
			}
		}
		""")
		if_AST = Program([
			FNDecl('main', ['num'], [
				If(BoolExpression(Identifier('num'), '<', IntegerLiteral(10)), [
					Return(IntegerLiteral(1))
				], [
					Return(IntegerLiteral(0))
				]),
			])
		])
		self.assertTrue(parse_program(if_prog) == if_AST)

	def test_expressions(self):

		# Tests that the big arithmetic expression shown below is parsed
		# correctly. The semicolon is required for the recursive calls to
		# terminate
		arithmetic = lex("1 + 2 * 3 / 4 - 5 % 6 < 7 ? 8 : 9;")
		arithmetic_AST = ArithmeticExpr(IntegerLiteral(1), '+',
			ArithmeticExpr(IntegerLiteral(2), '*',
				ArithmeticExpr(IntegerLiteral(3), '/',
					ArithmeticExpr(IntegerLiteral(4), '-',
						ArithmeticExpr(IntegerLiteral(5), '%',
							Ternary(BoolExpression(
								IntegerLiteral(6), '<', IntegerLiteral(7)),
								IntegerLiteral(8), IntegerLiteral(9)))))))
		self.assertTrue(parse_expression(arithmetic) == arithmetic_AST)
		print str(arithmetic_AST)

# Run the tests
if __name__ == '__main__':
	unittest.main()