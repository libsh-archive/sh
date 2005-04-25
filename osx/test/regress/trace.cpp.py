#!/usr/bin/python

import shtest, sys

def trace(p, types=[]):
    nrows = len(p)
    result = 0
    for i in range(nrows):
        result += p[i][i]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(trace(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(trace(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0))))
    test.add_test(trace(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(trace(((-1000.0, 1.3333),(0.0, 1.0))))
    test.add_test(trace(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2))))

test = shtest.ImmediateTest('trace', 1)
test.add_call(shtest.Call(shtest.Call.call, 'trace', 1))
insert_into(test)
test.output(sys.stdout)
