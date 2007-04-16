#!/usr/bin/python

from common import dot_product
import shtest, sys, common
 
def dot(p, q, types=[]):
    result = dot_product(p, q)
    return shtest.make_test([result], [p, q], types)

def insert_into(test):
    test.add_test(dot((0.0,), (0.0,)))
    test.add_test(dot((0.0,), (1.0,)))
    test.add_test(dot((-1.0,), (1.0,)))
    test.add_test(dot((-1.0,), (-1.0,)))
    test.add_test(dot((5.0,), (8.0,)))
    test.add_test(dot((5000.40,), (-3450.34,)))

    test.add_test(dot((1.0, 2.0), (1.0, 2.0)))
    test.add_test(dot((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)))
    test.add_test(dot((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0)))
    test.add_test(dot((1.0, 1.0, 1.0), (1.0, 1.0, 1.0)))
    test.add_test(dot((1.0, 0.0, 1.0), (0.0, 1.0, 0.0)))
    test.add_test(dot((0.5, -1.0, 1.0), (0.5, -1.5, -6.0)))

    test.add_test(dot((0,), (0,), ['i', 'i', 'i']))
    test.add_test(dot((0,), (1,), ['i', 'i', 'i']))
    test.add_test(dot((-1,), (1,), ['i', 'i', 'i']))
    test.add_test(dot((-1,), (-1,), ['i', 'i', 'i']))
    test.add_test(dot((5,), (8,), ['i', 'i', 'i']))
    test.add_test(dot((5000,), (-3450,), ['i', 'i', 'i']))

    test.add_test(dot((1, 2), (1, 2), ['i', 'i', 'i']))
    test.add_test(dot((1, 2, 3), (1, 2, 3), ['i', 'i', 'i']))
    test.add_test(dot((1, 2, 3, 4), (1, 2, 3, 4), ['i', 'i', 'i']))
    test.add_test(dot((1, 1, 1), (1, 1, 1), ['i', 'i', 'i']))
    test.add_test(dot((1, 0, 1), (0, 1, 0), ['i', 'i', 'i']))

# Test the dot product in stream programs
test = shtest.StreamTest('dot', 2)
test.add_call(shtest.Call(shtest.Call.infix, '|', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the dot product in immediate mode
test = shtest.ImmediateTest('dot_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '|', 2))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
