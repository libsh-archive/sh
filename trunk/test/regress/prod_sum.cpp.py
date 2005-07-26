#!/usr/bin/python

import shtest, sys, math

def cmul(l, types=[]):
    return shtest.make_test((reduce(lambda x, y: x * y, l, 1),), [l], types)

def csum(l, types=[]):
    return shtest.make_test((sum(l),), [l], types)

def insert_into(test, func):
    test.add_test(func((1.5, 1.0, 2.718, 3.14)))
    test.add_test(func((1.5,)))
    test.add_test(func((1.5, 1.0, 2.718, 3.14), ['d', 'd']))
    test.add_test(func((1.5,), ['d', 'd']))
    test.add_test(func((1, 2, 3, 4), ['i', 'i']))
    test.add_test(func((3,), ['i', 'i']))

    test.add_test(func((1.0, 2.0, 3.0)))
    test.add_test(func((1.0, 2.0, -3.0)))
    test.add_test(func((1.0, -2.0, -3.0)))
    test.add_test(func((-1.0, -2.0, -3.0)))
    test.add_test(func((-1.0, -2.0, 3.0)))
    test.add_test(func((-1.0, 2.0, 3.0)))

    test.add_test(func((0.2, 2.7, 3.23)))
    test.add_test(func((0.2, 2.7, -3.23)))
    test.add_test(func((0.2, -2.7, -3.23)))
    test.add_test(func((-0.2, -2.7, -3.23)))
    test.add_test(func((-0.2, -2.7, 3.23)))
    test.add_test(func((-0.2, 2.7, 3.23)))

    test.add_test(func((0.0,)))
    test.add_test(func((0.0, 0.0, 0.0)))
    test.add_test(func((0.0, 0.0, 0.0, 0.0)))
    test.add_test(func((0.0, 0.0, 0.0, 0.0)))

    test.add_test(func((1.0,)))
    test.add_test(func((-1.0,)))
    test.add_test(func((1000.0,)))
    test.add_test(func((-1000.0,)))
    test.add_test(func((0.5,)))
    test.add_test(func((-0.5,)))

# Test the product of components in stream programs
test = shtest.StreamTest('prod', 1)
test.add_call(shtest.Call(shtest.Call.call, 'prod', 1))
insert_into(test, cmul)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the sum of components in stream programs
test = shtest.StreamTest('sum', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sum', 1))
insert_into(test, csum)
test.output(sys.stdout, False)

# Test the product of components in immediate mode
test = shtest.ImmediateTest('prod_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'prod', 1))
insert_into(test, cmul)
test.output(sys.stdout, False)

# Test the sum of components in immediate mode
test = shtest.ImmediateTest('sum_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sum', 1))
insert_into(test, csum)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
