#!/usr/bin/python

from math import sqrt

import shtest, sys, common

def rsqrt_test(p, types=[]):
    result = [1.0 / sqrt(a) for a in p]
    return shtest.make_test(result, [p], types)

test = shtest.StreamTest('rsqrt', 1)
test.add_call(shtest.Call(shtest.Call.call, 'rsqrt', 1))

test.add_test(rsqrt_test((0.5, 1.0, 2.0, 4.0)))
test.add_test(rsqrt_test((0.1, 0.25, 0.3, 0.5)))
test.add_test(rsqrt_test((2.3, 3.8, 2.0, 3.0)))
test.add_test(rsqrt_test((0.5, 1.0, 2.9, 50.0)))

test.output(sys.stdout)
