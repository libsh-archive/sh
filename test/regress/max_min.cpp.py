#!/usr/bin/python

import shtest, sys, common

def func_test(p, q, func, types=[]):
    result = [func(a, b) for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def comp_test(p, func, types=[]):
    result = p[0]
    for a in p:
        if func(a, result) == a:
            result = a
    return shtest.make_test(result, [p], types)

def insert_into(test, func):
    test.add_test(func_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0), func))
    test.add_test(func_test((0.2, 0.5, 0.9, 1.5), (0, 0, 0, 0), func))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0), func))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 2.9, 2.3), func))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (-0.5, -1.0, -2.0, -4.0), func))
    test.add_test(func_test((-2.0, -3.0, -2.0, -3.0), (-0.5, -1.0, -2.0, -4.0), func))

    test.add_test(func_test((0.0, 0.0, 0.0), (1.0, 2.0, 5.0), func, ['i', 'i', 'i']))
    test.add_test(func_test((0.2, 0.5, 0.9, 1.5), (0, 0, 0, 0), func, ['i', 'i', 'i']))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (0.0, 1.0, 2.0, 5.0), func, ['i', 'i', 'i']))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (0.1, 0.5, 2.9, 2.3), func, ['i', 'i', 'i']))
    test.add_test(func_test((2.0, 3.0, 2.0, 3.0), (-0.5, -1.0, -2.0, -4.0), func, ['i', 'i', 'i']))
    test.add_test(func_test((-2.0, -3.0, -2.0, -3.0), (-0.5, -1.0, -2.0, -4.0), func, ['i', 'i', 'i']))

def insert_into_comp(test, func):
    test.add_test(comp_test((0, 0.1, 0.5, 1.9), func))
    test.add_test(comp_test((0.1, 0, 1.9, 1.5), func))
    test.add_test(comp_test((1.0, 2.0, 3.0, 4.0), func))
    test.add_test(comp_test((2.0, 1.0, 4.0, 3.0), func))
    test.add_test(comp_test((-1.0, -2.1, -3.5, -4.4), func))

    test.add_test(comp_test((0, 0.1, 0.5, 1.9), func, ['i', 'i', 'i']))
    test.add_test(comp_test((0.1, 0, 1.9, 1.5), func, ['i', 'i', 'i']))
    test.add_test(comp_test((1.0, 2.0, 3.0, 4.0), func, ['i', 'i', 'i']))
    test.add_test(comp_test((2.0, 1.0, 4.0, 3.0), func, ['i', 'i', 'i']))
    test.add_test(comp_test((-1.0, -2.1, -3.5, -4.4), func, ['i', 'i', 'i']))

#-------------
# MAX
#-------------

# Test N <- max(N, N) in stream programs
test = shtest.StreamTest('max', 2)
test.add_call(shtest.Call(shtest.Call.call, 'max', 2))
insert_into(test, max)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the componentwise max function in stream programs
test = shtest.StreamTest('max1', 1)
test.add_call(shtest.Call(shtest.Call.call, 'max', 1))
insert_into_comp(test, max)
test.output(sys.stdout, False)

# Test N <- max(N, N) in immediate mode
test = shtest.ImmediateTest('max_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'max', 2))
insert_into(test, max)
test.output(sys.stdout, False)

# Test the componentwise max function in immediate mode
test = shtest.ImmediateTest('max1_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'max', 1))
insert_into_comp(test, max)
test.output(sys.stdout, False)

#-------------
# MIN
#-------------

# Test N <- min(N, N) in stream programs
test = shtest.StreamTest('min', 2)
test.add_call(shtest.Call(shtest.Call.call, 'min', 2))
insert_into(test, min)
test.output(sys.stdout, False)

# Test the componentwise min function in stream programs
test = shtest.StreamTest('min1', 1)
test.add_call(shtest.Call(shtest.Call.call, 'min', 1))
insert_into_comp(test, min)
test.output(sys.stdout, False)

# Test N <- min(N, N) in immediate mode
test = shtest.ImmediateTest('min_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'min', 2))
insert_into(test, min)
test.output(sys.stdout, False)

# Test the componentwise min function in immediate mode
test = shtest.ImmediateTest('min1_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'min', 1))
insert_into_comp(test, min)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
