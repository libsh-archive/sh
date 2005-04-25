#!/usr/bin/python

# Tests the cond operator

import shtest, sys, common
from common import *

def cond(p, q, r, types=[]):
    result = []
    if is_array(p):
        for (a, b, c) in common.upzip3(p, q, r):
            if (a > 0):
                result.append(b)
            else:
                result.append(c)
    else:
        for (b, c) in common.upzip(q, r):
            if (p > 0):
                result.append(b)
            else:
                result.append(c)
        
    return shtest.make_test(result, [p, q, r], types)

test = shtest.StreamTest('cond', 3)
test.add_call(shtest.Call(shtest.Call.call, 'cond', 3))

# cond(1, 1, 1)
test.add_test(cond((10,), (1.0,), (0.0,)))
test.add_test(cond((1.0,), (1.0,), (0.0,)))
test.add_test(cond((0.5,), (1.0,), (0.0,)))
test.add_test(cond((0,), (1.0,), (0.0,)))
test.add_test(cond((-0.3,), (1.0,), (0.0,)))
test.add_test(cond((-3000,), (1.0,), (0.0,)))

# cond(4, 4, 4)
test.add_test(cond((1.4, 0.0, -45.0, 1024), (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))

# cond(1, N, N)
test.add_test(cond(1.4, (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))
test.add_test(cond(0.0, (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))

test.output(sys.stdout)
