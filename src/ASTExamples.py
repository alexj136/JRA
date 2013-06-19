from Interpreter import *
from AST import *

"""
PROGRAM TO TEST FUNCTION CALLS

fn main() {
	return hundred();
}

fn hundred() {
	return 100;
}
"""
fncall_test = Program([
	FNDecl('main', [], [
		Return(FNCall('hundred', []))
	]),
	FNDecl('hundred', [], [
		Return(IntegerLiteral(100))
	])
])

print str(interpret_program(fncall_test, []))

"""
TEST OF FUNCTION CALLS (WITH ARGUMENTS) AND ADDITION

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

"""
PRINT TEST

fn main(num) {
	print num;
	return num + 1;
}
"""
add_one = Program([
	FNDecl('main', ['num'], [
		Print(Identifier('num')),
		Return(ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1)))
	])
])

print str(interpret_program(add_one, [4]))

"""
TEST OF ASSIGNMENT AND ADDITION

fn main(num) {
	num = num + 1;
	return num;
}
"""
assign_test = Program([
	FNDecl('main', ['num'], [
		Assignment(Identifier('num'),
			ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1))),
		Return(Identifier('num'))
	])
])

print str(interpret_program(assign_test, [7]))

"""
WHILE LOOP TEST

fn main(num) {
	while(num < 10) {
		num = num + 1;
		print num;
	}
	return 0;
}
"""
while_test = Program([
	FNDecl('main', ['num'], [
		While(BoolExpression(Identifier('num'), '<', IntegerLiteral(10)), [
			Assignment(Identifier('num'),
				ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1))),
			Print(Identifier('num'))
		]),
		Return(IntegerLiteral('num'))
	])
])

#print str(interpret_program(while_test, [1]))