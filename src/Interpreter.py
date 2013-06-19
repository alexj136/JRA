from AST import *

class IntWrapper(object):
	"""
	Integer wrapper class that is used by the in_scope_variables dictionaries.
	Prevents the primitive being copied when a dictionary is copied so that
	changes to already-live variables are maintained when scope changes.
	"""
	def __init__(self, value):
		self.value = value

class VariableScope(object):
	"""
	Keeps track of the variables in scope at a particular nesting level. The
	__init__() method should not be called directly. To begin an entirely new
	scope, call the static method 'initiate()'. When a new scope is required,
	call proliferate().
	"""
	def __init__(self):
		self.var_map = None
		self.ret_val = None

	@staticmethod
	def initiate(names, values):
		initial_scope = VariableScope()
		wrapped_values = [IntWrapper(val) for val in values]
		initial_scope.var_map = dict(zip(names, wrapped_values))
		initial_scope.ret_val = IntWrapper(None)

	def proliferate(self):
		new_scope = VariableScope()
		new_scope.ret_val = self.ret_val
		new_scope.var_map = dict(self.var_map.items())
		return new_scope

	def has_var(self, var):
		return var in self.var_map

	def get(self, var):
		return self.var_map[var].value

	def set(self, var, value):
		if self.has_var(var):
			self.var_map[var].value = value
		else:
			self.var_map[var] = IntWrapper(value)

	def get_return(self):
		return self.ret_val.value

	def set_return(self, value):
		self.ret_val.value = value

	def has_return(self):
		return self.ret_val.value is not None

FUNCTION_MAP = {}

def interpret_program(program, program_arguments):
	"""
	interpret_program is the 'highest-level' function used to interpret ASTs,
	intended for external calls.

	A traditional compiler will compile and store generated code for every
	function that encounters. At runtime, functions that are called will have
	their target-language code executed. Since interpreters operate 'on the
	fly', there is no need to compile functions in advance. The AST nodes that
	correspond to functions are simply stored so that they can be interpreted
	when the function that they represent is called.

	This function takes a list of functions (a program) as its argument. Its job
	is simply to store the functions in a dictionary so that they can be
	accessed by any caller. After this, the main function is executed.
	"""

	# Check that the passed object is in fact a program
	if issubclass(program.__class__, Program):
		# Load the functions in the program into the function map, checking that
		# they are in fact function objects
		for fn in program.fns:
			if issubclass(fn.__class__, FNDecl):
				FUNCTION_MAP[fn.name] = fn
			else:
				raise Exception('Invalid object: \'' + \
					program.__class__.__name__ + '\', not a function')

	else:
		raise Exception('Cannot interpret object: \'' + \
			program.__class__.__name__ + '\', not a program')

	# If the given program has a main function, interpret it
	if 'main' in FUNCTION_MAP:
		return interpret_function('main', program_arguments)

	# If no main function was declared, raise an exception to the caller
	else: raise Exception('No main function found')

def interpret_function(function_name, arg_values):
	"""
	interpret_function is responsible for the interpretation of the AST objects
	that correspond to functions. Since a function should have its own variable
	scope, a dictionary called 'in_scope_variables' is created, which will
	allow any variables in the scope of this function to be accessed. The
	initial values in that dictionary are the function's arguments.

	Once the 'in_scope_variables' list has been created, all that remains is to
	execute the statements that comprise the function.

	An important issue when dealing with interpreted functions is handling
	return statements. In a compiled language, return statements have the
	ability to 'manage' their own execution - they are machine code statements
	that the CPU will execute. With an interpreted language, a return
	statement consists of a 'dummy object' with no ability to ensure that the
	operation it specifies is carried out. To deal with this issue, this
	implementation requires the interpretation of every single statement to
	return a boolean. If a statement returns false, no action need be taken,
	sequential execution of statements can continue. When statement
	interpretation returns true, it indicates that the statement that was just
	interpreted, was a return statement. Accordingly, this will cause all
	enclosing blocks of code to break, and indicate to the containing function
	that a return operation has occurred, meaning that there will be an entry in
	the 'in_scope_variables' dictionary with key 'return', which maps to the
	value to be returned. The function stops without any further statement
	interpretation and returns that value to the caller.
	"""

	# Check that the called function exists
	if not function_name in FUNCTION_MAP:
		raise Exception('Function: \'' + function_name + \
			'\' has not been declared')

	# Check that the function has been called with the appropriate number of
	# arguments
	if len(arg_values) != len(FUNCTION_MAP[function_name].arg_names):
		raise Exception('Function: \'' + function_name + '\' takes ' + \
			str(len(FUNCTION_MAP[function_name].arg_names)) + 'values, ' + \
			str(len(arg_values)) + ' given')

	# Retrieve the Function object from the map
	function = FUNCTION_MAP[function_name]

	# Create the VariableScope for this function
	scope = VariableScope.initiate(function.arg_names, wrapped_arg_values)

	# Interpret each statement
	for statement in function.statements:
		interpret_statement(statement, scope)
			# If we have reached this point, a return statement has been reached, so
			# hand the value left in the return variable to the caller
		if scope.has_return(): return scope.get_return()

	# If the statement interpretation loop finishes without returning, an error
	# has occurred in that the function had no return statement, so raise an
	# exception
	raise Exception('No return statement present in fn: ' + function_name)

