from AST import *

arithmetic_ops = ['+', '-', '*', '/', '%']
boolean_ops = ['=', '!=', '<', '>', '<=', '>=']

class EPrime(object):
	"""
	Temporary store for the E' productions of the grammar that can be easily
	converted into ASTNodes.
	"""
	def __init__(self, op, exp, true_exp=None, false_exp=None):
		self.op = op
		self.exp = exp
		self.true_exp = true_exp
		self.false_exp = false_exp

	def is_ternary(self):
		return (self.true_exp is not None) and (self.false_exp is not None)

	def __str__(self):
		return 'op = ' + str(self.op) + ', exp = ' + str(self.exp) + \
			('' if not self.is_ternary else (', true_exp = ' + \
				str(self.true_exp) + ', false_exp = ' + str(self.false_exp)))


def check_valid(expected, found):
	"""
	Checks that found is in expected, and if not, raises an exception with a
	friendly error message.
	"""
	if found not in expected:
		str_bldr = expected.pop(len(expected) - 1)
		if expected != []:
			str_bldr = expected.pop(len(expected) - 1) + ' or ' + str_bldr
			while expected != []:
				str_bldr = expected.pop(len(expected) - 1) + ', ' + str_bldr
		raise Exception('\'' + found + '\' found, ' + str_bldr + ' expected')

def parse_program(token_list):
	"""
	This function is the entry point for the parser. It is responsible for
	maing calls to parse_function for each function in the program, putting
	each function into a list, wrapping that list into a Program object and
	returning that Program object.
	"""
	function_list = []

	while token_list != []:
		function_list.append(parse_function(token_list))

	return Program(function_list)

def parse_function(token_list):
	"""
	This function is responsible for parsing an entire function, and returning
	that function in AST form as an FNDecl object. To acheive this, the
	following steps occur:
		- Process the 'fn' declaration
		- Record the function's name
		- Process the open-bracket that proceeds the arguments list
		- Record the argument names, while processing the delimiting commas
		- Process the close-bracket that ends the argument list
		- Process the opening curly-bracket
		- Iteratively process each statement in the function
		- Process the closing curly-bracket
	During these steps, the FNDecl ASTNode object is built up, and at the end it
	is returned to the caller.
	"""

	# Retrieve the first token, check that it's an FN
	next_token = token_list.pop(0)
	check_valid(['fn'], next_token.type)

	# Retrieve the next token, check that it's an ID
	next_token = token_list.pop(0)
	check_valid(['ID'], next_token.type)

	# We know it's an ID now, so store its name
	function_name = next_token.info

	# Retrieve the next token, check it's an open-bracket
	next_token = token_list.pop(0)
	check_valid(['('], next_token.type)

	# Create a list for the argument names to be stored in
	arg_names = []

	# Grab the first token in the argument list
	next_token = token_list.pop(0)

	# Keep looking for argument names until we hit a ')' or an exception
	while next_token.type != ')':

		# The only valid tokens at this point are IDs and commas to delimit them
		check_valid([',', 'ID'], next_token.type)

		# Add the ID to the list of argument names
		if next_token.type == 'ID': arg_names.append(next_token.info)

		# Retrieve the next token
		next_token = token_list.pop(0)

	# Create a list for the function's statements
	statements = parse_statement_block(token_list)

	return FNDecl(function_name, arg_names, statements)

