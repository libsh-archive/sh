#!/usr/bin/python

from math import pow

import shtest, sys, common

def exp10_test(p, types=[]):
    result = [pow(10, a) for a in p]
    return shtest.make_test(result, [p], types)

test = shtest.StreamTest('exp10', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp10', 1))

test.add_test(exp10_test((0.0, 1.0, 2.0, 4.0)))
test.add_test(exp10_test((0.1, 0.25, 0.3, 0.5)))
test.add_test(exp10_test((2.3, 3.8, -2.0, -3.0)))
test.add_test(exp10_test((-0.5, -1.0, 2.9, 50.0)))

test.output(sys.stdout)
