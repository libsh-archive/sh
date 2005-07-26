#!/usr/bin/python

import shtest, sys, common
from common import *

def length_1(a):
    result = 0
    for x in a:
        result += abs(x)
    return [result]

def length_inf(a):
    result = a[0]
    for x in a:
        if abs(x) > result:
            result = abs(x)
    return [result]

def sub(a, b):
    result = []
    for i in range(len(a)):
        result.append(a[i] - b[i])
    return result

def length_test(a, func, types=[]):
    return shtest.make_test(func(a), [a], types)

def distance_test(a, b, func, types=[]):
    return shtest.make_test(func(sub(a, b)), [a, b], types)

# for length()
def insert_into1(test, func):
    test.add_test(length_test((1.0, 2.0, 3.0), func))
    test.add_test(length_test((1.0, 2.0, -3.0), func))
    test.add_test(length_test((1.0, -2.0, -3.0), func))
    test.add_test(length_test((-1.0, -2.0, -3.0), func))
    test.add_test(length_test((-1.0, -2.0, 3.0), func))
    test.add_test(length_test((-1.0, 2.0, 3.0), func))

    test.add_test(length_test((0.2, 2.7, 3.23), func))
    test.add_test(length_test((0.2, 2.7, -3.23), func))
    test.add_test(length_test((0.2, -2.7, -3.23), func))
    test.add_test(length_test((-0.2, -2.7, -3.23), func))
    test.add_test(length_test((-0.2, -2.7, 3.23), func))
    test.add_test(length_test((-0.2, 2.7, 3.23), func))

    test.add_test(length_test((0.0,), func))
    test.add_test(length_test((0.0, 0.0, 0.0), func))
    test.add_test(length_test((0.0, 0.0, 0.0, 0.0), func))
    test.add_test(length_test((0.0, 0.0, 0.0, 0.0), func))

    test.add_test(length_test((1.0,), func))
    test.add_test(length_test((-1.0,), func))
    test.add_test(length_test((100.0,), func))
    test.add_test(length_test((-100.0,), func))
    test.add_test(length_test((0.5,), func))
    test.add_test(length_test((-0.5,), func))

# for distance()
def insert_into2(test, func):
    test.add_test(distance_test((1.0, 2.0, 3.0), (4, 5, 6), func))
    test.add_test(distance_test((1.0, 2.0, -3.0), (-4, 5, 6), func))
    test.add_test(distance_test((1.0, -2.0, -3.0), (4, 5, 6), func))
    test.add_test(distance_test((-1.0, -2.0, -3.0), (-4, -5, -6), func))
    test.add_test(distance_test((-1.0, -2.0, 3.0), (4, 5, 6), func))
    test.add_test(distance_test((-1.0, 2.0, 3.0), (4, 5, 6), func))

    test.add_test(distance_test((0.2, 2.7, 3.23), (0.1, -34.2, 90.111), func))
    test.add_test(distance_test((0.2, 2.7, -3.23), (0.1, 34.2, 90.111), func))
    test.add_test(distance_test((0.2, -2.7, -3.23), (0.1, 34.2, -90.111), func))
    test.add_test(distance_test((-0.2, -2.7, -3.23), (-0.1, -34.2, 90.111), func))
    test.add_test(distance_test((-0.2, -2.7, 3.23), (0.1, -34.2, -90.111), func))
    test.add_test(distance_test((-0.2, 2.7, 3.23), (-0.1, -34.2, -90.111), func))

    test.add_test(distance_test((0.0,), (0,), func))
    test.add_test(distance_test((0.0, 0.0, 0.0), (0, 0, 0), func))
    test.add_test(distance_test((0.0, 0.0, 0.0, 0.0), (0, 0, 0, 0), func))

    test.add_test(distance_test((1.0,), (1.0,), func))
    test.add_test(distance_test((-1.0,), (1.0,), func))
    test.add_test(distance_test((100.0,), (2.0,), func))
    test.add_test(distance_test((-100.0,), (2.4,), func))
    test.add_test(distance_test((0.5,), (0.1,), func))
    test.add_test(distance_test((-0.5,), (0.7333,), func))

# Test length() in stream programs
test = shtest.StreamTest('length', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length', 1))
insert_into1(test, length)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test length_1() in stream programs
test = shtest.StreamTest('length_1', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length_1', 1))
insert_into1(test, length_1)
test.output(sys.stdout, False)

# Test length_inf() in stream programs
test = shtest.StreamTest('length_inf', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length_inf', 1))
insert_into1(test, length_inf)
test.output(sys.stdout, False)

# Test distance() in stream programs
test = shtest.StreamTest('distance', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance', 2))
insert_into2(test, length)
test.output(sys.stdout, False)

# Test distance_1() in stream programs
test = shtest.StreamTest('distance_1', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance_1', 2))
insert_into2(test, length_1)
test.output(sys.stdout, False)

# Test distance_inf() in stream programs
test = shtest.StreamTest('distance_inf', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance_inf', 2))
insert_into2(test, length_inf)
test.output(sys.stdout, False)

# Test length() in immediate mode
test = shtest.ImmediateTest('length_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length', 1))
insert_into1(test, length)
test.output(sys.stdout, False)

# Test length_1() in immediate mode
test = shtest.ImmediateTest('length_1_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length_1', 1))
insert_into1(test, length_1)
test.output(sys.stdout, False)

# Test length_inf() in immediate mode
test = shtest.ImmediateTest('length_inf_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'length_inf', 1))
insert_into1(test, length_inf)
test.output(sys.stdout, False)

# Test distance() in immediate mode
test = shtest.ImmediateTest('distance_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance', 2))
insert_into2(test, length)
test.output(sys.stdout, False)

# Test distance_1() in immediate mode
test = shtest.ImmediateTest('distance_1_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance_1', 2))
insert_into2(test, length_1)
test.output(sys.stdout, False)

# Test distance_inf() in immediate mode
test = shtest.ImmediateTest('distance_inf_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'distance_inf', 2))
insert_into2(test, length_inf)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
