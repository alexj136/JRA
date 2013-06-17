class Program(object):
	"""
	A Program has one attribute: a list of functions
	"""
	def __init__(self, fns):
		self.fns = fns

class FN(object):
	"""
	A FN has four attributes:
		name - a string that identifies the function
		args - a dictionary that maps variable names to values for all arguments
		return_identifier - the variable name for the value that will get returned
		statements - the list of statements that the program contains
	"""
	def __init__(self, name, args, return_identifier, statements):
		self.name = name
		self.args = args
		self.return_identifier = return_identifier
		self.statements = statements

class Statement(object):
	pass

class Assignment(Statement):
	def __init__(self, assignee_identifier, expression):
		self.assignee_identifier = assignee_identifier
		self.expression = expression

class For(Statement):
	def __init__(self, int_lit_1, int_lit_2, statements):
		self.int_lit_1 = int_lit_1
		self.int_lit_2 = int_lit_2
		self.statements = statements

class BoolStatement(Statement):
	def __init__(self, expr_left, comparison, expr_right, statements):
		self.expr_left = expr_left
		self.comparison = comparison
		self.expr_right = expr_right
		self.statements = statements

class While(BoolStatement):
	pass

class If(BoolStatement):
	def __init__(self, else_statements):
		self.else_statements = else_statements

class Expression(object):
	pass

class ArithmeticExpr(Expression):
	def __init__(self, lhs, op, rhs):
		self.lhs = lhs
		self.op = op
		self.rhs = rhs

class Identifier(Expression):
	def __init__(self, name):
		self.name = name

class IntegerLiteral(Expression):
	def __init__(self, value):
		self.value = value

class FNCall(Expression):
	def __init__(self, name, args):
		self.name = name
		self.args = args