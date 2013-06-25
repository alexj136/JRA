class Program(object):
	def __init__(self, fns):
		# List of FNDecl objects
		self.fns = fns

class FNDecl(object):
	def __init__(self, name, arg_names, statements):
		# String: name that this function can be called with
		self.name = name

		# List of Strings: names of the arguments to this function
		self.arg_names = arg_names

		# List of Statement objects
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
		# Assignment object
		self.assignment = assignment

		# BoolExpression object
		self.bool_expr = bool_expr

		# Assignment object
		self.incrementor = incrementor

		# List of Statement objects
		self.statements = statements

class While(Statement):
	def __init__(self, bool_expr, statements):
		# BoolExpression object
		self.bool_expr = bool_expr

		# List of Statement objects
		self.statements = statements

class If(Statement):
	def __init__(self, bool_expr, if_statements, else_statements):
		# BoolExpression object
		self.bool_expr = bool_expr

		# List of Statement objects
		self.if_statements = if_statements

		# List if Statement objects
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

class Ternary(Expression):
	def __init__(self, bool_expr, true_exp, false_exp):
		# BoolExpression object
		self.bool_expr = bool_expr

		# Expression object
		self.true_exp = true_exp

		# Expression object
		self.false_exp = false_exp