from AST import *

class IntWrapper(object):
	"""
	Integer wrapper class that is used by VariableScope objects to prevent
	primitives being copied when a VariableScope is proliferated, so that
	changes to already-live variables are maintained when the scope reverts.
	"""
	def __init__(self, value):
		self.value = value

class VariableScope(object):
	"""
	This class is used to keep track of the variables in scope at a particular
	nesting level. The __init__() method should not be called directly. To begin
	an entirely new scope, call the static method 'initiate()'. When a new scope
	is required, call proliferate().
	This class can also be used to determine if a statement has caused a
	function to return, and to record the return value.
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
		return initial_scope

	def proliferate(self):
		new_scope = VariableScope()
		new_scope.ret_val = self.ret_val
		new_scope.var_map = dict(self.var_map.items())
		return new_scope

	def has_var(self, var):
		return var in self.var_map

	def get(self, var):
		if var in self.var_map:
			return self.var_map[var].value
		else:
			raise Exception(
				'Variable \'' + var + '\' is not declared in this scope')

	def set(self, var, value):
		if self.has_var(var):
			self.var_map[var].value = value
		else:
			self.var_map[var] = IntWrapper(value)

	def get_return(self):
		if self.ret_val.value is not None:
			return self.ret_val.value
		else:
			raise Exception('Return value not present')

	def set_return(self, value):
		self.ret_val.value = value

	def has_return(self):
		return self.ret_val.value is not None

FUNCTION_MAP = {}

def interpret_program(program, program_arguments, REPL=False):
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
	if not REPL and 'main' in FUNCTION_MAP:
		return interpret_function('main', program_arguments)

	elif REPL:
		raise Exception('REPL not yet implemented')

	# If no main function was declared, raise an exception to the caller
	else: raise Exception('No main function found')

def interpret_function(function_name, arg_values):
	"""
	interpret_function is responsible for the interpretation of the AST objects
	that correspond to functions. Since a function should have its own variable
	scope, a object of type VariableScope is created, which will allow access to
	any variables in the scope of this function. The initial values in a
	VariableScope are the function's arguments.

	Once the VariableScope has been created, all that remains is to	execute the
	statements that comprise the function.

	An important issue when dealing with interpreted functions is handling
	return statements. In a compiled language, return statements have the
	ability to 'manage' their own execution - they are machine code statements
	that the CPU will execute. With an interpreted language, a return
	statement consists of a 'dummy object' with no ability to ensure that the
	operation it specifies is carried out. To deal with this issue, this
	implementation checks if a return operation has occurred after every
	statement. If a statement is not a return, no action need be taken,
	sequential execution of statements can continue. When a return statement is
	reached, this is recorded by calling set_return() method of the
	VariableScope object. Accordingly, this will cause all enclosing blocks of
	code to break, and indicate to the containing function that a return
	operation has occurred, meaning that there will be a stored return value in
	the VariableScope object. The function stops without any further statement
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
	scope = VariableScope.initiate(function.arg_names, arg_values)

	# Interpret each statement
	for statement in function.statements:
		interpret_statement(statement, scope)

		# If a return value is found after any statement, we must break out of
		# the loop and return to the calling function
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

	iterpret_statement proceeds a 'return' statement has been evaluated. If a
	return statement has been evaluated, either in the immediate given
	statement, or in any statements that exists	within the statement, all loops
	will break, without evaluating any further sub-statements. The reason for
	this is given in the doc-string for interpret_function.

	There are 6 kinds of statement: assignment, print, return, for, while and
	if. This function begins by checking which of those the given statement is.
	"""

	if issubclass(statement.__class__, Assignment):

		# With assignment, we add the variable to the scope
		scope.set(statement.assignee_identifier.name,
			interpret_expression(statement.expression, scope))

	elif issubclass(statement.__class__, For):

		# Proliferate the scope object for the new scope
		new_scope = scope.proliferate()

		# Interpret the assignment in the for loop declaration
		interpret_statement(statement.assignment, new_scope)

		# Repeatedly execute each sub-statement, and do the specified
		# incrementation after each iteration
		while interpret_boolean(statement.bool_expr, new_scope):
			for st in statement.statements:
				interpret_statement(st, new_scope)

				# If a return value is found after any statement, we must break
				# out of the loop and return to the calling function
				if new_scope.has_return(): break
			if new_scope.has_return(): break

			# If we reach this statement, we have not reached a return, so
			# increment the loop control variable for the next iteration
			interpret_statement(statement.incrementor, new_scope)

	elif issubclass(statement.__class__, While):

		# Proliferate the scope object for the new scope
		new_scope = scope.proliferate()

		# The while loop case is simple - set up a while loop which executes all
		# sub-statements until the declared condition is false, or a return
		# statement is evaluated
		while interpret_boolean(statement.bool_expr, new_scope):
			for st in statement.statements:
				interpret_statement(st, new_scope)
				
				# If a sub-statement executes a return statement, so we must
				# break, returning control up to the calling function
				if new_scope.has_return(): break
			if new_scope.has_return(): break

	elif issubclass(statement.__class__, If):

		# Proliferate the scope object for the new scope
		new_scope = scope.proliferate()

		# If statements are also simple - evaluate the boolean expression, and
		# if true, do the if-block, otherwise do the else-block
		if interpret_boolean(statement.bool_expr, new_scope):
			for st in statement.statements:
				interpret_statement(st, new_scope)
				
				# If a sub-statement executes a return statement, so we must
				# break, returning control up to the calling function
				if new_scope.has_return(): break

		else:
			for st in statement.else_statements:
				interpret_statement(st, new_scope)
				
				# If a sub-statement executes a return statement, so we must
				# break, returning control up to the calling function
				if new_scope.has_return(): break

	elif issubclass(statement.__class__, Print):

		# Print is simple, just evaluate the expression and print the result
		print str(interpret_expression(statement.expression, scope))

	elif issubclass(statement.__class__, Return):

		# With return, we have to call the set_return() method of the scope
		# object. If this statement is embedded in an if, for or while, it will
		# be detected, and no further statements in that function will be
		# executed
		scope.set_return(interpret_expression(statement.expression, scope))

	# If the statement parameter is not a subclass of Statement, raise an
	# exception
	else:
		raise Exception('Cannot interpret object: \'' + \
			tree.__class__.__name__ + '\', not a statement')

def interpret_expression(expression, scope):
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

		Ternary - evaluates to one of two possible expressions based on the
		value of a boolean

	The function determines the type of the integer expression, and handles it
	accordingly.
	"""

	# To interpret an arithmetic expression, we simply look at the required
	# arithmetic operation in the passed object, and apply it to the left and
	# right hand side of the operator:
	if issubclass(expression.__class__, ArithmeticExpr):
		if expression.op == '+':
			return interpret_expression(expression.lhs, scope) + \
				interpret_expression(expression.rhs, scope)

		elif expression.op == '-':
			return interpret_expression(expression.lhs, scope) - \
				interpret_expression(expression.rhs, scope)

		elif expression.op == '*':
			return interpret_expression(expression.lhs, scope) * \
				interpret_expression(expression.rhs, scope)

		elif expression.op == '/':
			return interpret_expression(expression.lhs, scope) / \
				interpret_expression(expression.rhs, scope)

		elif expression.op == '%':
			return interpret_expression(expression.lhs, scope) % \
				interpret_expression(expression.rhs, scope)

		# Anything else is an error
		else: raise Exception("Invalid arithmetic expression found!")

	# Interpreting an identifier requires querying the scope, which is passed as
	# a parameter to this function. We look it up with the name of the
	# identifier, and the scope hands back the corresponding value.
	elif issubclass(expression.__class__, Identifier):
		return scope.get(expression.name)


	# Interpreting integer literals is very simple - we retrieve the literal
	# value from the passed object and hand it back to the caller:
	elif issubclass(expression.__class__, IntegerLiteral):
		return expression.value

	# To interpret a function call, evaluate the passed arguments, and then pass
	# them, with the name of the function being called, to interpret_function,
	# which will retrieve the correct function object from the function map,
	# and interpret it with the arguments as initial in-scope variables.
	elif issubclass(expression.__class__, FNCall):
		evaluated_args = [interpret_expression(arg, scope) \
			for arg in expression.arg_vals]
		return interpret_function(expression.name, evaluated_args)

	# To interpret a ternary, we evaluate the boolean, returning the
	# interpretation of the true part if the boolean is true, and the false part
	# otherwise
	elif issubclass(expression.__class__, Ternary):
		if interpret_boolean(expression.bool_expr, scope):
			return interpret_expression(expression.true_exp, scope)
		else:
			return interpret_expression(expression.false_exp, scope)

	# If a non-integer-expression has been passed, this is an error, so an
	# exception is generated:
	else: raise Exception('Invalid expression found!')

