#!/usr/bin/python

import shtest, sys

def matneg(m):
    nrows = len(m)
    ncols = len(m[0])
    res = [[None] * ncols for i in range(nrows)]
        
    for i in range(nrows):
        for j in range(ncols):
            res[i][j] = -(m[i][j])
    return res

def neg(p, types=[]):
    result = matneg(p)
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(neg(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(neg(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0))))
    test.add_test(neg(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(neg(((-1000.0, 1.3333),(0.0, 1.0),(-0.1, 0.5))))
    test.add_test(neg(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7))))

test = shtest.ImmediateTest('neg', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '-', 1))
insert_into(test)
test.output(sys.stdout)

