from AST import *

def check_valid(expected, found):
	"""
	Checks that found is in expected, and if not, raises an exception with a
	friendly error message
	"""
	if found not in expected:
		str_bldr = expected.pop(len(expected - 1))
		if expected != []:
			str_bldr = expected.pop(len(expected - 1)) + ' or ' + str_bldr
			while expected != []:
				str_bldr = expected.pop(len(expected - 1)) + ', ' + str_bldr
		raise Exception('\'' + found + '\' found, ' + str_bldr + ' expected')

def parse_Program(token_list):

	function_list = []

	while token_list != []:
		function_list.append(parse_FNDecl(token_list))

	return Program(fuction_list)

def parse_Function(token_list):

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

	# Keep looking for argument names until we hit a ')' or an exception
	while next_token.type != ')':

		# Retrieve the next token - should be an ID
		next_token = token_list.pop(0)
		check_valid(['ID'], next_token.type)

		# Add the ID to the list of argument names
		arg_names.append(next_token.info)

		# Process the next token - make sure it's a ',' (incurs further
		# iteration) or ')' (ends iteration)
		next_token = token_list.pop(0)
		check_valid([',', ')'], next_token.type)

	# Retrieve the next token, check it's an open-curly-bracket
	next_token = token_list.pop(0)
	check_valid(['{'], next_token.type)

	# Create a list for the function's statements
	statements = []

	# Repeatedly parse statements until a '}' is seen (note that '}'s within the
	# function that do not end the function will be handled by the call to
	# parse_Statement and will not cause the iteration to end early)
	while next_token.type != '}':
		statements.append(parse_Statement(token_list))

	# Retrieve the next token, check it's an close-curly-bracket
	next_token = token_list.pop(0)
	check_valid(['}'], next_token.type)

	return FNDecl(name, arg_names, statements)

def parse_Statement(token_list):

	# Retrieve the next token, and check that it's valid
	next_token = token_list.pop(0)
	check_valid(['print', 'for', 'return',
		'while', 'for', 'ID'], next_token.type)

	# If the token is a print, parse the corresponding expression and return it
	# in a Print AST object
	if next_token.type == 'print':
		return Print(parse_Expression(token_list))

	# A return token is handled the same way as a print token, except the object
	# returned is a Return AST object as opposed to a Print
	elif next_token.type == 'return':
		return Print(parse_Expression(token_list))

	elif next_token.type == 'if':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_BoolExpression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Retrieve the next token, check it's an open-curly-bracket
		next_token = token_list.pop(0)
		check_valid(['{'], next_token.type)

		# Create a list for the if's statements
		if_statements = []

		# Parse statements until the end of the if-block is reached
		while next_token.type != '}':
			if_statements.append(parse_Statement(token_list))

			# Parse the semicolon
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

			# Peek at the next token for purposes of loop-control
			next_token = token_list[0]

		# Eat the '}'. No need to check that it is a '}' because we would
		# never have left the loop if it wasn't
		next_token = token_list.pop(0)

		# Retrieve the next token, check it's an 'else'
		next_token = token_list.pop(0)
		check_valid(['else'], next_token.type)

		# Retrieve the next token, check it's an open-curly-bracket
		next_token = token_list.pop(0)
		check_valid(['{'], next_token.type)

		# Create a list for the else's statements
		else_statements = []

		# Again, parse statements until the end of the else-block is reached
		while next_token.type != '}':
			else_statements.append(parse_Statement(token_list))

			# Parse the semicolon
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

			# Peek at the next token for purposes of loop-control
			next_token = token_list[0]

		# Eat the final '}'. No need to check that it is a '}' because we would
		# never have left the loop if it wasn't
		next_token = token_list.pop(0)

		# Return an If ASTNode representing the parsed if-statement
		return If(bool_expr, if_statements, else_statements)

	elif next_token.type == 'while':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_BoolExpression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Retrieve the next token, check it's an open-curly-bracket
		next_token = token_list.pop(0)
		check_valid(['{'], next_token.type)

		# Create a list for the while-loop's statements
		statements = []

		# Parse statements until the end of the while-loop is reached
		while next_token.type != '}':
			statements.append(parse_Statement(token_list))

			# Parse the semicolon
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

			# Peek at the next token for purposes of loop-control
			next_token = token_list[0]

		# Eat the '}'. No need to check that it is a '}' because we would
		# never have left the loop if it wasn't
		next_token = token_list.pop(0)

		# Return a While ASTNode that corresponds to the parsed while-loop
		return While(bool_expr, statements)

	elif next_token.type == 'for':

		# Parse the assignment statement of the for loop, checking that it is in
		# fact an assignment (parse_Statement can return other stuff)
		assignment = parse_Statement(token_list)
		if not issubclass(assignment.__class__, Assignment):
			raise Exception('Assignment statement required ' + \
				'in for-loop declaration')

		# Pop the comma, making sure that it is a comma
		next_token = token_list.pop(0)
		check_valid([','], next_token.type)

		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_BoolExpression(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Pop the second comma, making sure that it is a comma
		next_token = token_list.pop(0)
		check_valid([','], next_token.type)

		# Parse the 'incrementor' - an Assignment - from the token list. Also
		# ensure that it applies to the same variable that was declared in the
		# for-loop's assignment, and again, assert its assignyness
		incrementor = parse_Statement(token_list)
		if not issubclass(incrementor.__class__, Assignment):
			raise Exception('Incrementation of control variable required ' + \
				'in for-loop declaration')

		# Retrieve the next token, check it's an open-curly-bracket
		next_token = token_list.pop(0)
		check_valid(['{'], next_token.type)

		# Create a list for the for-loop's statements
		statements = []

		# Repeatedly parse statements until a '}' is seen (note that '}'s within
		# the for-loop that do not end this loop will be handled by the call to
		# parse_Statement that initiates the inner block of code, and will not
		# cause the iteration to end early)
		while next_token.type != '}':
			statements.append(parse_Statement(token_list))

			# Parse the semicolon
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

			# Peek at the next token for purposes of loop-control
			next_token = token_list[0]

		# Eat the final '}'. No need to check that it is a '}' because we would
		# never have left the loop if it wasn't
		next_token = token_list.pop(0)

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
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'+', IntegerLiteral(1)))

		elif next_token.type == '--':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'-', IntegerLiteral(1)))

		elif next_token.type == '+=':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'+', parse_Expression(token_list)))

		elif next_token.type == '-=':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'-', parse_Expression(token_list)))

		elif next_token.type == '<-':
			return Assignment(assignee, parse_Expression(token_list))

		else: raise Exception('Unknown error occured - ' + \
			'could not parse valid assignment statement')

	else: raise Exception('Unknown error occured -' + \
		'could not parse valid statement')

def parse_Expression(token_list):
	pass

def parse_BoolExpression(token_list):
	pass