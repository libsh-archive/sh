#!/usr/bin/python

import shtest, sys

def matdiv(m1, m2):
    nrows = len(m1)
    ncols = len(m1[0])
    res = [[None] * ncols for i in range(nrows)]
        
    for i in range(nrows):
        for j in range(ncols):
            res[i][j] = m1[i][j] / m2[i][j]
    return res

def div(p, q, types=[]):
    result = matdiv(p, q)
    return shtest.make_test(result, [p, q], types)

def insert_into(test):
    test.add_test(div(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(div(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(div(((-1.0, 0.5, 3.23333),(-3.0, 4.2, -0.1),(5000.0, -6.0, 7.1)), ((1.0, 1.0, 1.0),(1.0, 1.0, 1.0),(1.0, 1.0, 1.0))))
    test.add_test(div(((0.0, 1.0, 2.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(div(((-1000.0, 1.3333),(0.0, 1.0),(-0.1, 0.5)), ((1.0, 2.0),(3.0, 4.0),(5.0, 6.0))))
    test.add_test(div(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7)), ((1.0, 2.0, 3.0, 4.0),(3.0, 4.0, 5.0, 90.0),(5.0, 6.0, 7.0, -1.0))))

# Test regular division of matrices in immediate mode
test = shtest.ImmediateTest('div', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying division of matrices in immediate mode
test = shtest.ImmediateTest('mdiv', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/=', 2))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