def interpret_statement(statement, scope):
	"""
	interpret_statement is a function that interprets a single statement. In the
	case that the statement to be interpreted is a loop construct, this function
	will recursively call itself for each statement within that loop construct.

	iterpret_statement returns False except in the case that a 'return'
	statement has been evaluated. If a return statement has been evaluated,
	either in the immediate given statement, or in any statements that exists
	within the statement, this function will immediately return True, without
	evaluating any further sub-statements. The reason for this is given in the
	doc-string for interpret_function.

	There are 6 kinds of statement: assignment, print, return, for, while and
	if. This function begins by checking which of those the given statement is.
	"""

	if issubclass(statement.__class__, Assignment):

		# With assignment, we add the variable to the scope
		scope.set(statement.assignee_identifier.name,
			interpret_expression(statement.expression, scope))

	elif issubclass(statement.__class__, For):

		# Interpret the assignment in the for loop declaration
		interpret_statement(statement.assignment, scope)

		# Proliferate the scope object for the new scope
		new_scope = scope.proliferate()

		# Repeatedly execute each sub-statement, and do the specified
		# incrementation after each iteration
		while interpret_boolean(statement.bool_expr, scope):
			for st in statement.statements:
				interpret_statement(st, new_scope)
				# If a return value is found after any statement, we must break
				# out of both loops and return to the calling function
				if scope.has_return(): break
			else:
				interpret_statement(statement.incrementor, scope)
			if scope.has_return(): break

	elif issubclass(statement.__class__, While):

		# The while loop case is simple - set up a while loop which executes all
		# sub-statements until the declared condition is false
		while interpret_boolean(statement.bool_expr, in_scope_variables):
			for st in statement.statements:
				interpret_statement(st, dict(in_scope_variables.items())):
					# If a sub-statement interpretation returns True, a return
					# operation has been executed, so we must return control up
					# to the calling function
					return True

	elif issubclass(statement.__class__, If):

		# If statements are also simple - evaluate the boolean expression, and
		# if true, do the if-block, otherwise do the else-block
		if interpret_boolean(statement.bool_expr, in_scope_variables):
			for st in statement.if_statements:
				if interpret_statement(st, dict(in_scope_variables.items())):
					# If a sub-statement interpretation returns True, a return
					# operation has been executed, so we must return control up
					# to the calling function
					return True

		else:
			for st in statement.else_statements:
				if interpret_statement(st, dict(in_scope_variables.items())):
					# If a sub-statement interpretation returns True, a return
					# operation has been executed, so we must return control up
					# to the calling function
					return True

		# If we reach this point, the if-statement contents finished without a
		# return statement being evaluated, so return False
		return False

	elif issubclass(statement.__class__, Print):

		# Print is very simple: just evaluate the expression and print the
		# result
		print \
			str(interpret_expression(statement.expression, in_scope_variables))

		# No return statement evaluated - return False
		return False

	elif issubclass(statement.__class__, Return):

		# With return, we add a 'return' entry to the in_scope_variables
		# dictionary. This will never conflict with a programmer-declared
		# variable because 'return' is a reserved word.
		in_scope_variables['return'] = IntWrapper(
			interpret_expression(statement.expression, in_scope_variables))

		# By returning true, we force any enclosing loops to finish, so that
		# control returns to interpret_function, which can then hand the
		# dictionary value labeled 'return' back to the caller of that function
		return True

	# If the statement parameter is not a subclass of Statement, raise an
	# exception
	else:
		raise Exception('Cannot interpret object: \'' + \
			tree.__class__.__name__ + '\', not a statement')