def parse_statement(token_list):
	"""
	This function is responsible for parsing a single statement. There are 6
	types of statement, each of which is handled differently, but in each case
	a Statement object is returned.
	"""

	# Retrieve the next token, and check that it's valid
	next_token = token_list.pop(0)
	check_valid(['print', 'for', 'return', 'if',
		'while', 'for', 'ID'], next_token.type)

	# If the token is a print, parse the corresponding expression and return it
	# in a Print AST object
	if next_token.type == 'print':

		# Grab the expression from the token list, create an ASTNode from it
		print_statement =  Print(parse_expression(token_list))

		# Return the generated ASTNode
		return print_statement

	# A return token is handled the same way as a print token, except the object
	# returned is a Return AST object as opposed to a Print
	elif next_token.type == 'return':

		# Grab the expression from the token list, create an ASTNode from it
		return_statement =  Return(parse_expression(token_list))

		# Return the generated ASTNode
		return return_statement

	# An if-statement requires a boolean expression of the form 'E COMP E' to be
	# parsed, followed by a statement block, which a call to 
	# parse_statement_block() handles, after which an 'else' token should be
	# parsed, and finally a futher statement block.
	elif next_token.type == 'if':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_expression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Create a list for the if's statements
		statements = parse_statement_block(token_list)

		# Retrieve the next token, check it's an 'else'
		next_token = token_list.pop(0)
		check_valid(['else'], next_token.type)

		# Create a list for the else's statements
		else_statements = parse_statement_block(token_list)

		# Return an If ASTNode representing the parsed if-statement
		return If(bool_expr, statements, else_statements)

	# While loops are simpler than if-statements to parse: just a boolean and
	# a single statement block to handle.
	elif next_token.type == 'while':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_expression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Create a list for the while-loop's statements
		statements = parse_statement_block(token_list)

		# Return a While ASTNode that corresponds to the parsed while-loop
		return While(bool_expr, statements)

	# For loops require parsing an assignment, a boolean expression, and a
	# further assignment statement, separated by commas, and finally the
	# statement block.
	elif next_token.type == 'for':

		# Parse the assignment statement of the for loop, checking that it is in
		# fact an assignment (parse_statement can return other stuff)
		assignment = parse_statement(token_list)
		if not issubclass(assignment.__class__, Assignment):
			raise Exception('Assignment statement required ' + \
				'in for-loop declaration')

		# Pop the comma, making sure that it is a comma
		next_token = token_list.pop(0)
		check_valid([','], next_token.type)

		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_expression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Pop the second comma, making sure that it is a comma
		next_token = token_list.pop(0)
		check_valid([','], next_token.type)

		# Parse the 'incrementor' - an Assignment - from the token list. Also
		# ensure that it applies to the same variable that was declared in the
		# for-loop's assignment, and again, assert its assignyness
		incrementor = parse_statement(token_list)
		if not issubclass(incrementor.__class__, Assignment):
			raise Exception('Incrementation of control variable required ' + \
				'in for-loop declaration')

		# Create a list for the for-loop's statements
		statements = parse_statement_block(token_list)

		# Return a For ASTNode representing the parsed for-loop
		return For(assignment, bool_expr, incrementor, statements)

	# If the token is an identifier, this indicates an assignment statement
	# - something with a ++, --, +=, -=, or an explicit ID <- E.
	elif next_token.type == 'ID':

		# Record the name of the ID
		assignee = next_token.info

		# Grab the next token - will be one of the 5 assignment operators if the
		# syntax is valid
		next_token = token_list.pop(0)

		# Switch on the assignment operator type so as to build the appropriate
		# syntax tree
		if next_token.type == '++':
			return Assignment(Identifier(assignee),
				ArithmeticExpr(Identifier(assignee), '+', IntegerLiteral(1)))

		elif next_token.type == '--':
			return Assignment(Identifier(assignee),
				ArithmeticExpr(Identifier(assignee), '-', IntegerLiteral(1)))

		elif next_token.type == '+=':
			return Assignment(Identifier(assignee),
				ArithmeticExpr(Identifier(assignee), '+',
					parse_expression(token_list)))

		elif next_token.type == '-=':
			return Assignment(Identifier(assignee),
				ArithmeticExpr(Identifier(assignee), '-',
					parse_expression(token_list)))

		elif next_token.type == '<-':
			return Assignment(Identifier(assignee),
				parse_expression(token_list))

		else: raise Exception('Unknown error occured - ' + \
			'could not parse valid assignment statement')

	else: raise Exception('Unknown error occured -' + \
		'could not parse valid statement')

def parse_statement_block(token_list):
	"""
	Parses a block of statements encased in curly brackets. Returns a list that
	contains a Statement object for each statement in the block, by calling
	parse_statement for each of them.
	"""

	# Create a list for the block's statements
	statements = []

	# Retrieve the opening curly-bracket
	next_token = token_list.pop(0)
	check_valid(['{'], next_token.type)

	# Repeatedly parse statements until a '}' is seen (note that '}'s within
	# the block that do not end this loop will be handled by a recursive call to
	# parse_statement_block, and will not cause the iteration to end early)
	while next_token.type != '}':

		# See if we need to process a semicolon after the statement
		expecting_semicolon = token_list[0].type in ['print', 'return', 'ID']

		statements.append(parse_statement(token_list))

		# Parse the semicolon if necessary
		if expecting_semicolon:
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

		# Peek at the next token for purposes of loop-control
		next_token = token_list[0]

	# Eat the final '}'. No need to check that it is a '}' because we would
	# never have left the loop if it wasn't
	token_list.pop(0)

	return statements

