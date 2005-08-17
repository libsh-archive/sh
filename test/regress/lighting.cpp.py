#!/usr/bin/python

# Test the lighting functions (see Table 8.14 in book)

import shtest, sys, common
from common import * 

def normalize(p, types=[]):
    N = len(p)
    result = [None] * N
    euclidean_length = length(p)[0]
    for i in range(N):
        if euclidean_length == 0:
            result[i] = 0
        else:
            result[i] = p[i] / euclidean_length
    return shtest.make_test(result, [p], types)

def lit(a, b, c, types=[]):
    x = max(a[0], 0)
    y = max(b[0], 0)
    w = min(max(c[0], -128.0), 128.0)
    third = 0
    if x > 0:
        third = pow(y, w)
    result = [1, x, third, 1]
    return shtest.make_test(result, [a, b, c], types)

def faceforward(p, q, types=[]):
    N = len(p)
    result = [None] * N
    if dot_product(p, q) > 0:
        for i in range(N):
            result[i] = q[i]
    else:
        for i in range(N):
            result[i] = -q[i]
    return shtest.make_test(result, [p, q], types)

# for normalize()
def insert_into1(test):
    test.add_test(normalize((1.0, 0.0, 0.0)))
    test.add_test(normalize((1.0, 2.0, 3.0)))
    test.add_test(normalize((1.0, 1.0, 1.0)))
    test.add_test(normalize((1.0, 0.0, 1.0)))
    test.add_test(normalize((0.5, -1.0, 1.0)))
    test.add_test(normalize((0.005, -1.0, 1.0, 5)))
    test.add_test(normalize((-0.023,)))

# for lit()
def insert_into2(test):
    test.add_test(lit((0.75,),(0.5,),(10.0,)))
    test.add_test(lit((0.5,),(0.5,),(50.0,)))
    test.add_test(lit((0.5,),(0.75,),(100.0,)))
    test.add_test(lit((0.5,),(0.75,),(100.0,)))
    test.add_test(lit((0.01,),(0.01,),(30.0,)))
    test.add_test(lit((0.99,),(0.99,),(5.0,)))
    
    test.add_test(lit((1.0,),(-3.0,),(0.5,)))
    test.add_test(lit((1.0,),(-3.0,),(2.5,)))
    test.add_test(lit((0.5,),(-1.5,),(5.5,)))
    
    test.add_test(lit((1.0,),(1.0,),(1.0,)))
    test.add_test(lit((-1.0,),(-1.0,),(1.0,)))
    test.add_test(lit((2.0,),(1.0,),(0.0,)))
    test.add_test(lit((2.0,),(0.5,),(0.0,)))
    test.add_test(lit((0.0,),(0.0,),(1.0,)))
    test.add_test(lit((0.0,),(0.0,),(2.0,)))
    test.add_test(lit((0.0,),(1.0,),(1.0,)))
    test.add_test(lit((0.0,),(1.0,),(2.0,)))

    test.add_test(lit((1.0,),(1.0,),(1.0,)))
    test.add_test(lit((2.0,),(1.0,),(1.0,)))
    test.add_test(lit((1.0,),(0.0,),(1.0,)))
    test.add_test(lit((2.0,),(0.0,),(1.0,)))
    test.add_test(lit((2.0,),(0.5,),(1.0,)))
    test.add_test(lit((0.0,),(0.0,),(1.0,)))
    test.add_test(lit((0.0,),(1.0,),(1.0,)))

    test.add_test(lit((0.0,),(1.0,),(-128,)))
    test.add_test(lit((0.0,),(1.0,),(-130,)))
    test.add_test(lit((0.0,),(1.0,),(-5000,)))
    test.add_test(lit((0.0,),(1.0,),(128,)))
    test.add_test(lit((0.0,),(1.0,),(130,)))
    test.add_test(lit((0.0,),(1.0,),(5000,)))

    test.add_test(lit((0.0,),(0.5,),(-128,)))
    test.add_test(lit((0.0,),(0.5,),(-130,)))
    test.add_test(lit((0.0,),(0.5,),(-5000,)))
    test.add_test(lit((0.0,),(0.5,),(128,)))
    test.add_test(lit((0.0,),(0.5,),(130,)))
    test.add_test(lit((0.0,),(0.5,),(5000,)))

    test.add_test(lit((0.0,),(0.01,),(-128,)))
    test.add_test(lit((0.0,),(0.01,),(-130,)))
    test.add_test(lit((0.0,),(0.01,),(-5000,)))
    test.add_test(lit((0.0,),(0.01,),(128,)))
    test.add_test(lit((0.0,),(0.01,),(130,)))
    test.add_test(lit((0.0,),(0.01,),(5000,)))

# for faceforward()
def insert_into3(test):
    #test.add_test(faceforward((1.0, 2.0), (2.0, 1.0)))
    test.add_test(faceforward((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)))
    test.add_test(faceforward((1.0, 2.0, 3.0), (0, 0, 0)))
    test.add_test(faceforward((0.0, 0.0, 0.0), (1.0, 2.0, 3.0)))
    test.add_test(faceforward((0.0, 0.0, 0.0), (0.0, 0.0, 0.0)))
    test.add_test(faceforward((1.0, 1.0, 1.0), (0.5, 2.0, 4.0)))
    test.add_test(faceforward((1.0, 0.0, 1.0), (0.0, 1.0, 0.0)))
    test.add_test(faceforward((0.5, -1.0, 1.0), (0.5, -1.5, -6.0)))
    test.add_test(faceforward((-0.5, -1.0, 1.0), (0.5, 1.5, -6.0)))

# Test lit in stream programs
test = shtest.StreamTest('lit', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lit', 3))
insert_into2(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test normalize in stream programs
test = shtest.StreamTest('normalize', 1)
test.add_call(shtest.Call(shtest.Call.call, 'normalize', 1))
insert_into1(test)
test.output(sys.stdout, False)

# Test faceforward in stream programs
test = shtest.StreamTest('faceforward', 2)
test.add_call(shtest.Call(shtest.Call.call, 'faceforward', 2))
insert_into3(test)
test.output(sys.stdout, False)

# Test lit in immediate mode
test = shtest.ImmediateTest('lit_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lit', 3))
insert_into2(test)
test.output(sys.stdout, False)

# Test normalize in immediate mode
test = shtest.ImmediateTest('normalize_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'normalize', 1))
insert_into1(test)
test.output(sys.stdout, False)

# Test faceforward in immediate programs
test = shtest.ImmediateTest('faceforward_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'faceforward', 2))
insert_into3(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
