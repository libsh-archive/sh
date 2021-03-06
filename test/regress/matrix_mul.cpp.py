#!/usr/bin/python

import shtest, sys, common
from common import *

def matmult(m1, m2):
    ncols1 = len(m1)
    nrows1 = 1
    if is_array(m1[0]):
        nrows1 = len(m1)
        ncols1 = len(m1[0])
    else:
        # promote this vector to a matrix (row vector)
        colvec = [[None] * len(m1)]
        for i in range(len(m1)):
            colvec[0][i] = m1[i]
        m1 = colvec

    ncols2 = 1
    nrows2 = len(m2)
    if is_array(m2[0]):
        nrows2 = len(m2)
        ncols2 = len(m2[0])
    else:
        # promote this vector to a matrix (column vector)
        colvec = [[None] for i in range(len(m2))]
        for i in range(len(m2)):
            colvec[i][0] = m2[i]
        m2 = colvec

    # Perform matrix expansion if necessary
    if ncols1 < nrows2:
        m1 = matrix_expand(m1, nrows1, nrows2, True)
        ncols1 = nrows2
    elif ncols1 > nrows2:
        m2 = matrix_expand(m2, ncols1, ncols2, True)
        nrows2 = ncols1
        
    res = [[None] * ncols2 for i in range(nrows1)]
        
    for row1 in range(nrows1):
        sum = 0
        for col2 in range(ncols2):
            sum = 0
            for col1 in range(ncols1):
                sum += m1[row1][col1]*m2[col1][col2]
            res[row1][col2] = sum

    if nrows1 == 1:
        return res[0] # return a row vector
    elif ncols2 == 1:
        r = [None for i in range(nrows1)]
        for i in range(nrows1):
            r[i] = res[i][0]
        return r # return a column vector
    else:
        return res

def scalar_matrix(s, m):
    nrows = len(m)
    ncols = len(m[0])

    res = [[None] * ncols for i in range(nrows)]
    
    for i in range(nrows):
        for j in range(ncols):
            res[i][j] = s * m[i][j]
    return res

def mul(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = matmult(p, q)
    elif is_array(q):
        result = scalar_matrix(p, q)
    elif is_array(p):
        result = scalar_matrix(q, p)
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=False):
    # Trivial cases
    test.add_test(mul(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(mul(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))

    # Same size
    test.add_test(mul(((0.0, 1.0, 2.0, 0.5),(0.0, 1.0, 0.0, 20.0),(0.0, 0.0, 1.0, -0.3),(1.0, 2.0, -0.334, 50.0)), ((1.0, 2.0, 3.0, 4.0),(3.0, 4.0, 5.0, 6.0),(5.0, 6.0, 7.0, 8.0),(9.0, -10.0, -11.0,-12.0))))
    test.add_test(mul(((0.0, 1.0, 2.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(mul(((0.0, 1.0),(0.0, 1.0)), ((1.0, 2.0),(3.0, 4.0))))
    
    # Different sizes
    if not modifying:
        test.add_test(mul(((34.0, 1.0),(0.5, -1.9),(1000.0, 0.012345)), ((1.0, 2.0, 3.0, 4.0),(5.0, 6.0, 7.0, 8.0))))

    # Scalar-matrix product
    if not modifying:
        test.add_test(mul(0.5, ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))

    # Matrix-scalar product
    test.add_test(mul(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)), -2.3))

    # Row vector-matrix product
    if not modifying:
        test.add_test(mul((0.0, 1.0, 2.0), ((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
        test.add_test(mul((0.0, 1.0, 2.0), ((1.0, 2.0),(3.0, 4.0),(5.0, 6.0))))

    # Matrix-column vector product
    if not modifying:
        test.add_test(mul(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)), (1.0, -0.3, 10.1)))
        test.add_test(mul(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0)), (1.0, -0.3, 10.1)))

    # Matrix expansion
    if not modifying:
        # 4x3 * 4x4 => first matrix should be expanded to 4x4
        test.add_test(mul(((0.0, 1.0, 2.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0),(1.0, 2.0, -0.334)), ((1.0, 2.0, 3.0, 4.0),(3.0, 4.0, 5.0, 6.0),(5.0, 6.0, 7.0, 8.0),(9.0, -10.0, -11.0,-12.0))))
        # 2x1 * 2x2 => first matrix should be expanded to 2x2
        test.add_test(mul(((-0.5,),(2.1,)), ((-1.0, 2.3),(30, -4))))
        # 2x3 * 2x2 => second matrix should be expanded to 3x2
        test.add_test(mul(((-0.5, 10.0, 1.2),(2.1, 1.2, 0.3)), ((-1.0, 2.3),(30, -4))))
    
# Test regular multiplication of matrices in immediate mode
test = shtest.ImmediateTest('mul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the modifying multiplication of matrices in immediate mode
test = shtest.ImmediateTest('mmul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
