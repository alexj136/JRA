from AST import *

def interpret_program(tree):
	function_map = {}

	if issubclass(tree.__class__, Program):
		# Load the functions in the program into the function map
		for fn in tree.fns:
			if issubclass(fn.__class__, FNDecl):
				function_map[fn.name] = fn
			else:
				raise Exception('Invalid object: \'' + tree.__class__.__name__ + '\', not a function')

	else:
		raise Exception('Cannot interpret object: \'' + tree.__class__.__name__ + '\', not a program')

	# If the given program has a main function, interpret it
	if 'main' in function_map:
		print str(interpret_function(function_map['main'], []))

def interpret_function(function, arg_values):
	# Begin by creating the list of in-scope variables
	in_scope_variables = dict(zip(function.arg_names, arg_values))

	# The return variable must be added to the list of in-scope variables
	in_scope_variables[function.return_identifier.name] = 0

	# Interpret the statements
	for st in function.statements:
		interpret_statement(st, in_scope_variables)

	# Return the value left in the return variable
	return in_scope_variables[function.return_identifier.name]

def interpret_statement(statement, in_scope_variables):
	# Check which kind of Statement this is

	if issubclass(statement.__class__, Assignment): #ASSIGNMENT
		# With assignment, we add an entry to the in_scope_variables dictionary
		in_scope_variables[statement.assignee_identifier] = interpret_expression(statement.expression)

	elif issubclass(statement.__class__, For): #FOR-LOOP
		# Interpret the assignment in the for loop declaration
		interpret_statement(assignment, in_scope_variables)
		# Repeatedly execute each substatement, and do the specified
		# incrementation after each iteration
		while interpret_boolean(statement.bool_expr, in_scope_variables):
			for st in statement.statements:
				interpret_statement(st, dict(in_scope_variables.items()))
			interpret_statement(statement.incrementor, in_scope_variables)

	elif issubclass(statement.__class__, While):
		# The while loop case is simple - set up a while loop which executes all
		# substatements until the declared condition is false
		while interpret_boolean(statement.bool_expr, in_scope_variables):
			for st in statement.statements:
				interpret_statement(st, dict(in_scope_variables.items()))

	elif issubclass(statement.__class__, If):
		# If statements are also simple - evaluate the boolean expression, and
		# if true, do the if-block, otherwise do the else-block
		if interpret_boolean(statement.bool_expr, in_scope_variables):
			for st in statement.if_statements:
				interpret_statement(st, dict(in_scope_variables.items()))
		else:
			for st in statement.statements:
				interpret_statement(st, dict(in_scope_variables.items()))

	elif issubclass(statement.__class__, Incrementor):
		# To interpret the increment operator, work out what is the appropriate
		# incrementation action, and apply it to the specified variable
		if statement.op == '+=':
			in_scope_variables[statement.assignee] = in_scope_variables[statement.assignee] + interpret_expression(statement.expression)
		elif statement.op == '-=':
			in_scope_variables[statement.assignee] = in_scope_variables[statement.assignee] - interpret_expression(statement.expression)
		else:
			raise Exception('Invalid incrementor/decrementor operation in AST')

	elif issubclass(statement.__class__, Print):
		# Print is super-easy: just evaluate the expression and print the result
		print str(interpret_expression(statement.expression, in_scope_variables))

	# If the statement parameter is not a subclass of Statement, raise an exception
	else:
		raise Exception('Cannot interpret object: \'' + tree.__class__.__name__ + '\', not a statement')


def interpret_expression(expression, in_scope_variables):
	# Check which kind of Expression this is
	if issubclass(expression.__class__, ArithmeticExpr):
		# Check which kind of operator this ArithmeticExpr has, and
		# return 'LHS OP RHS'
		if expression.op == '+':
			return expression.lhs + expression.rhs
		elif expression.op == '-':
			return expression.lhs - expression.rhs
		elif expression.op == '*':
			return expression.lhs * expression.rhs
		elif expression.op == '/':
			return expression.lhs / expression.rhs
		elif expression.op == '%':
			return expression.lhs % expression.rhs
		else: # Anything else is an error
			raise Exception("Invalid arithmetic expression found!")

	elif issubclass(expression.__class__, Identifier):
		# If we see an identifier, we lookup in_scope_variables to find its value
		return in_scope_variables[expression.name]

	elif issubclass(expression.__class__, IntegerLiteral):
		# Simple case - return the integer literal's value
		return expression.value

	elif issubclass(expression.__class__, FNCall):
		return interpret_function(expression.name, expression.arg_vals)
	else:
		raise Exception('Invalid expression found!')

def interpret_boolean(bool_expr, in_scope_variables):
	# Do the appropriate comparison between the LHS and RHS, and return the result
	if bool_expr.comparison == '=':
		return interpret_expression(bool_expr.expr_left) == interpret_expression(bool_expr.expr_right)
	if bool_expr.comparison == '!=':
		return interpret_expression(bool_expr.expr_left) != interpret_expression(bool_expr.expr_right)
	elif bool_expr.comparison == '<':
		return interpret_expression(bool_expr.expr_left) < interpret_expression(bool_expr.expr_right)
	elif bool_expr.comparison == '>':
		return interpret_expression(bool_expr.expr_left) > interpret_expression(bool_expr.expr_right)
	elif bool_expr.comparison == '<=':
		return interpret_expression(bool_expr.expr_left) <= interpret_expression(bool_expr.expr_right)
	elif bool_expr.comparison == '>=':
		return interpret_expression(bool_expr.expr_left) >= interpret_expression(bool_expr.expr_right)
	else:
		raise Exception('Invalid boolean expression found!')