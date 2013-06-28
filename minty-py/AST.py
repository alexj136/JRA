class ASTNode(object):
	pass

class Program(ASTNode):
	def __init__(self, fns):
		# Ensure that the given list contains only functions
		if not all([issubclass(fn.__class__, FNDecl) for fn in fns]):
			raise Exception('Non-FNDecl object in Program object')

		# List of FNDecl objects
		self.fns = fns

class FNDecl(ASTNode):
	def __init__(self, name, arg_names, statements):
		# Ensure types are correct
		if not issubclass(name.__class__, str):
			raise Exception('\'' + name.__class__.__name__ + \
				'\' object found in FNDecl.name, str expected')
		if not all([issubclass(arg.__class__, str) for arg in arg_names]):
			raise Exception('Non-str object in argument list for FNDecl: ' + \
				name)
		if not all([issubclass(st.__class__, Statement) for st in statements]):
			raise Exception('Non-Statement object in statement list \
				for FNDecl: ' + name)

		# String: name that this function can be called with
		self.name = name

		# List of Strings: names of the arguments to this function
		self.arg_names = arg_names

		# List of Statement objects
		self.statements = statements

class Statement(ASTNode):
	pass

class ControlStatement(Statement):
	def __init__(self, bool_expr, statements):
		# Ensure types are correct
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('\'' + bool_expr.__class__.__name__ + \
				'\' object found in ControlStatement: ' + \
				self.__class__.__name__ + ', BoolExpression expected')
		if not all([issubclass(st.__class__, Statement) for st in statements]):
			raise Exception('Non-Statement object in statement list \
				for ControlStatement: ' + self.__class__.__name__)

		# BoolExpression object
		self.bool_expr = bool_expr

		# List of Statement objects
		self.statements = statements


class For(ControlStatement):
	def __init__(self, assignment, bool_expr, incrementor, statements):
		ControlStatement.__init__(self, bool_expr, statements)

		# Ensure types are correct
		if (not issubclass(assignment.__class__, Assignment)) or \
			(not issubclass(incrementor.__class__, Assignment)):
			raise Exception('\'' + assignment.__class__.__name__ + \
				'\' object found in For, Assignment expected')

		# Assignment object
		self.assignment = assignment

		# Assignment object
		self.incrementor = incrementor

class While(ControlStatement):
	def __init__(self, bool_expr, statements):
		ControlStatement.__init__(self, bool_expr, statements)

class If(ControlStatement):
	def __init__(self, bool_expr, statements, else_statements):
		ControlStatement.__init__(self, bool_expr, statements)

		# Ensure types are correct
		if not all([issubclass(statement.__class__, Statement) \
			for statement in else_statements]):
			raise Exception('Non-Statement object in else statement list in If')

		# List if Statement objects
		self.else_statements = else_statements

class StatementWithExpr(Statement):
	def __init__(self, expression):

		# Ensure types are correct
		if not issubclass(expression.__class__, Expression):
			raise Exception('Non-Expression object in StatementWithExpr: ' + \
				self.__class__.__name__)

		# Expression object
		self.expression = expression

class Print(StatementWithExpr):
	def __init__(self, expression):
		StatementWithExpr.__init__(self, expression)

class Assignment(StatementWithExpr):
	def __init__(self, assignee_identifier, expression):
		StatementWithExpr.__init__(self, expression)

		# Ensure types are correct
		if not issubclass(assignee_identifier.__class__, Identifier):
			raise Exception('Non-Identifier object in StatementWithExpr: ' + \
				self.__class__.__name__)

		# Identifier object
		self.assignee_identifier = assignee_identifier

class Return(StatementWithExpr):
	def __init__(self, expression):
		StatementWithExpr.__init__(self, expression)

class Expression(ASTNode):
	pass

class BoolExpression(Expression):
	def __init__(self, expr_left, comparison, expr_right):
		# Ensure types are correct
		if not issubclass(expr_left.__class__, Expression):
			raise Exception(expr_left.__class__.__name__ + ' object found ' + \
				'in BoolExpression, Expression expected')
		if comparison not in ['=', '!=', '<', '>', '<=', '>=']:
			raise Exception(comparison.__class__.__name__ + ' object found' + \
				'in BoolExpression, comparison operator string expected')
		if not issubclass(expr_right.__class__, Expression):
			raise Exception(expr_right.__class__.__name__ + ' object found ' + \
				'in BoolExpression, Expression expected')

		# Expression object
		self.expr_left = expr_left

		# String: '=', '!=', '<', '>', '<=' or '>='
		self.comparison = comparison

		#Expression object
		self.expr_right = expr_right

class ArithmeticExpr(Expression):
	def __init__(self, lhs, op, rhs):
		# Ensure types are correct
		if not issubclass(lhs.__class__, Expression):
			raise Exception(lhs.__class__.__name__ + ' object found ' + \
				'in ArithmeticExpr, Expression expected')
		if op not in ['+', '-', '*', '/', '%']:
			raise Exception(op.__class__.__name__ + ' object found' + \
				'in ArithmeticExpr, arithmetic operator string expected')
		if not issubclass(rhs.__class__, Expression):
			raise Exception(rhs.__class__.__name__ + ' object found ' + \
				'in ArithmeticExpr, Expression expected')

		# Expression object
		self.lhs = lhs

		# String: '+', '-', '*', '/' or '%'
		self.op = op

		# Expression object
		self.rhs = rhs

class Identifier(Expression):
	def __init__(self, name):
		# Ensure types are correct
		if not issubclass(name.__class__, str):
			raise Exception('\'' + name.__class__.__name__ + \
				'\' object found in Identifier.name, str expected')

		# String: declared name
		self.name = name

class IntegerLiteral(Expression):
	def __init__(self, value):
		# Ensure types are correct
		if not issubclass(value.__class__, int):
			raise Exception(value.__class__.__name__ + ' object found in ' + \
				'IntegerLiteral, int expected')

		# Integer value
		self.value = value

class FNCall(Expression):
	def __init__(self, name, arg_vals):
		# Ensure types are correct
		if not issubclass(name.__class__, str):
			raise Exception('\'' + name.__class__.__name__ + \
				'\' object found in FNCall.name, str expected')
		if not all([issubclass(arg.__class__, Expression) for arg in arg_vals]):
			raise Exception('Non-Expression object in argument list \
				for FNCall: ' + name)

		# String: declared name of function being called
		self.name = name

		# List of expression objects
		self.arg_vals = arg_vals

class Ternary(Expression):
	def __init__(self, bool_expr, true_exp, false_exp):
		# Ensure types are correct
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception(bool_expr.__class__.__name__ + ' object found ' + \
				'in Ternary.bool_expr, BoolExpression expected')
		if not issubclass(true_exp.__class__, Expression):
			raise Exception(true_exp.__class__.__name__ + ' object found' + \
				'in Ternary.true_exp, Expression expected')
		if not issubclass(false_exp.__class__, Expression):
			raise Exception(false_exp.__class__.__name__ + ' object found ' + \
				'in Ternary.false_exp, Expression expected')

		# BoolExpression object
		self.bool_expr = bool_expr

		# Expression object
		self.true_exp = true_exp

		# Expression object
		self.false_exp = false_exp