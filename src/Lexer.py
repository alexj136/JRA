import string

class LexerResult(object):
	"""
	Empty class for Token & LexerResult to extend
	"""
	pass

class Token(LexerResult):
	"""
	Representation of a token object, with __str__ method for printing etc.
	"""
	def __init__(self, type, info):
		self.type = type
		self.info = info

	def __str__(self):
		return 'T_' + self.type + ('' if self.info is None else '_' + self.info)

class LexerError(LexerResult):
	"""
	Class that denotes lexical errors in a program
	"""
	def __init__(self, bad_syntax):
		self.bad_syntax = bad_syntax

	def __str__(self):
		return 'Lexical error: \'' + self.bad_syntax + '\' is not a valid lexeme'

# The transition table represents the finite state machine that describes the
# syntax of the language
transition_table = [#                                                 abcdjkx
# e   f   g   h   i   l   m   n   o   p   q   r   s   t   u   v   w   yzCAPS_   0-9   {   }   (   )   *   /   %   ,   ;   ?   :   <   >   =   +   -   !
[ 49,  1, 49, 49,  4, 49, 49, 49, 49,  2, 49,  3, 49, 49, 49, 49,  5, 49,       48,   37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 23, 26, 36, 28, 31, 34], # state 0
[ 49, 49, 49, 49, 49, 49, 49, 17,  6, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 1
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  8, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 2
[ 12, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 3
[ 49, 18, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 4
[ 49, 49, 49, 19, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 5
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  7, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 6
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 7
[ 49, 49, 49, 49,  9, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 8
[ 49, 49, 49, 49, 49, 49, 49, 10, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 9
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 11, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 10
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 11
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 13, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 12
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 14, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 13
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 15, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 14
[ 49, 49, 49, 49, 49, 49, 49, 16, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 15
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 16
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 17
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 18
[ 49, 49, 49, 49, 20, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 19
[ 49, 49, 49, 49, 49, 21, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 20
[ 22, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 21
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 22
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 25, -1, 24, -1], # state 23
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 24
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 25
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 27, -1, -1, -1], # state 26
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 27
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 29, -1, -1], # state 28
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 29
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 30
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, 32, -1], # state 31
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 32
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 33
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 35, -1, -1, -1], # state 34
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 35
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 36
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 37
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 38
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 39
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 40
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 41
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 42
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 43
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 44
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 45
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 46
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 47
[ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       48,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1], # state 48
[ 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1]] # state 49

# The char_indices dictionary maps characters in the language to columns in the
# transition table so that the table can be 'traversed' like a finite state
# machine
char_indices = {
	'a': 17, 'b': 17, 'c': 17, 'd': 17, 'e':  0, 'f':  1, 'g':  2,
	'h':  3, 'i':  4, 'j': 17, 'k': 17, 'l':  5, 'm':  6, 'n':  7,
	'o':  8, 'p':  9, 'q': 10, 'r': 11, 's': 12, 't': 13, 'u': 14,
	'v': 15, 'w': 16, 'x': 17, 'y': 17, 'z': 17, 'A': 17, 'B': 17,
	'C': 17, 'D': 17, 'E': 17, 'F': 17, 'G': 17, 'H': 17, 'I': 17,
	'J': 17, 'K': 17, 'L': 17, 'M': 17, 'N': 17, 'O': 17, 'P': 17,
	'Q': 17, 'R': 17, 'S': 17, 'T': 17, 'U': 17, 'V': 17, 'W': 17,
	'X': 17, 'Y': 17, 'Z': 17, '0': 18, '1': 18, '2': 18, '3': 18,
	'4': 18, '5': 18, '6': 18, '7': 18, '8': 18, '9': 18, '_': 17,
	'{': 19, '}': 20, '(': 21, ')': 22, '*': 23, '/': 24, '%': 25,
	':': 26, ';': 27, '?': 28, ',': 29, '<': 30, '>': 31, '=': 32,
	'+': 33, '-': 34, '!': 35 }

# final_mapping maps final states to token types so that after processing some
# text, we can determine what token we have seen by passing in the final state
final_mapping = {
	0:    -1, 1:      'ID', 2:     'ID', 3:   'ID', 4:  'ID', 
	5:  'ID', 6:      'ID', 7:    'FOR', 8:   'ID', 9:  'ID', 
	10: 'ID', 11:  'PRINT', 12:    'ID', 13:  'ID', 14: 'ID', 
	15: 'ID', 16: 'RETURN', 17:    'FN', 18:  'IF', 19: 'ID', 
	20: 'ID', 21:     'ID', 22: 'WHILE', 23:   '<', 24: '<-', 
	25: '<=', 26:      '>', 27:    '>=', 28:   '+', 29: '++', 
	30: '+=', 31:      '-', 32:    '--', 33:  '-=', 34:   -1, 
	35: '!=', 36:      '=', 37:     '{', 38:   '}', 39:  '(', 
	40:  ')', 41:      '*', 42:     '/', 43:   '%', 44:  ',', 
	45:  ';', 46:      '?', 47:     ':', 48: 'INT', 49: 'ID' }

valueless_tokens = ['fn', 'for', 'if', 'while', 'print', 'return', '<', '<-',
	'<=', '>', '>=', '+', '++', '+=', '-', '--', '-=', '!=', '=', '{', '}', '(',
	')', '*', '/', '%',',', ';', '?', ':']

def get_next_token(text):
	"""
	Retrieves the first token present in the given text. Also trims any
	processed text from the input text. The Token (or LexerError) found is
	returned in a tuple with the remaining text:
	(Token_or_LexerError, remaining_text)
	"""

	string_builder = ''
	final_state = 0
	current_state = 0

	# Clear leading whitespace from input
	text = text.lstrip(string.whitespace)
	
	# If there is no text left after clearing the whitespace, return None as
	# there are no tokens left
	if text == '': return (None, None)

	while current_state != -1 and len(text) > 0 and \
		text[0] in char_indices.keys():

		# Get the next character from the input
		next_char = text[0]

		# get the index of the next state
		next_state = transition_table[current_state][char_indices[next_char]]

		# If the next state is non-final...
		if next_state != -1:
			# Add the next char to the string builder
			string_builder = string_builder + next_char

			# Record the current state as final
			final_state = next_state

			# Remove the character we just handled from the input
			text = text[1:]

		# Update the current_state variable
		current_state = next_state

	# At this point we have finished walking through the finite state machine.
	# Now the result of the walk must be analysed and the appropriate output
	# returned.

	# The info for the token only needs recording if it's an ID, INT or invalid
	# syntax
	info = None if string_builder in valueless_tokens else string_builder

	# Return null/None as the token if the input was an empty string or
	# lead with characters not found in the alphabet of the language:
	if final_mapping[final_state] == -1:

		#The while loop allows all erroneous text to be processed in one go
		while len(text) > 0 and text[0] not in char_indices.keys():
			info = info + text[0]
			text = text[1:]
		return (LexerError(info), text)

	# Otherwise, return a tuple with the new token, and the remaining text that
	# has not been processed
	else: return (Token(final_mapping[final_state], info), text)

def lex(text):
	"""
	Scans some text, and returns either a list of Tokens, if there were no
	lexical errors, or a list of LexerErrors, if lexical errors were found
	"""
	tokens = []
	errors = []

	# Get the first token
	next_token, text = get_next_token(text)

	# Loop while we haven't finished processing the tex
	while next_token is not None:

		# If the token is valid, add it to the token list
		if issubclass(next_token.__class__, Token):
			tokens.append(next_token)

		# If not, add it to the errors list
		else:
			errors.append(next_token)

		# Get the next token
		next_token, text = get_next_token(text)

	# Return the tokens if there were no errors, otherwise return the errors
	return tokens if len(errors) == 0 else errors