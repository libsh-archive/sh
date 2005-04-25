#!/usr/bin/python

import shtest, sys, math

def csum(l, types=[]):
    return shtest.make_test((sum(l),), [l], types)

test = shtest.StreamTest('csum', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sum', 1))
test.add_test(csum((1.5, 1.0, 2.718, 3.14)))
test.add_test(csum((1.5,)))
test.add_test(csum((1.5, 1.0, 2.718, 3.14), ['d', 'd']))
test.add_test(csum((1.5,), ['d', 'd']))
test.add_test(csum((1, 2, 3, 4), ['i', 'i']))
test.add_test(csum((3,), ['i', 'i']))

test.add_test(csum((1.0, 2.0, 3.0)))
test.add_test(csum((1.0, 2.0, -3.0)))
test.add_test(csum((1.0, -2.0, -3.0)))
test.add_test(csum((-1.0, -2.0, -3.0)))
test.add_test(csum((-1.0, -2.0, 3.0)))
test.add_test(csum((-1.0, 2.0, 3.0)))

test.add_test(csum((0.2, 2.7, 3.23)))
test.add_test(csum((0.2, 2.7, -3.23)))
test.add_test(csum((0.2, -2.7, -3.23)))
test.add_test(csum((-0.2, -2.7, -3.23)))
test.add_test(csum((-0.2, -2.7, 3.23)))
test.add_test(csum((-0.2, 2.7, 3.23)))

test.add_test(csum((0.0,)))
test.add_test(csum((0.0, 0.0, 0.0)))
test.add_test(csum((0.0, 0.0, 0.0, 0.0)))
test.add_test(csum((0.0, 0.0, 0.0, 0.0)))

test.add_test(csum((1.0,)))
test.add_test(csum((-1.0,)))
test.add_test(csum((1000.0,)))
test.add_test(csum((-1000.0,)))
test.add_test(csum((0.5,)))
test.add_test(csum((-0.5,)))

test.output(sys.stdout)
