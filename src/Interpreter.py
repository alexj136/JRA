from AST import *

class Interpreter(object):

	def __init__(self):
		self.function_map = {}

	def interpret(tree):

		if tree.__class__.__name__ == 'Program':
			# Load the functions in the program into the function map
			for fn in tree:
				self.function_map[fn.name] = fn

			# If the given program has a main function, execute it
			if 'main' in function_map:
				interpret(function_map['main'])

		elif tree.__class__.__name__ == 'FN':
			# NOT SURE THAT WE NEED TO DO ANYTHING AT ALL
			# Create a mapping from variable identifiers to values
			in_scope_variables = {}
			for arg in tree.args:
				pass

		elif tree.__class__.__name__ == 'Statement':
			# Check which kind of Statement this is
			if tree.__class__.__name__ == 'Assignment':
				pass
			elif tree.__class__.__name__ == 'For':
				pass
			elif tree.__class__.__name__ == 'BoolStatement':
				# Check which kind of BoolStatement this is
				if tree.__class__.__name__ == 'While':
					pass
				elif tree.__class__.__name__ == 'If':
					pass
				else:
					pass
			else:
				pass
		elif tree.__class__.__name__ == 'Expression':
			# Check which kind of Expression this is
			if tree.__class__.__name__ == 'ArithmeticExpr':
				# Check which kind of operator this ArithmeticExpr has
				if tree.op == '<':
					pass
			elif tree.__class__.__name__ == 'Identifier':
				pass
			elif tree.__class__.__name__ == 'IntegerLiteral':
				return tree.value
			elif tree.__class__.__name__ == 'FNCall':
				pass
			else:
				pass
		else:
			pass