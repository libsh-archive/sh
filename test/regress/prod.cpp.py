#!/usr/bin/python

import shtest, sys, common

def prod(a, types=[]):
    s = 1
    for x in a:
        s *= x
    result = s
    return shtest.make_test([result], [a], types)

test = shtest.StreamTest('prod', 1)
test.add_call(shtest.Call(shtest.Call.call, 'prod', 1))

test.add_test(prod((1.0, 2.0, 3.0)))
test.add_test(prod((1.0, 2.0, -3.0)))
test.add_test(prod((1.0, -2.0, -3.0)))
test.add_test(prod((-1.0, -2.0, -3.0)))
test.add_test(prod((-1.0, -2.0, 3.0)))
test.add_test(prod((-1.0, 2.0, 3.0)))

test.add_test(prod((0.2, 2.7, 3.23)))
test.add_test(prod((0.2, 2.7, -3.23)))
test.add_test(prod((0.2, -2.7, -3.23)))
test.add_test(prod((-0.2, -2.7, -3.23)))
test.add_test(prod((-0.2, -2.7, 3.23)))
test.add_test(prod((-0.2, 2.7, 3.23)))

test.add_test(prod((0.0,)))
test.add_test(prod((0.0, 0.0, 0.0)))
test.add_test(prod((0.0, 0.0, 0.0, 0.0)))
test.add_test(prod((0.0, 0.0, 0.0, 0.0)))

test.add_test(prod((1.0,)))
test.add_test(prod((-1.0,)))
test.add_test(prod((1000.0,)))
test.add_test(prod((-1000.0,)))
test.add_test(prod((0.5,)))
test.add_test(prod((-0.5,)))

test.output(sys.stdout)
