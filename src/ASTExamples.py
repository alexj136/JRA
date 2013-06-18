from Interpreter import *
from AST import *

"""
fn main() {
	return hundred(4);
}

fn hundred(num) {
	return 100;
}

"""
hundred = Program([
	FNDecl('main', [], [
		Return(FNCall('hundred', [IntegerLiteral(4)]))
	]),
	FNDecl('hundred', ['num'], [
		Return(IntegerLiteral(100))
	])
])

print str(interpret_program(hundred, []))

"""
fn main() {
	return binary_add(4, 5);
}

fn binary_add(num1, num2) {
	return num1 + num2;
}

"""
binary_add = Program([
	FNDecl('main', [], [
		Return(FNCall('binary_add', [IntegerLiteral(4), IntegerLiteral(5)]))
	]),
	FNDecl('binary_add', ['num1', 'num2'], [
		Return(ArithmeticExpr(Identifier('num1'), '+', Identifier('num2')))
	])
])

print str(interpret_program(binary_add, []))