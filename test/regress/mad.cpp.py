#!/usr/bin/python

from common import *

import shtest, sys, common

def mad(p, q, r, types=[]):
    result = [a * b + c for (a,b,c) in upzip3(p, q, r)]
    return shtest.make_test(result, [p, q, r], types)

test = shtest.StreamTest('mad', 3)
test.add_call(shtest.Call(shtest.Call.call, 'mad', 3))

test.add_test(mad((0.0, 1.0, 2.0), (3.0, 4.0, 5.0), (1.0, 1.1, 1.2)))
test.add_test(mad((1.0, 2.0, 3.0), (7.0,), (-1.0, 0.5, 10.0)))
test.add_test(mad((1.0,), (7.0,), (-5000.0,)))
test.add_test(mad((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0), (0.0, 0.0, 0.0)))

test.output(sys.stdout)
