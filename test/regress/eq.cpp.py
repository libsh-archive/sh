#!/usr/bin/python

# Tests the equal operator

import shtest, sys, common

def eq(p, q, types=[]):
    result = [(a == b) + 0 for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('eq', 2)
test.add_call(shtest.Call(shtest.Call.infix, '==', 2))

test.add_test(eq((1.0,), (2.0,)))
test.add_test(eq((1.0,), (1.0,)))
test.add_test(eq((3.0,), (1.0,)))

test.add_test(eq((1.0, 2.0, 3.0), (2.0, 1.0, 3.0)))
test.add_test(eq((-1.0, -2.0, -3.0), (-2.0, -1.0, -3.0)))
test.add_test(eq((1.0, -2.0, 3.0), (-2.0, 1.0, -3.0)))

test.output(sys.stdout)
