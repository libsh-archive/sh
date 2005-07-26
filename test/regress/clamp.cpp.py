#!/usr/bin/python

import shtest, sys

# for clamp()
def insert_into1(test):
    test.add_test(shtest.make_test([0.0, 0.5, 1.0, 1.0], [[0.0, 0.5, 1.0, 1.5], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))
    test.add_test(shtest.make_test([0.0, 0.0, 0.0, 0.0], [[-5.2, -1.0, -0.5, -0.0], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))
    test.add_test(shtest.make_test([1.0, 1.0, 1.0, 1.0], [[1.0, 1.1, 5.0, 100], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))
    test.add_test(shtest.make_test([1.0, 1.0, 1.0, 1.2], [[0.0, 0.5, 1.0, 1.5], [1.0, 1.0, 1.0, 1.0], [1.2, 1.2, 1.2, 1.2]], []))
    test.add_test(shtest.make_test([-0.5, -0.5, -0.5, -0.4], [[-5.2, -1.0, -0.5, -0.0], [-0.5, -0.5, -0.5, -0.5], [-0.4, -0.4, -0.4, -0.4]], []))
    test.add_test(shtest.make_test([5.0, 5.0, 5.0, 10.0], [[1.0, 1.1, 5.0, 100], [5.0, 5.0, 5.0, 5.0], [10.0, 10.0, 10.0, 10.0]], []))
    # clamp(N, 1, 1)
    test.add_test(shtest.make_test([5.0, 5.0, 10.0, 10.0], [[1.0, 5.0, 10, 100], 5.0, 10.0], []))

# for pos()
def insert_into2(test):
    test.add_test(shtest.make_test([3.0, 3.1, 3.5, 3.9], [[3.0, 3.1, 3.5, 3.9]], []))
    test.add_test(shtest.make_test([0, 0, 0.5, 1.0], [[-0.5, -0.0, 0.5, 1.0]], []))
    test.add_test(shtest.make_test([0, 0, 0, 0], [[-3.0, -3.1, -3.5, -3.9]], []))

# for sat()
def insert_into3(test):
    test.add_test(shtest.make_test([1, 1, 1, 1], [[3.0, 3.1, 3.5, 3.9]], []))
    test.add_test(shtest.make_test([-0.5, -0.0, 0.5, 1.0], [[-0.5, -0.0, 0.5, 1.0]], []))
    test.add_test(shtest.make_test([-3.0, -3.1, -3.5, -3.9], [[-3.0, -3.1, -3.5, -3.9]], []))
    

# Test clamp in stream programs
test = shtest.StreamTest('clamp', 3)
test.add_call(shtest.Call(shtest.Call.call, 'clamp', 3))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test pos in stream programs
test = shtest.StreamTest('pos', 1)
test.add_call(shtest.Call(shtest.Call.call, 'pos', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test sat in stream programs
test = shtest.StreamTest('sat', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sat', 1))
insert_into3(test)
test.output(sys.stdout, False)

# Test clamp in immediate mode
test = shtest.ImmediateTest('clamp_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'clamp', 3))
insert_into1(test)
test.output(sys.stdout, False)

# Test pos in immediate mode
test = shtest.ImmediateTest('pos_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'pos', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test sat in immediate_mode
test = shtest.ImmediateTest('sat_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sat', 1))
insert_into3(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
