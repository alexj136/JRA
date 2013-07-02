"""
The classes in this file represent elements of a program, such as functions,
statements and expressions. The ASTNode objects are built into ASTs by the
parser, and the program that an AST represents can then be executed by the
interpreter.
"""

class ASTNode(object):
	pass

class Program(ASTNode):
	"""
	A program object is essentially just a list of FNDecl objects
	"""

	def __init__(self, fns):
		# Ensure that the given list contains only functions
		if not all([issubclass(fn.__class__, FNDecl) for fn in fns]):
			raise Exception('Non-FNDecl object in Program object')

		# List of FNDecl objects
		self.fns = fns

	def __eq__(self, other):
		return isinstance(other, Program) and \
			other.fns == self.fns

	def __str__(self):
		return '\n'.join(map(str, self.fns))

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

	def __eq__(self, other):
		return isinstance(other, FNDecl) and \
			other.name == self.name and \
			other.arg_names == self.arg_names and \
			other.statements == self.statements

	def __str__(self):
		return 'fn ' + self.name + '(' + ','.join(self.arg_names) + \
			') {\n' + '\n'.join(map(str, self.statements)) + '\n}'

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

	def __eq__(self, other):
		return issubclass(other.__class__, ControlStatement) and \
			other.bool_expr == self.bool_expr and \
			other.statements == self.statements

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

	def __eq__(self, other):
		return isinstance(other, For) and \
			super(For, self).__eq__(other) and \
			other.assignment == self.assignment and \
			other.incrementor == self.incrementor

	def __str__(self):
		return 'for ' + str(self.assignment) + ', ' + \
			str(self.bool_expr) + ', ' + str(self.incrementor) + \
			' {\n' + '\n'.join(map(str, self.statements)) + '\n}'

class While(ControlStatement):
	def __init__(self, bool_expr, statements):
		ControlStatement.__init__(self, bool_expr, statements)

	def __eq__(self, other):
		return isinstance(other, While) and \
			super(While, self).__eq__(other)

	def __str__(self):
		return 'while ' + str(self.bool_expr) + ' {\n' + \
			'\n'.join(map(str, self.statements)) + '\n}'

class If(ControlStatement):
	def __init__(self, bool_expr, statements, else_statements):
		ControlStatement.__init__(self, bool_expr, statements)

		# Ensure types are correct
		if not all([issubclass(statement.__class__, Statement) \
			for statement in else_statements]):
			raise Exception('Non-Statement object in else statement list in If')

		# List if Statement objects
		self.else_statements = else_statements

	def __eq__(self, other):
		return isinstance(other, If) and \
			super(If, self).__eq__(other) and \
			other.else_statements == self.else_statements

	def __str__(self):
		return 'for ' + str(self.bool_expr) + ', ' + ' {\n' + \
			'\n'.join(map(str, self.statements)) + '\n} else {\n' + \
			'\n'.join(map(str, self.else_statements)) + '\n}'

class StatementWithExpr(Statement):
	def __init__(self, expression):

		# Ensure types are correct
		if not issubclass(expression.__class__, Expression):
			raise Exception('Non-Expression object in StatementWithExpr: ' + \
				self.__class__.__name__)

		# Expression object
		self.expression = expression

	def __eq__(self, other):
		return issubclass(other.__class__, StatementWithExpr) and \
			other.expression == self.expression

class Print(StatementWithExpr):
	def __init__(self, expression):
		StatementWithExpr.__init__(self, expression)

	def __eq__(self, other):
		return isinstance(other, Print) and \
			super(Print, self).__eq__(other)

	def __str__(self):
		return 'print ' + str(self.expression) + ';'

class Assignment(StatementWithExpr):
	def __init__(self, assignee_identifier, expression):
		StatementWithExpr.__init__(self, expression)

		# Ensure types are correct
		if not issubclass(assignee_identifier.__class__, Identifier):
			raise Exception('Non-Identifier object in StatementWithExpr: ' + \
				self.__class__.__name__)

		# Identifier object
		self.assignee_identifier = assignee_identifier

	def __eq__(self, other):
		return isinstance(other, Assignment) and \
			super(Assignment, self).__eq__(other) and \
			other.assignee_identifier == self.assignee_identifier

	def __str__(self):
		return str(self.assignee_identifier) + ' <- ' + str(self.expression) + \
			';'

class Return(StatementWithExpr):
	def __init__(self, expression):
		StatementWithExpr.__init__(self, expression)

	def __eq__(self, other):
		return isinstance(other, Return) and \
			super(Return, self).__eq__(other)

	def __str__(self):
		return 'return ' + str(self.expression) + ';'

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

	def __eq__(self, other):
		return isinstance(other, BoolExpression) and \
			other.expr_left == self.expr_left and \
			other.comparison == self.comparison and \
			other.expr_right == self.expr_right

	def __str__(self):
		return '(' + str(self.expr_left) + ' ' + self.comparison + ' ' \
			+ str(self.expr_right) + ')'

class ArithmeticExpr(Expression):
	def __init__(self, lhs, op, rhs):
		# Ensure types are correct
		if not issubclass(lhs.__class__, Expression):
			raise Exception('\'' + lhs.__class__.__name__ + \
				'\' object found in ArithmeticExpr, Expression expected')
		if op not in ['+', '-', '*', '/', '%']:
			raise Exception(op.__class__.__name__ + ' object found' + \
				'in ArithmeticExpr, arithmetic operator string expected')
		if not issubclass(rhs.__class__, Expression):
			raise Exception('\'' + rhs.__class__.__name__ + \
				'\' object found in ArithmeticExpr, Expression expected')

		# Expression object
		self.lhs = lhs

		# String: '+', '-', '*', '/' or '%'
		self.op = op

		# Expression object
		self.rhs = rhs

	def __eq__(self, other):
		return isinstance(other, ArithmeticExpr) and \
			other.lhs == self.lhs and \
			other.op == self.op and \
			other.rhs == self.rhs

	def __str__(self):
		return '(' + str(self.lhs) + ' ' + self.op + ' ' + str(self.rhs) + ')'

class Identifier(Expression):
	def __init__(self, name):
		# Ensure types are correct
		if not issubclass(name.__class__, str):
			raise Exception('\'' + name.__class__.__name__ + \
				'\' object found in Identifier.name, str expected')

		# String: declared name
		self.name = name

	def __eq__(self, other):
		return isinstance(other, Identifier) and \
			other.name == self.name

	def __str__(self):
		return self.name

class IntegerLiteral(Expression):
	def __init__(self, value):
		# Ensure types are correct
		if not issubclass(value.__class__, int):
			raise Exception(value.__class__.__name__ + ' object found in ' + \
				'IntegerLiteral, int expected')

		# Integer value
		self.value = value

	def __eq__(self, other):
		return isinstance(other, IntegerLiteral) and \
			other.value == self.value

	def __str__(self):
		return str(self.value)

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

	def __eq__(self, other):
		return isinstance(other, FNCall) and \
			other.name == self.name and \
			other.arg_vals == self.arg_vals

	def __str__(self):
		return self.name + '(' + ', '.join(map(str, self.arg_vals)) + ')'

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

	def __eq__(self, other):
		return isinstance(other, Ternary) and \
			other.bool_expr == self.bool_expr and \
			other.true_exp == self.true_exp and \
			other.false_exp == self.false_exp

	def __str__(self):
		return '(' + str(self.bool_expr) + ' ? ' + str(self.true_exp) + \
			' : ' + str(self.false_exp) + ')'