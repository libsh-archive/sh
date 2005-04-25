#!/usr/bin/python

from common import cross_product
import shtest, sys, math, common

def cross_test(p, q, types=[]):
    result = cross_product(p, q)
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('cross', 2)
test.add_call(shtest.Call(shtest.Call.call, 'cross', 2))

test.add_test(cross_test((0.0, 0.0, 0.0), (0.0, 0.0, 0.0)))
test.add_test(cross_test((0.0, 0.0, 0.0), (1.0, 1.0, 1.0)))
test.add_test(cross_test((1.0, 1.0, 1.0), (0.0, 0.0, 0.0)))
test.add_test(cross_test((1.0, 1.0, 1.0), (1.0, 1.0, 1.0)))

test.add_test(cross_test((1.0, 1.0, 1.0), (1.5, 0.5, 2.0)))
test.add_test(cross_test((10.2, 1.4, 3.2), (1.0, 1.0, 1.0)))
test.add_test(cross_test((50.0, 100.0, 0.001), (0.3, 1.1, 0.95)))

test.add_test(cross_test((-1.0, -1.0, -1.0), (1.5, 0.5, 2.0)))
test.add_test(cross_test((-10.2, -1.4, -3.2), (1.0, 1.0, 1.0)))
test.add_test(cross_test((-50.0, -100.0, -0.001), (0.3, 1.1, 0.95)))

test.add_test(cross_test((1.0, 1.0, 1.0), (-1.5, -0.5, -2.0)))
test.add_test(cross_test((10.2, 1.4, 3.2), (-1.0, -1.0, -1.0)))
test.add_test(cross_test((50.0, 100.0, 0.001), (-0.3, -1.1, -0.95)))

test.add_test(cross_test((-1.0, 1.0, 1.0), (-1.5, 0.5, -2.0)))
test.add_test(cross_test((10.2, -1.4, 3.2), (1.0, -1.0, 1.0)))
test.add_test(cross_test((-50.0, 100.0, -0.001), (0.3, 1.1, -0.95)))

test.output(sys.stdout)
