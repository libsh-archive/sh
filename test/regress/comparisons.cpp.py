#!/usr/bin/python

import shtest, sys, common
from common import *

def lt(a, b):
    return (a < b) + 0

def le(a, b):
    return (a <= b) + 0

def gt(a, b):
    return (a > b) + 0

def ge(a, b):
    return (a >= b) + 0

def eq(a, b):
    return (a == b) + 0

def ne(a, b):
    return (a != b) + 0

def func(p, q, op, types=[]):
    if is_array(p) and is_array(q):
        result = [op(a, b) for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [op(p, b) for b in q]
    else:
        result = [op(a, q) for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, op):
    test.add_test(func((1.0,), (2.0,), op))
    test.add_test(func((1.0,), (1.0,), op))
    test.add_test(func((3.0,), (1.0,), op))
    
    test.add_test(func((1.0, 2.0, 3.0), (2.0, 1.0, 3.0), op))
    test.add_test(func((-1.0, -2.0, -3.0), (-2.0, -1.0, -3.0), op))

    test.add_test(func((1.0, -2.0, 3.0), (-2.0, 1.0, -3.0), op))
    test.add_test(func((0.0, 1.0, -0.5, 0.1), (0.0, 0.0, 1.0, -1.0), op))

    test.add_test(func(0.0, (-1.0, 0.0, 1.0), op))
    test.add_test(func(2.0, (-1.0, 0.0, 1.0), op))

    test.add_test(func((-1.0, 0.0, 1.0), -2.0, op))
    test.add_test(func((-1.0, 0.0, 1.0), 0.0, op))

    test.add_test(func((1.0,), (2.0,), op, ['i']))
    test.add_test(func((1.0,), (1.0,), op, ['i']))
    test.add_test(func((3.0,), (1.0,), op, ['i']))
    
    test.add_test(func((1.0, 2.0, 3.0), (2.0, 1.0, 3.0), op, ['i', 'i', 'i']))
    test.add_test(func((-1.0, -2.0, -3.0), (-2.0, -1.0, -3.0), op, ['i', 'i', 'i']))

    test.add_test(func((1.0, -2.0, 3.0), (-2.0, 1.0, -3.0), op, ['i', 'i', 'i']))
    test.add_test(func((0.0, 1.0, -0.5, 0.1), (0.0, 0.0, 1.0, -1.0), op, ['i', 'i', 'i', 'i']))

    test.add_test(func(0.0, (-1.0, 0.0, 1.0), op, ['i', 'i', 'i']))
    test.add_test(func(2.0, (-1.0, 0.0, 1.0), op, ['i', 'i', 'i']))

    test.add_test(func((-1.0, 0.0, 1.0), -2.0, op, ['i', 'i', 'i']))
    test.add_test(func((-1.0, 0.0, 1.0), 0.0, op, ['i', 'i', 'i']))

# Test the "less than" operator in stream programs
test = shtest.StreamTest('lt', 2)
test.add_call(shtest.Call(shtest.Call.infix, '<', 2))
insert_into(test, lt)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the "less than or equal to" operator in stream programs
test = shtest.StreamTest('le', 2)
test.add_call(shtest.Call(shtest.Call.infix, '<=', 2))
insert_into(test, le)
test.output(sys.stdout, False)

# Test the "greater than" operator in stream programs
test = shtest.StreamTest('gt', 2)
test.add_call(shtest.Call(shtest.Call.infix, '>', 2))
insert_into(test, gt)
test.output(sys.stdout, False)

# Test the "greater than or equal to" operator in stream programs
test = shtest.StreamTest('ge', 2)
test.add_call(shtest.Call(shtest.Call.infix, '>=', 2))
insert_into(test, ge)
test.output(sys.stdout, False)

# Test the "equal" operator in stream programs
test = shtest.StreamTest('eq', 2)
test.add_call(shtest.Call(shtest.Call.infix, '==', 2))
insert_into(test, eq)
test.output(sys.stdout, False)

# Test the "not equal" operator in stream programs
test = shtest.StreamTest('ne', 2)
test.add_call(shtest.Call(shtest.Call.infix, '!=', 2))
insert_into(test, ne)
test.output(sys.stdout, False)

# Test the "less than" operator in immediate mode
test = shtest.ImmediateTest('lt_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '<', 2))
insert_into(test, lt)
test.output(sys.stdout, False)

# Test the "less than or equal to" operator in immediate mode
test = shtest.ImmediateTest('le_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '<=', 2))
insert_into(test, le)
test.output(sys.stdout, False)

# Test the "greater than" operator in immediate mode
test = shtest.ImmediateTest('gt_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '>', 2))
insert_into(test, gt)
test.output(sys.stdout, False)

# Test the "greater than or equal to" operator in immediate mode
test = shtest.ImmediateTest('ge_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '>=', 2))
insert_into(test, ge)
test.output(sys.stdout, False)

# Test the "equal" operator in immediate mode
test = shtest.ImmediateTest('eq_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '==', 2))
insert_into(test, eq)
test.output(sys.stdout, False)

# Test the "not equal" operator in immediate mode
test = shtest.ImmediateTest('ne_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '!=', 2))
insert_into(test, ne)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
