# Python default params trap #
## Intro ##
foo.py:

	def foo(bar=[]):
			bar.append('bar')
			print bar

		foo()
		foo()
		foo()

output:

	['bar']
	['bar', 'bar']
	['bar', 'bar', 'bar']


## Expalation ##
This is slightly not expected result can be fully explained by the fact that bar got as default parameter a reference to a list, not list itself. So each time we call a foo we  deal with the same list.
When interpreted came to a function definition it executed a code bar=[]. And then this code is not executed anymore.

## Prof ##
Lest change our foo.py a bit and then execute it:

	def make_a_list():
		print 'making list'
		return []


	def foo(bar=make_a_list()):
		bar.append('bar')
		print bar

	foo()
	foo()
	foo()


output:

	making list
	['bar']
	['bar', 'bar']
	['bar', 'bar', 'bar']

As we see - make_a_list was called only once when compiler come to a point of function definition.
