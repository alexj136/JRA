class Program(object):
	"""
	A Program has one attribute: a list of functions
	"""
	def __init__(self, fns):
		self.fns = fns

class FNDecl(object):
	"""
	A FN has four attributes:
		name - a string that identifies the function
		args - a dictionary that maps variable names to values for all arguments
		return_identifier - the variable name for the value that will get returned
		statements - the list of statements that the program contains
	"""
	def __init__(self, name, arg_names, return_identifier, statements):
		self.name = name
		self.arg_names = arg_names
		self.return_identifier = return_identifier
		self.statements = statements

class Statement(object):
	pass

class Assignment(Statement):
	def __init__(self, assignee_identifier, expression):
		self.assignee_identifier = assignee_identifier
		self.expression = expression

class For(Statement):
	def __init__(self, assignment, bool_expr, incrementor, statements):
		self.assignment = assignment
		self.bool_expr = bool_expr
		self.incrementor = incrementor
		self.statements = statements

class While(Statement):
	def __init__(self, bool_expr, statements):
		self.bool_expr = bool_expr
		self.statements = statements

class If(Statement):
	def __init__(self, bool_expr, if_statements, else_statements):
		self.bool_expr = bool_expr
		self.if_statements = if_statements
		self.else_statements = else_statements

class Print(Statement):
	def __init__(self, expression):
		self.expression = expression

class Incrementor(Statement):
	def __init__(self, assignee, op, expression):
		self.assignee = assignee
		self.op = op
		self.expression = expression

class BoolExpression(object):
	def __init__(self, expr_left, comparison, expr_right):
		self.expr_left = expr_left
		self.comparison = comparison
		self.expr_right = expr_right

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
	def __init__(self, name, arg_vals):
		self.name = name
		self.arg_vals = arg_vals