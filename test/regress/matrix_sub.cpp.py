#!/usr/bin/python

import shtest, sys

def matsub(m1, m2):
    nrows = len(m1)
    ncols = len(m1[0])
    res = [[None] * ncols for i in range(nrows)]
        
    for i in range(nrows):
        for j in range(ncols):
            res[i][j] = m1[i][j] - m2[i][j]
    return res

def sub(p, q, types=[]):
    result = matsub(p, q)
    return shtest.make_test(result, [p, q], types)

def insert_into(test):
    test.add_test(sub(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)),((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(sub(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(sub(((0.0, 1.0, 2.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(sub(((-1000.0, 1.3333),(0.0, 1.0),(-0.1, 0.5)), ((1.0, 2.0),(3.0, 4.0),(5.0, 6.0))))
    test.add_test(sub(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7)), ((1.0, 2.0, 3.0, 4.0),(3.0, 4.0, 5.0, 90.0),(5.0, 6.0, 7.0, -1.0))))

# Test regular subtraction of matrices in immediate mode
test = shtest.ImmediateTest('sub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying subtraction of matrices in immediate mode
test = shtest.ImmediateTest('msub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-=', 2))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