def interpret_nested_block(statements, in_scope_variables):
	for statement in statements:
		if interpret_statement(statement, new_scope_vars):
			# If a sub-statement interpretation returns True, a return
			# operation has been executed, so we must return control up
			# to the calling function
			if 'return' in new_scope_vars:
				in_scope_variables['return'] = new_scope_vars['return']
			else: raise Exception('Return statement evaluated but no \
					return value found')
			return True
	return False

def interpret_expression(expression, in_scope_variables):
	"""
	interpret_expression is an integer-typed function used to evaluate integer
	expressions. There are four kinds of integer expression:

		Arithmetic expressions - basic operations on integers such as addition,
		subtraction, multiplication, division and modulo.

		Identifier - the use of a previously declared (and therefore stored)
		integer variable

		Integer literal - an definite, explicitly given integer

		Function call - the call to a function which will return an integer
		value

		Incrementor - increment or decrements an expression and then evaluates
		the resulting expression

	The function determines the type of the integer expression, and handles it
	accordingly.
	"""

	# To interpret an arithmetic expression, we simply look at the required
	# arithmetic operation in the passed object, and apply it to the left and
	# right hand side of the operator:
	if issubclass(expression.__class__, ArithmeticExpr):
		if expression.op == '+':
			return interpret_expression(expression.lhs, in_scope_variables) + \
				interpret_expression(expression.rhs, in_scope_variables)

		elif expression.op == '-':
			return interpret_expression(expression.lhs, in_scope_variables) - \
				interpret_expression(expression.rhs, in_scope_variables)

		elif expression.op == '*':
			return interpret_expression(expression.lhs, in_scope_variables) * \
				interpret_expression(expression.rhs, in_scope_variables)

		elif expression.op == '/':
			return interpret_expression(expression.lhs, in_scope_variables) / \
				interpret_expression(expression.rhs, in_scope_variables)

		elif expression.op == '%':
			return interpret_expression(expression.lhs, in_scope_variables) % \
				interpret_expression(expression.rhs, in_scope_variables)

		# Anything else is an error
		else: raise Exception("Invalid arithmetic expression found!")

	# Interpreting an identifier requires looking up the dictionary of in-scope
	# variables, which is passed as a parameter to this function. We simply
	# look at the name of the identifier, and lookup the dictionary with that
	# name. We then hand the value that the dictionary returns to us back up to
	# the caller.
	elif issubclass(expression.__class__, Identifier):
		return in_scope_variables[expression.name].value


	# Interpreting integer literals is very simple - we retrieve the literal
	# value from the passed object and hand it back to the caller:
	elif issubclass(expression.__class__, IntegerLiteral):
		return expression.value

	# To interpret a function call, evaluate the passed arguments, and then pass
	# them, with the name of the function being called, to interpret_function,
	# which will retrieve the correct function object from the function map,
	# and interpret it with the arguments as initial in-scope variables.
	elif issubclass(expression.__class__, FNCall):
		evaluated_args = [interpret_expression(arg, in_scope_variables) \
			for arg in expression.arg_vals]
		return interpret_function(expression.name, evaluated_args)

	# If a non-integer-expression has been passed, this is an error, so an
	# exception is generated:
	else: raise Exception('Invalid expression found!')

def interpret_boolean(bool_expr, in_scope_variables):
	"""
	interpret_boolean is a boolean-typed function used to evaluate boolean
	expressions. The 'bool_expr' argument has three members: a LHS and RHS
	expression, and	a boolean operator OP. This purpose of this function is to
	find  and return the result of 'LHS OP RHS'.
	"""

	# Make the appropriate comparison between LHS and RHS, and return the result
	if bool_expr.comparison == '=':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) ==\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	elif bool_expr.comparison == '!=':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) !=\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	elif bool_expr.comparison == '<':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) <\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	elif bool_expr.comparison == '>':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) >\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	elif bool_expr.comparison == '<=':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) <=\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	elif bool_expr.comparison == '>=':
		return interpret_expression(bool_expr.expr_left, in_scope_variables) >=\
			interpret_expression(bool_expr.expr_right, in_scope_variables)

	else: raise Exception('Invalid boolean expression found!')