#!/usr/bin/python

import shtest, sys, common

def dot_product(a, b):
    return sum([x*y for (x,y) in zip(a,b)])
 
def dot(p, q, types=[]):
    result = dot_product(p, q)
    return shtest.make_test([result], [p, q], types)

def insert_into(test):
    test.add_test(dot((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)))
    test.add_test(dot((1.0, 1.0, 1.0), (1.0, 1.0, 1.0)))
    test.add_test(dot((1.0, 0.0, 1.0), (0.0, 1.0, 0.0)))
    test.add_test(dot((0.5, -1.0, 1.0), (0.5, -1.5, -6.0)))

test = shtest.StreamTest('dot', 2)
test.add_call(shtest.Call(shtest.Call.infix, '|', 2))
test.add_call(shtest.Call(shtest.Call.call, 'dot', 2))
insert_into(test)
test.output(sys.stdout)
