#!/usr/bin/python

from math import log

import shtest, sys, common

def log2_test(p, types=[]):
    result = [log(a,2) for a in p]
    return shtest.make_test(result, [p], types)

test = shtest.StreamTest('log2', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log2', 1))

test.add_test(log2_test((0.0001, 1.0, 2.0, 4.0)))
test.add_test(log2_test((0.1, 0.25, 0.3, 0.5)))
test.add_test(log2_test((2.3, 3.8, 10.0, 30.0)))
test.add_test(log2_test((0.95, 1.1, 2.9, 5000.0)))

test.output(sys.stdout)
