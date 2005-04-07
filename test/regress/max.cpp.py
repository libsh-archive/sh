#!/usr/bin/python

import shtest, sys, common

def max_test(p, q, types=[]):
    result = [max(a, b) for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def insert_into1(test):
    test.add_test(max_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0)))
    test.add_test(max_test((0.2, 0.5, 0.9, 1.5), (0, 0, 0, 0)))
    test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0)))
    test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 2.9, 2.3)))
    test.add_test(max_test((2.0, 3.0, 2.0, 3.0), (-0.5, -1.0, -2.0, -4.0)))
    test.add_test(max_test((-2.0, -3.0, -2.0, -3.0), (-0.5, -1.0, -2.0, -4.0)))

def insert_into2(test):
    test.add_test(shtest.make_test([1.9], [[0, 0.1, 0.5, 1.9]], []))
    test.add_test(shtest.make_test([4.0], [[1.0, 2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([-1.0], [[-1.0, -2.1, -3.5, -4.4]], []))

# Test N <- max(N, N) in stream programs
test = shtest.StreamTest('max', 2)
test.add_call(shtest.Call(shtest.Call.call, 'max', 2))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the componentwise max function in stream programs
test = shtest.StreamTest('max1', 1)
test.add_call(shtest.Call(shtest.Call.call, 'max', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test N <- max(N, N) in immediate mode
test = shtest.ImmediateTest('max_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'max', 2))
insert_into1(test)
test.output(sys.stdout, False)

# Test the componentwise max function in immediate mode
test = shtest.ImmediateTest('max1_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'max', 1))
insert_into2(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