def interpret_boolean(bool_expr, scope):
	"""
	interpret_boolean is a boolean-typed function used to evaluate boolean
	expressions. The 'bool_expr' argument has three members: a LHS and RHS
	expression, and	a boolean operator OP. This purpose of this function is to
	find  and return the result of 'LHS OP RHS'.
	"""

	# Make the appropriate comparison between LHS and RHS, and return the result
	if bool_expr.comparison == '=':
		return interpret_expression(bool_expr.expr_left, scope) == \
			interpret_expression(bool_expr.expr_right, scope)

	elif bool_expr.comparison == '!=':
		return interpret_expression(bool_expr.expr_left, scope) != \
			interpret_expression(bool_expr.expr_right, scope)

	elif bool_expr.comparison == '<':
		return interpret_expression(bool_expr.expr_left, scope) < \
			interpret_expression(bool_expr.expr_right, scope)

	elif bool_expr.comparison == '>':
		return interpret_expression(bool_expr.expr_left, scope) > \
			interpret_expression(bool_expr.expr_right, scope)

	elif bool_expr.comparison == '<=':
		return interpret_expression(bool_expr.expr_left, scope) <= \
			interpret_expression(bool_expr.expr_right, scope)

	elif bool_expr.comparison == '>=':
		return interpret_expression(bool_expr.expr_left, scope) >= \
			interpret_expression(bool_expr.expr_right, scope)

	else: raise Exception('Invalid boolean expression found!')