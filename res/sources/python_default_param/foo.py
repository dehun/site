def make_a_list():
    print 'making list'
    return []


def foo(bar=make_a_list()):
    bar.append('bar')
    print bar

foo()
foo()
foo()