def parse_expression(token_list):
	"""
	This function, along with the parse_e_prime function, perform the job of
	parsing expressions according to the context-free grammar specified in the
	file CFG-parsable.txt, which can be found in the minty-spec directory of
	the project repository. This function handles the following productions:

	E -> INT E'
		| ID E'
		| (E) E'
		| ID(EA) E'

	Accordingly, this function will look for the part of the production that
	comes before the E' part, and try to parse it, before making a call to
	parse_e_prime to parse the E' part.
	"""

	next_token = token_list.pop(0)

	left_part = None

	if next_token.type == 'ID' and token_list[0].type == '(':
		# Record the name of the function
		fn_name = next_token.info

		# Get rid of the open bracket, we don't need it
		token_list.pop(0)

		# Create a list for the passed arguments (expressions) to go in
		arguments = []

		# Peek at the top of the token_list, otherwise we might enter the
		# argument-collecting loop erroneously
		next_token = token_list[0]

		# Keep processing an arg, then a comma, until the close-bracket is
		# reached. The args are added to the list, the commas are thrown away
		while not next_token.type == ')':
			arguments.append(parse_expression(token_list))

			# Get the next token - should be a ',' if there are more args, or a
			# ')' if not
			next_token = token_list.pop(0)
			check_valid([',', ')'], next_token.type)

		# Clear the ')' from the token list
		token_list.pop(0)

		left_part = FNCall(fn_name, arguments)

	elif next_token.type == 'ID':
		left_part = Identifier(next_token.info)

	elif next_token.type == 'INT':
		left_part = IntegerLiteral(int(next_token.info))

	elif next_token.type == '(':
		left_part = parse_expression(token_list)

		# Process the close-bracket
		next_token = token_list.pop(0)
		check_valid([')'], next_token.type)

	else: raise Exception('Could not parse E production, \'' + \
		next_token.type + '\' not expected')

	e_prime = parse_e_prime(token_list)

	if e_prime is None:
		return left_part

	elif e_prime.op in arithmetic_ops and not e_prime.is_ternary():
		return ArithmeticExpr(left_part, e_prime.op, e_prime.exp)

	elif e_prime.op in boolean_ops and e_prime.is_ternary():
		return Ternary(
			BoolExpression(left_part, e_prime.op, e_prime.exp),
			e_prime.true_exp,
			e_prime.false_exp)

	elif e_prime.op in boolean_ops:
		return BoolExpression(left_part, e_prime.op, e_prime.exp)

	else: raise Exception('E\' production parsed incorrectly: ' + str(e_prime))

def parse_e_prime(token_list):
	"""
	This function, along with the parse_e_prime function, perform the job of
	parsing expressions according to the context-free grammar specified in the
	file CFG-parsable.txt, which can be found in the minty-spec directory of
	the project repository. This function handles the following productions:
	
	E' -> (epsilon)
		| + E
		| - E
		| * E
		| / E
		| % E
		| COMP E ? E : E

	It also handles the 'COMP E' parts of the following productions:

	ST -> if E COMP E { STMTS } else { STMTS } 
		| while E COMP E { STMTS }
		| for ID <- E, E COMP E, INCR { STMTS }

	Accordingly, this function will look for an operator (either an arithmetic
	operator, or a boolean comparison) and in the case of an arithmetic operator
	it will then try to parse a single expression. If a boolean operation is
	found (produced by the COMP nonterminal) then the function will attempt to
	find a '?' indicating a ternary (although its absence is not an error).
		It will return an EPrime object, which represents the production. EPrime
	objects are not ASTNodes but are temporary stores for objects that
	parse_expression will build into ASTNodes.
	"""
	
	# Peek at the next token (don't pop it yet - it might be needed elsewhere)
	next_token = token_list[0]

	if next_token.type in arithmetic_ops:
		# We can pop the token in this case
		token_list.pop(0)

		return EPrime(next_token.type, parse_expression(token_list))

	elif next_token.type in boolean_ops:
		# We can pop the token in this case
		token_list.pop(0)

		# Record the op type
		op = next_token.type

		# Parse the initial expression
		exp = parse_expression(token_list)

		# Peek at the next token
		next_token = token_list[0]

		# If it's a ternary...
		if next_token.type == '?':
			# We can pop the token in this case
			token_list.pop(0)

			# Grab the expression the ternary evaluates to when true
			true_exp = parse_expression(token_list)

			# Grab the colon
			next_token = token_list.pop(0)
			check_valid([':'], next_token.type)

			# Grab false expression
			false_exp = parse_expression(token_list)

			# Return a ternary EPrime with extra expressions
			return EPrime(op, exp, true_exp=true_exp, false_exp=false_exp)

		# If it's not a ternary, return an EPrime object without true_exp and
		# false_exp values
		else: return EPrime(op, exp)

	# Any other tokens indicates the epsilon production, so return None
	else: return None