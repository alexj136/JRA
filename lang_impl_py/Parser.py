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
		pass
	elif next_token.type == 'while':
		pass
	elif next_token.type == 'for':
		pass

	# If the token is an identifier, this indicates an INCR statement (something
	# with a ++, --, += or -=). The parse_INCR function requires said
	# identifier to be present, so stick it back on the list and call parse_INCR
	elif next_token.type == 'ID':
		token_list.insert(0, next_token)
		return parse_INCR(token_list)

	else: raise Exception('Unknown error occured -' + \
		'could not parse valid statement')

def parse_INCR(token_list):
	pass

def parse_Expression(token_list):
	pass