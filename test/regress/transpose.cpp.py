#!/usr/bin/python

import shtest, sys

def transpose(p, types=[]):
    nrows = len(p)
    ncols = len(p[0])
    result = [ [None] * nrows for i in range(ncols) ]
    for i in range(ncols):
        for j in range(nrows):
            result[i][j] = p[j][i]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(transpose(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0))))
    test.add_test(transpose(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(transpose(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0))))
    test.add_test(transpose(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(transpose(((-1000.0, 1.3333),(0.0, 1.0))))
    test.add_test(transpose(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2))))

    test.add_test(transpose(((1, 2, 3), (4, 5, 6),(-7, -8, -9))))
    test.add_test(transpose(((0.1, 0.1, 0.1), (-4.5, -4.5, -4.5),(70000, 70000, 70000)))) 

# Test matrix transpose in immediate mode
test = shtest.ImmediateTest('transpose', 1)
test.add_call(shtest.Call(shtest.Call.call, 'transpose', 1))
insert_into(test)
test.output(sys.stdout)
