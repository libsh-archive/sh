#!/usr/bin/python

import shtest, sys, common

from common import *

def pow_test(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [pow(a, b) for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [pow(p, b) for b in q]
    else:
        result = [pow(a, q) for a in p]
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

# pow(1, N)
test.add_test(pow_test(0.0, (0.2, 0.5, 0.9, 1.5)))
test.add_test(pow_test(2.0, (0.0, 1.0, 2.0, 5.0)))
test.add_test(pow_test(3.0, (0.1, 0.5, 0.9, 2.3)))
test.add_test(pow_test(1.0, (-0.0, -1.0, -2.0, -4.0)))

# pow(N, 1)
test.add_test(pow_test((0.2, 0.5, 0.9, 1.5), 0.0))
test.add_test(pow_test((0.0, 1.0, 2.0, 5.0), 2.0))
test.add_test(pow_test((0.1, 0.5, 0.9, 2.3), 3.0))
test.add_test(pow_test((-0.0, -1.0, -2.0, -4.0), 1.0))

test.output(sys.stdout)
