#!/usr/bin/python

import shtest, sys, common

# for sort()
def insert_into1(test):
    test.add_test(shtest.make_test([0.0, 0.1, 0.5, 1.9], [[1.9, 0.1, 0.5, 0.0]], []))
    test.add_test(shtest.make_test([-4.4, -3.5, -2.1, -1.5], [[-1.5, -2.1, -3.5, -4.4]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0, 4.0], [[2.0, 1.0, 4.0, 3.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0, 4.0], [[1.0, 2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([-2.1, -1.5, 0.0, 6.7], [[6.7, 0, -2.1, -1.5]], []))

# for cast()
def insert_into2(test):
    test.add_test(shtest.make_test([1.0, 2.0, 3.0], [[1.0, 2.0, 3.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0], [[1.0, 2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 0.0], [[1.0, 2.0]], []))
    test.add_test(shtest.make_test([1.0, 0.0, 0.0], [[1.0,]], []))

# for fillcast()
def insert_into3(test):
    test.add_test(shtest.make_test([1.0, 2.0, 3.0], [[1.0, 2.0, 3.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0], [[1.0, 2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 2.0], [[1.0, 2.0]], []))
    test.add_test(shtest.make_test([1.0, 1.0, 1.0], [[1.0,]], []))

# for join()
def insert_into5(test):
    test.add_test(shtest.make_test([1.0, 2.0], [[1.0,], [2.0,]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0], [[1.0], [2.0, 3.0]], []))
    test.add_test(shtest.make_test([1.0, 2.0, 3.0, 4.0], [[1.0,], [2.0, 3.0, 4.0]], []))
    test.add_test(shtest.make_test([-3.0, -2.0, -1.0], [[-3.0, -2.0], [-1.0]], []))
    test.add_test(shtest.make_test([4.0, -3.0, 2.0, -1.0], [[4.0, -3.0], [2.0, -1.0]], []))
    test.add_test(shtest.make_test([4.0, 3.0, 2.0, 1.0], [[4.0, 3.0, 2.0], [1.0,]], []))

# Test the sort function in stream programs
test = shtest.StreamTest('sort', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sort', 1))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the cast function in stream programs
test = shtest.StreamTest('cast', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cast<3>', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test the fillcast function in stream programs
test = shtest.StreamTest('fillcast', 1)
test.add_call(shtest.Call(shtest.Call.call, 'fillcast<3>', 1))
insert_into3(test)
test.output(sys.stdout, False)

# Test the join function in stream programs
test = shtest.StreamTest('join', 2)
test.add_call(shtest.Call(shtest.Call.call, 'join', 2))
insert_into5(test)
test.output(sys.stdout, False)

# Test the sort function in immediate mode
test = shtest.ImmediateTest('sort_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sort', 1))
insert_into1(test)
test.output(sys.stdout, False)

# Test the cast function in immediate mode
test = shtest.ImmediateTest('cast_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cast<3>', 1))
insert_into2(test)
test.output(sys.stdout, False)

# Test the fillcast function in immediate mode
test = shtest.ImmediateTest('fillcast_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'fillcast<3>', 1))
insert_into3(test)
test.output(sys.stdout, False)

# Test the join function in immediate mode
test = shtest.ImmediateTest('join_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'join', 2))
insert_into5(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
