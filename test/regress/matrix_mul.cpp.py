#!/usr/bin/python

import shtest, sys
from common import *


def matmult(m1, m2):
    ncols1 = len(m1)
    nrows1 = 1
    if type(m1[0]) == tuple:
        nrows1 = len(m1)
        ncols1 = len(m1[0])
    else:
        m1 = [m1] # temporarily promote this vector to a matrix
    ncols2 = len(m2)
    if type(m2[0]):
        ncols2 = len(m2[0])
    else:
        m2 = [m2] # temporarily promote this vector to a matrix

    res = [[None] * ncols2 for i in range(nrows1)]
        
    for row1 in range(nrows1):
        sum = 0
        for col2 in range(ncols2):
            sum = 0
            for col1 in range(ncols1):
                sum += m1[row1][col1]*m2[col1][col2]
            res[row1][col2] = sum

    if nrows1 > 1:
        return res
    else:
        return res[0] # return a vector
        
def mul(p, q, types=[]):
    result = matmult(p, q)
    return shtest.make_test(result, [p, q], types)

test = shtest.ImmediateTest('mul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*', 2))
test.add_test(mul(((0.0, 1.0, 2.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
#test.add_test(mul((0.5, 1.0, 2.0), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)))) # not implemented yet?
test.output(sys.stdout)
