#!/usr/bin/python

from math import sqrt
import shtest, sys, common

def length(a, types=[]):
    s = 0
    for x in a:
        s += x*x
    result = sqrt(s)
    return shtest.make_test([result], [a], types)

test = shtest.StreamTest('length', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length', 1))

test.add_test(length((1.0, 2.0, 3.0)))
test.add_test(length((1.0, 2.0, -3.0)))
test.add_test(length((1.0, -2.0, -3.0)))
test.add_test(length((-1.0, -2.0, -3.0)))
test.add_test(length((-1.0, -2.0, 3.0)))
test.add_test(length((-1.0, 2.0, 3.0)))

test.add_test(length((0.2, 2.7, 3.23)))
test.add_test(length((0.2, 2.7, -3.23)))
test.add_test(length((0.2, -2.7, -3.23)))
test.add_test(length((-0.2, -2.7, -3.23)))
test.add_test(length((-0.2, -2.7, 3.23)))
test.add_test(length((-0.2, 2.7, 3.23)))

test.add_test(length((0.0,)))
test.add_test(length((0.0, 0.0, 0.0)))
test.add_test(length((0.0, 0.0, 0.0, 0.0)))
test.add_test(length((0.0, 0.0, 0.0, 0.0)))

test.add_test(length((1.0,)))
test.add_test(length((-1.0,)))
test.add_test(length((1000.0,)))
test.add_test(length((-1000.0,)))
test.add_test(length((0.5,)))
test.add_test(length((-0.5,)))

test.output(sys.stdout)
