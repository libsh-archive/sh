#!/usr/bin/python

import shtest, sys, common

def max_test(p, q, types=[]):
    result = [max(a, b) for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('max', 2)
test.add_call(shtest.Call(shtest.Call.call, 'max', 2))

test.add_test(max_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0)))
test.add_test(max_test((0.2, 0.5, 0.9, 1.5), (0, 0, 0, 0)))

test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0)))
test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 2.9, 2.3)))
test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (-0.5, -1.0, -2.0, -4.0)))
test.add_test(max_test((-2.0, -3.0, -2.0, -3.0), (-0.5, -1.0, -2.0, -4.0)))

test.output(sys.stdout)