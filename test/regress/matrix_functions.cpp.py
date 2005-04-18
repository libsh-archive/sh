#!/usr/bin/python

import shtest, sys

def det(p, types=[]):
    nrows = len(p)
    ncols = len(p[0])
    if (nrows == ncols):
        if (nrows == 1):
            result = p[0][0]
        elif (nrows == 2):
            result = p[0][0]*p[1][1] - p[0][1]*p[1][0]
        elif (nrows == 3):
            result = p[0][0]*(p[1][1]*p[2][2] - p[1][2]*p[2][1]) - p[0][1]*(p[1][0]*p[2][2] - p[1][2]*p[2][0]) + p[0][2]*(p[1][0]*p[2][1] - p[1][1]*p[2][0])
        else:
            # TODO: generalize for N > 3
            result = 0
        return shtest.make_test(result, [p], types)

def trace(p, types=[]):
    nrows = len(p)
    result = 0
    for i in range(nrows):
        result += p[i][i]
    return shtest.make_test(result, [p], types)

def transpose(p, types=[]):
    nrows = len(p)
    ncols = len(p[0])
    result = [ [None] * nrows for i in range(ncols) ]
    for i in range(ncols):
        for j in range(nrows):
            result[i][j] = p[j][i]
    return shtest.make_test(result, [p], types)

# for det()
def insert_into(test):
    test.add_test(det(((2.0,),)))
    test.add_test(det(((-0.05,),)))
    test.add_test(det(((1,),)))
    test.add_test(det(((0,),)))
    test.add_test(det(((1,0),(0,1))))
    test.add_test(det(((0,0),(0,0))))
    test.add_test(det(((1,2),(3,4))))
    test.add_test(det(((-1,0.2),(3000,-4.234))))
    test.add_test(det(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0))))
    test.add_test(det(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(det(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0))))
    test.add_test(det(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(det(((-1000.0, 1.3333),(0.0, 1.0))))
    #test.add_test(det(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2))))

# for trace() and transpose()
def insert_into2(test, func):
    test.add_test(func(((1.0, 0.0, 0.0),(0.0, 1.0, 0.0),(0.0, 0.0, 1.0))))
    test.add_test(func(((0.0, 0.0, 0.0),(0.0, 0.0, 0.0),(0.0, 0.0, 0.0))))
    test.add_test(func(((-1.0, -2.0, -3.0),(-3.0, -4.0, -5.0),(-5.0, -6.0, -7.0))))
    test.add_test(func(((1.0, 2.0, 3.0),(3.0, 4.0, 5.0),(5.0, 6.0, 7.0))))
    test.add_test(func(((-1000.0, 1.3333),(0.0, 1.0))))
    test.add_test(func(((0.0, 1.0, 2.0, 3.0),(0.0, 1.0, 0.0, 0.5),(0.0, 0.0, 1.0, 6.7),(1.0, -0.3, -3000, -1.2))))

    test.add_test(func(((1, 2, 3), (4, 5, 6),(-7, -8, -9))))
    test.add_test(func(((0.1, 0.1, 0.1), (-4.5, -4.5, -4.5),(70000, 70000, 70000)))) 

# Test matrix determinant in immediate mode
test = shtest.ImmediateTest('det', 1)
test.add_call(shtest.Call(shtest.Call.call, 'det', 1))
insert_into(test)
test.output_header(sys.stdout)
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