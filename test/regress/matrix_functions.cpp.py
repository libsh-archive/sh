#!/usr/bin/python

import shtest, sys

def submatrix(m, row, col):
    nrows = len(m) - 1
    ncols = len(m[0]) - 1
    result = [ [None] * ncols for i in range(nrows) ]

    r = 0
    for i in range(nrows + 1):
        if i != row:
            c = 0
            for j in range(ncols + 1):
                if j != col:
                    result[r][c] = m[i][j]
                    c += 1
            r += 1
    return result

def cofactor(m, row, col):
    if (len(m) == 1):
        return m[0][0]
    else:
        sign = 1
        if (row + col) % 2 != 0:
            sign = -1
        return sign * det(submatrix(m, row, col))

# This function is based on:
# http://mail.python.org/pipermail/edu-sig/2001-May/001187.html
def det(m):
    nrows = len(m)
    ncols = len(m[0])
    if (nrows == ncols):
        if (nrows == 1):
            result = m[0][0]
        elif (nrows == 2):
            result = m[0][0]*m[1][1] - m[0][1]*m[1][0]
        elif (nrows == 3):
            result = m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1]) - m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0]) + m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0])
        else:
            result = 0
            for i in range(ncols):
                result += m[0][i] * cofactor(m, 0, i)
        return result

def transpose(m):
    nrows = len(m)
    ncols = len(m[0])
    result = [ [None] * nrows for i in range(ncols) ]
    for i in range(ncols):
        for j in range(nrows):
            result[i][j] = m[j][i]
    return result

# Defined as the transpose of the cofactor matrix
def adjoint(m):
    nrows = len(m)
    ncols = len(m[0])
    result = [ [None] * ncols for i in range(nrows) ]
    for i in range(nrows):
        for j in range(ncols):
            result[i][j] = cofactor(m, i, j)
    return transpose(result)

# Sum of the diagonal entries
def trace(m):
    nrows = len(m)
    result = 0
    for i in range(nrows):
        result += m[i][i]
    return result

# Defined as: (1 / det(M)) * adj(M)
def inverse(m):
    nrows = len(m)
    ncols = len(m[0])
    result = [ [0] * ncols for i in range(nrows) ]

    determinant = det(m)
    if determinant == 0:
        return result
    
    adj = adjoint(m)
    for i in range(nrows):
        for j in range(ncols):
            result[i][j] = (1.0 / determinant) * adj[i][j]
    return result

def func(p, op, types=[]):
    result = op(p)
    return shtest.make_test(result, [p], types)

# for det(), adjoint() and inverse() -- only square matrices are allowed here
def insert_into(test, op):
    test.add_test(func(((2.0,),), op))
    test.add_test(func(((-0.05,),), op))
    test.add_test(func(((1,),), op))
    test.add_test(func(((1,0),(0,1)), op))
    test.add_test(func(((1,2),(3,4)), op))
    test.add_test(func(((-1,0.2),(3000,-4.234)), op))
    test.add_test(func(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), op))

    if not op == inverse:
        # These matrices are not invertible (their determinant is zero)
        test.add_test(func(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0)), op))
        test.add_test(func(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)), op))
        test.add_test(func(((0,),), op))
        test.add_test(func(((0,0),(0,0)), op))
        test.add_test(func(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0)), op))
        
    test.add_test(func(((-1000.0, 1.3333),(0.0, 1.0)), op))
    test.add_test(func(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2)), op))
    test.add_test(func(((0.0, 1.0, 2.0, 3.0, -1.34),(50, 0.0, 1.0, 0.0, 0.5),(2.0, 0.0, 0.0, 1.0, 6.7),(0.5, 1.0, -0.3, -30, -1.2),(-0.3, 0.0, 1.1, 3.4, 90)), op))
    test.add_test(func(((0, 0.0, 1.0, 2.0, 3.0, -0.34),(1, 5, 0.0, 1.0, 0.0, 0.5),(10, 2.0, 0.0, 0.0, 1.0, 6.7),(10, 0.5, 1.0, -0.3, -0.30, -1.2),(-0.3, -0.3, 0.0, 1.1, 3.4, -0.9),(1, 2, 3, 4, 5, 6)), op))

# for trace() and transpose()
def insert_into2(test, op):
    test.add_test(func(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0)), op))
    test.add_test(func(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0)), op))
    test.add_test(func(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0)), op))
    test.add_test(func(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0)), op))
    test.add_test(func(((-1000.0, 1.3333),(0.0, 1.0)), op))
    test.add_test(func(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2)), op))
    test.add_test(func(((1, 2, 3), (4, 5, 6),(-7, -8, -9)), op))
    test.add_test(func(((0.1, 0.1, 0.1), (-4.5, -4.5, -4.5),(70000, 70000, 70000)), op)) 

# Test matrix determinant in immediate mode
test = shtest.ImmediateTest('det', 1)
test.add_call(shtest.Call(shtest.Call.call, 'det', 1))
insert_into(test, det)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test matrix adjoint in immediate mode
test = shtest.ImmediateTest('adjoint', 1)
test.add_call(shtest.Call(shtest.Call.call, 'adjoint', 1))
insert_into(test, adjoint)
test.output(sys.stdout, False)

# Test matrix inverse in immediate mode
test = shtest.ImmediateTest('inverse', 1)
test.add_call(shtest.Call(shtest.Call.call, 'inverse', 1))
insert_into(test, inverse)
test.output(sys.stdout, False)

# Test matrix trace in immediate mode
test = shtest.ImmediateTest('trace', 1)
test.add_call(shtest.Call(shtest.Call.call, 'trace', 1))
insert_into2(test, trace)
test.output(sys.stdout, False)

# Test matrix transpose in immediate mode
test = shtest.ImmediateTest('transpose', 1)
test.add_call(shtest.Call(shtest.Call.call, 'transpose', 1))
insert_into2(test, transpose)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
