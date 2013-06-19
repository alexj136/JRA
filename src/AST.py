class Program(object):
	def __init__(self, fns):
		self.fns = fns

class FNDecl(object):
	def __init__(self, name, arg_names, statements):
		self.name = name
		self.arg_names = arg_names
		self.statements = statements

class Statement(object):
	pass

class Assignment(Statement):
	def __init__(self, assignee_identifier, expression):
		# Identifier object
		self.assignee_identifier = assignee_identifier

		# Expression object
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
		# Expression object
		self.expression = expression

class Return(Statement):
	def __init__(self, expression):
		# Expression object
		self.expression = expression



class BoolExpression(object):
	def __init__(self, expr_left, comparison, expr_right):
		# Expression object
		self.expr_left = expr_left

		# String: '=', '!=', '<', '>', '<=' or '>='
		self.comparison = comparison

		#Expression object
		self.expr_right = expr_right

class Expression(object):
	pass

class ArithmeticExpr(Expression):
	def __init__(self, lhs, op, rhs):
		# Expression object
		self.lhs = lhs

		# String: '+', '-', '*', '/' or '%'
		self.op = op

		# Expression object
		self.rhs = rhs

class Identifier(Expression):
	def __init__(self, name):
		# String: declared name
		self.name = name

class IntegerLiteral(Expression):
	def __init__(self, value):
		# Integer value
		self.value = value

class FNCall(Expression):
	def __init__(self, name, arg_vals):
		# String: declared name of function being called
		self.name = name

		# List of expression objects
		self.arg_vals = arg_vals