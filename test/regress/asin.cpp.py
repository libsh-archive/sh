#!/usr/bin/python

import shtest, sys, math 

def asin(l, types=[]):
    return shtest.make_test([math.asin(x) for x in l], [l], types)

test = shtest.StreamTest('asin', 1)
test.add_call(shtest.Call(shtest.Call.call, 'asin', 1))
test.add_test(asin((0.0,)))
test.add_test(asin((0.3, 0.5, 0.8, 0.9)))
test.add_test(asin((1 - 0.1, -1 + 0.1, 0.0)))
test.add_test(asin((-0.5, -0.9, -0.1)))
test.add_test(asin((0.5, 0.6, 0.9)))

test.output(sys.stdout)
