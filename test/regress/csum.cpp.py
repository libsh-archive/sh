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

test.output(sys.stdout)
