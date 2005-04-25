#!/usr/bin/python

import shtest, sys, common

def min_test(p, q, types=[]):
    result = [min(a, b) for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def insert_into1(test):
    test.add_test(min_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0)))
    test.add_test(min_test((0.2, 0.5, 0.9, 1.5), (0, 0, 0, 0)))
    test.add_test(min_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0)))
    test.add_test(min_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 2.9, 2.3)))
    test.add_test(min_test((2.0, 3.0, 2.0, 3.0), (-0.5, -1.0, -2.0, -4.0)))
    test.add_test(min_test((-2.0, -3.0, -2.0, -3.0), (-0.5, -1.0, -2.0, -4.0)))

def insert_into2(test):
    test.add_test(shtest.make_test([0.0], [[0, 0.1, 0.5, 1.9]], []))
    test.add_test(shtest.make_test([1.0], [[1.0, 2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([-4.4], [[-1.0, -2.1, -3.5, -4.4]], []))

# Test N <- min(N, N) in stream programs
test = shtest.StreamTest('min', 2)
test.add_call(shtest.Call(shtest.Call.call, 'min', 2))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the componentwise min function in stream programs
test = shtest.StreamTest('min1', 1)
test.add_call(shtest.Call(shtest.Call.call, 'min', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test N <- min(N, N) in immediate mode
test = shtest.ImmediateTest('min_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'min', 2))
insert_into1(test)
test.output(sys.stdout, False)

# Test the componentwise min function in immediate mode
test = shtest.ImmediateTest('min1_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'min', 1))
insert_into2(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
