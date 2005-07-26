#!/usr/bin/python

# Tests the cond operator

import shtest, sys, common
from common import *

def cond(p, q, r, types=[]):
    result = []
    if is_array(p):
        for (a, b, c) in common.upzip3(p, q, r):
            if (a > 0):
                result.append(b)
            else:
                result.append(c)
    else:
        for (b, c) in common.upzip(q, r):
            if (p > 0):
                result.append(b)
            else:
                result.append(c)
        
    return shtest.make_test(result, [p, q, r], types)

def any(p, types=[]):
    result = p[0]
    for a in p:
        if a > result:
            result = a
    return shtest.make_test(result, [p], types)

def all(p, types=[]):
    result = p[0]
    for a in p:
        if a < result:
            result = a
    return shtest.make_test(result, [p], types)

def and_test(p, q, types=[]):
    result = [min(a, b) for (a, b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def or_test(p, q, types=[]):
    result = [max(a, b) for (a, b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def not_test(p, types=[]):
    result = [1 - (a > 0) for a in p]
    return shtest.make_test(result, [p], types)

# for cond()
def insert_into1(test):
    # cond(1, 1, 1)
    test.add_test(cond((10,), (1.0,), (0.0,)))
    test.add_test(cond((1.0,), (1.0,), (0.0,)))
    test.add_test(cond((0.5,), (1.0,), (0.0,)))
    test.add_test(cond((0,), (1.0,), (0.0,)))
    test.add_test(cond((-0.3,), (1.0,), (0.0,)))
    test.add_test(cond((-3000,), (1.0,), (0.0,)))

    # cond(N, N, N)
    test.add_test(cond((1.4, 0.0, -45.0, 1024), (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))
    test.add_test(cond((0.0, 20.0, -1.0), (28.1, 0.3, 0.045), (0.0, -1.0, -45.8)))
    test.add_test(cond((1.4, 0.0), (-0.4, 2.2), (0.0, -1.1)))

    # cond(1, N, N)
    test.add_test(cond(1.4, (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))
    test.add_test(cond(0.0, (1.0, 2.0, 3.0, 4.0), (0.0, -1.0, -2.0, -45.8)))

# for any(), all() and !
def insert_into2(test, func):
    test.add_test(func((0, 0, 0, 0)))
    test.add_test(func((0, 0, 0, 1)))
    test.add_test(func((1, 0, 0, 0)))
    test.add_test(func((0.1, 15, 0.5, 12)))
    test.add_test(func((0, 1, 0)))
    test.add_test(func((10,)))
    test.add_test(func((0,)))
    test.add_test(func((-1, -0.5, -5000, -2)))
    test.add_test(func((-1, 0, 0)))
    test.add_test(func((-1000, 0, 1000)))

# for && and ||
def insert_into3(test, func):
    test.add_test(func((0, 0, 0), (0, 0, 0)))
    test.add_test(func((0, 0, 0), (1, 0, -1)))
    test.add_test(func((1, 0, 0.4, -1), (1, 0, 0.4, -1)))
    test.add_test(func((0.1, 0.2, 0.3), (-0.1, -0.2, -0.3)))
    test.add_test(func((0.1,), (-0.1,)))
    test.add_test(func((0.1,), (0,)))
    test.add_test(func((0,), (-1000,)))

# Test the conditional operator in stream programs
test = shtest.StreamTest('cond', 3)
test.add_call(shtest.Call(shtest.Call.call, 'cond', 3))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the 'all' function in stream programs
test = shtest.StreamTest('all', 1)
test.add_call(shtest.Call(shtest.Call.call, 'all', 1))
insert_into2(test, all)
test.output(sys.stdout, False)

# Test the 'any' function in stream programs
test = shtest.StreamTest('any', 1)
test.add_call(shtest.Call(shtest.Call.call, 'any', 1))
insert_into2(test, any)
test.output(sys.stdout, False)

# Test the 'not' function in stream programs
test = shtest.StreamTest('not', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '!', 1))
insert_into2(test, not_test)
test.output(sys.stdout, False)

# Test the 'and' function in stream programs
test = shtest.StreamTest('and', 2)
test.add_call(shtest.Call(shtest.Call.infix, '&&', 2))
insert_into3(test, and_test)
test.output(sys.stdout, False)

# Test the 'or' function in stream programs
test = shtest.StreamTest('or', 2)
test.add_call(shtest.Call(shtest.Call.infix, '||', 2))
insert_into3(test, or_test)
test.output(sys.stdout, False)

# Test the conditional operator in immediate mode
test = shtest.ImmediateTest('cond_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'cond', 3))
insert_into1(test)
test.output(sys.stdout, False)

# Test the 'all' function in immediate mode
test = shtest.ImmediateTest('all_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'all', 1))
insert_into2(test, all)
test.output(sys.stdout, False)

# Test the 'any' function in immediate mode
test = shtest.ImmediateTest('any_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'any', 1))
insert_into2(test, any)
test.output(sys.stdout, False)

# Test the 'not' function in immediate mode
test = shtest.ImmediateTest('not_im', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '!', 1))
insert_into2(test, not_test)
test.output(sys.stdout, False)

# Test the 'and' function in immediate mode
test = shtest.ImmediateTest('and_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '&&', 2))
insert_into3(test, and_test)
test.output(sys.stdout, False)

# Test the 'or' function in immediate mode
test = shtest.ImmediateTest('or_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '||', 2))
insert_into3(test, or_test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
