#!/usr/bin/python

import shtest, sys, math

def cmul(l, types=[]):
    return shtest.make_test((reduce(lambda x, y: x * y, l, 1),), [l], types)

test = shtest.StreamTest('cmul', 1)
test.add_call(shtest.Call(shtest.Call.call, 'prod', 1))
test.add_test(cmul((1.5, 1.0, 2.718, 3.14)))
test.add_test(cmul((1.5,)))
test.add_test(cmul((1.5, 1.0, 2.718, 3.14), ['d', 'd']))
test.add_test(cmul((1.5,), ['d', 'd']))
test.add_test(cmul((1, 2, 3, 4), ['i', 'i']))
test.add_test(cmul((3,), ['i', 'i']))

test.add_test(cmul((1.0, 2.0, 3.0)))
test.add_test(cmul((1.0, 2.0, -3.0)))
test.add_test(cmul((1.0, -2.0, -3.0)))
test.add_test(cmul((-1.0, -2.0, -3.0)))
test.add_test(cmul((-1.0, -2.0, 3.0)))
test.add_test(cmul((-1.0, 2.0, 3.0)))

test.add_test(cmul((0.2, 2.7, 3.23)))
test.add_test(cmul((0.2, 2.7, -3.23)))
test.add_test(cmul((0.2, -2.7, -3.23)))
test.add_test(cmul((-0.2, -2.7, -3.23)))
test.add_test(cmul((-0.2, -2.7, 3.23)))
test.add_test(cmul((-0.2, 2.7, 3.23)))

test.add_test(cmul((0.0,)))
test.add_test(cmul((0.0, 0.0, 0.0)))
test.add_test(cmul((0.0, 0.0, 0.0, 0.0)))
test.add_test(cmul((0.0, 0.0, 0.0, 0.0)))

test.add_test(cmul((1.0,)))
test.add_test(cmul((-1.0,)))
test.add_test(cmul((1000.0,)))
test.add_test(cmul((-1000.0,)))
test.add_test(cmul((0.5,)))
test.add_test(cmul((-0.5,)))


test.output(sys.stdout)
