#!/usr/bin/python

from common import dot_product
import shtest, sys, common
 
def dot(p, q, types=[]):
    result = dot_product(p, q)
    return shtest.make_test([result], [p, q], types)

def insert_into(test):
    test.add_test(dot((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)))
    test.add_test(dot((1.0, 1.0, 1.0), (1.0, 1.0, 1.0)))
    test.add_test(dot((1.0, 0.0, 1.0), (0.0, 1.0, 0.0)))
    test.add_test(dot((0.5, -1.0, 1.0), (0.5, -1.5, -6.0)))

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
