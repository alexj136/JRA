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

def semicolon_expected(token_list):
	return True if \
		token_list[0].type == 'print' or \
		token_list[0].type == 'return' or \
		token_list[0].type == 'ID' \
	else False

def parse_program(token_list):

	function_list = []

	while token_list != []:
		function_list.append(parse_function(token_list))

	return Program(fuction_list)

def parse_function(token_list):

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
	statements = parse_statement_block(token_list)

	return FNDecl(name, arg_names, statements)

def parse_statement(token_list):

	# Retrieve the next token, and check that it's valid
	next_token = token_list.pop(0)
	check_valid(['print', 'for', 'return',
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

	elif next_token.type == 'if':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_boolean(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Create a list for the if's statements
		if_statements = parse_statement_block(token_list)

		# Retrieve the next token, check it's an 'else'
		next_token = token_list.pop(0)
		check_valid(['else'], next_token.type)

		# Create a list for the else's statements
		else_statements = parse_statement_block(token_list)

		# Return an If ASTNode representing the parsed if-statement
		return If(bool_expr, if_statements, else_statements)

	elif next_token.type == 'while':
		
		# Parse the BoolExpression, assert that it is a BoolExpression
		bool_expr = parse_boolean(token_list)
		if not issubclass(bool_expr.__class__, BoolExpression):
			raise Exception('Boolean expression required ' + \
				'in for-loop declaration')

		# Create a list for the while-loop's statements
		statements = parse_statement_block(token_list)

		# Return a While ASTNode that corresponds to the parsed while-loop
		return While(bool_expr, statements)

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
		bool_expr = parse_boolean(token_list)
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
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'+', IntegerLiteral(1)))

		elif next_token.type == '--':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'-', IntegerLiteral(1)))

		elif next_token.type == '+=':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'+', parse_expression(token_list)))

		elif next_token.type == '-=':
			return Assignment(assignee, ArithmeticExpr(Identifier(assignee),
				'-', parse_expression(token_list)))

		elif next_token.type == '<-':
			return Assignment(assignee, parse_expression(token_list))

		else: raise Exception('Unknown error occured - ' + \
			'could not parse valid assignment statement')

	else: raise Exception('Unknown error occured -' + \
		'could not parse valid statement')

def parse_statement_block(token_list):
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
		expecting_semicolon = semicolon_expected(token_list)

		statements.append(parse_statement(token_list))

		# Parse the semicolon if necessary
		if expecting_semicolon:
			next_token = token_list.pop(0)
			check_valid([';'], next_token.type)

		# Peek at the next token for purposes of loop-control
		next_token = token_list[0]

	# Eat the final '}'. No need to check that it is a '}' because we would
	# never have left the loop if it wasn't
	next_token = token_list.pop(0)

	return statements

def parse_expression(token_list):

	next_token = token_list.pop(0)

	if next_token.type == 'ID':
		pass
	elif next_token.type == 'INT':
		return IntegerLiteral(int(next_token.info))
	elif next_token.type == '(':
		pass
	else:
		pass

def parse_e_prime(token_list):

	next_token = token_list.pop(0)

	if next_token.type == '+':
		pass
	elif next_token.type == '-':
		pass
	elif next_token.type == '*':
		pass
	elif next_token.type == '/':
		pass
	elif next_token.type == '%':
		pass
	elif next_token.type == '=':
		pass
	elif next_token.type == '!=':
		pass
	elif next_token.type == '<':
		pass
	elif next_token.type == '>':
		pass
	elif next_token.type == '<=':
		pass
	elif next_token.type == '>=':
		pass
	else:
		pass

def parse_ternary(token_list):
	pass

def parse_boolean(token_list):
	pass