#!/usr/bin/python

import shtest, sys, common

def pow_test(p, q, types=[]):
    result = [pow(a, b) for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('pow', 2)
test.add_call(shtest.Call(shtest.Call.call, 'pow', 2))

# zero
test.add_test(pow_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0)))
test.add_test(pow_test((0.0, 0.0, 0.0, 0.0), (0.2, 0.5, 0.9, 1.5)))

# one to any power
test.add_test(pow_test((1.0, 1.0, 1.0, 1.0), (0.0, 1.0, 2.0, 5.0)))
test.add_test(pow_test((1.0, 1.0, 1.0, 1.0), (-0.0, -1.0, -2.0, -5.0)))
test.add_test(pow_test((1.0, 1.0, 1.0, 1.0), (0.2, 0.5, 0.9, 1.5)))
test.add_test(pow_test((1.0, 1.0, 1.0, 1.0), (-0.2, -0.5, -0.9, -1.5)))

# normal
test.add_test(pow_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0)))
test.add_test(pow_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 0.9, 2.3)))
test.add_test(pow_test((2.0, 3.0, 2.0, 3.0), (-0.0, -1.0, -2.0, -4.0)))

test.output(sys.stdout)
