#!/usr/bin/python

import shtest, sys, math
from math import sqrt

def rsqrt(a):
    return 1.0 / sqrt(a)

def func(p, op, types=[]):
    result = [op(a) for a in p]
    return shtest.make_test(result, [p], types)

def insert_into(test, op):
    if (op != rsqrt):
        test.add_test(func((0.0,), op))
    test.add_test(func((1.0, 2.0, 4.0), op))
    test.add_test(func((0.1, 0.25, 0.3, 0.5), op))
    test.add_test(func((2.3, 3.8, 2.0, 3.0), op))
    test.add_test(func((0.5, 1.0, 2.9, 50.0), op))

# Test the square root function in stream programs
test = shtest.StreamTest('sqrt', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sqrt', 1))
insert_into(test, sqrt)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the inverse square root function in stream programs
test = shtest.StreamTest('rsqrt', 1)
test.add_call(shtest.Call(shtest.Call.call, 'rsqrt', 1))
insert_into(test, rsqrt)
test.output(sys.stdout, False)

# Test the square root function in immediate mode
test = shtest.ImmediateTest('sqrt_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sqrt', 1))
insert_into(test, sqrt)
test.output(sys.stdout, False)

# Test the inverse square root function in immediate mode
test = shtest.ImmediateTest('rsqrt_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'rsqrt', 1))
insert_into(test, rsqrt)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
