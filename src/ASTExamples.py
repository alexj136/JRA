from Interpreter import *
from AST import *

print 'FUNCTION CALLS (NO ARGUMENTS) TEST'
fncall_test_str = """
fn main() {
	return hundred();
}

fn hundred() {
	return 100;
}
"""
fncall_test_AST = Program([
	FNDecl('main', [], [
		Return(FNCall('hundred', []))
	]),
	FNDecl('hundred', [], [
		Return(IntegerLiteral(100))
	])
])
print 'RESULT: ' + str(interpret_program(fncall_test_AST, []))


print '================================================='
print 'FUNCTION CALLS (WITH ARGUMENTS) AND ADDITION TEST'
"""
fn main() {
	return binary_add(4, 5);
}

fn binary_add(num1, num2) {
	return num1 + num2;
}
"""
binary_add_AST = Program([
	FNDecl('main', [], [
		Return(FNCall('binary_add', [IntegerLiteral(4), IntegerLiteral(5)]))
	]),
	FNDecl('binary_add', ['num1', 'num2'], [
		Return(ArithmeticExpr(Identifier('num1'), '+', Identifier('num2')))
	])
])
print 'RESULT: ' + str(interpret_program(binary_add_AST, []))


print '================================================='
print 'PRINT TEST'
"""
fn main(num) {
	print num;
	return num + 1;
}
"""
add_one_AST = Program([
	FNDecl('main', ['num'], [
		Print(Identifier('num')),
		Return(ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1)))
	])
])
print 'RESULT: ' + str(interpret_program(add_one_AST, [4]))


print '================================================='
print 'ASSIGNMENT AND ADDITION TEST'
"""
fn main(num) {
	num = num + 1;
	return num;
}
"""
assign_test_AST = Program([
	FNDecl('main', ['num'], [
		Assignment(Identifier('num'),
			ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1))),
		Return(Identifier('num'))
	])
])
print 'RESULT: ' + str(interpret_program(assign_test_AST, [7]))


print '================================================='
print 'WHILE LOOP TEST'
"""
fn main(num) {
	while(num < 10) {
		num = num + 1;
		print num;
	}
	return 0;
}
"""
while_test_AST = Program([
	FNDecl('main', ['num'], [
		While(BoolExpression(Identifier('num'), '<', IntegerLiteral(10)), [
			Assignment(Identifier('num'),
				ArithmeticExpr(Identifier('num'), '+', IntegerLiteral(1))),
			Print(Identifier('num'))
		]),
		Return(Identifier('num'))
	])
])
print 'RESULT: ' + str(interpret_program(while_test_AST, [1]))


print '================================================='
print 'FOR LOOP TEST'
"""
fn main() {
	for i = 20, i < 30, i = i + 1 {
		print i;
	}
	return 999;
}
"""
for_test_AST = Program([
	FNDecl('main', [], [
		For(Assignment(Identifier('i'), IntegerLiteral(20)),
			BoolExpression(Identifier('i'), '<', IntegerLiteral(30)),
			Assignment(Identifier('i'), ArithmeticExpr(Identifier('i'), '+',
				IntegerLiteral(1))), [

			Print(Identifier('i'))
		]),
		Return(IntegerLiteral(999))
	])
])
print 'RESULT: ' + str(interpret_program(for_test_AST, []))


print '================================================='
print 'IF TEST'
"""
fn main(num) {
	if(num < 10) {
		return 1;
	}
	else {
		return 0;
	}
}
"""
if_test_AST = Program([
	FNDecl('main', ['num'], [
		If(BoolExpression(Identifier('num'), '<', IntegerLiteral(10)), [
			Return(IntegerLiteral(1))
		], [
			Return(IntegerLiteral(0))
		]),
	])
])
print 'RESULT: ' + str(interpret_program(if_test_AST, [1]))