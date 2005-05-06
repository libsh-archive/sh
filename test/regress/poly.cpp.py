#!/usr/bin/python

import shtest, sys, math

def poly(a, b, types=[]):
    result = [None] * len(a)
    for i in range(0, len(a)):
        s = 0
        for j in range(0, len(b)):
            s += pow(a[i], j) * b[j] 
        result[i] = s
    return shtest.make_test(result, [a, b], types)

def insert_into(test, func):
    test.add_test(func((0.0,), (0,)))
    test.add_test(func((0.0, 0.0), (0, 0)))
    test.add_test(func((0.0, 0.0, 0.0), (0, 0, 0)))
    test.add_test(func((0.0, 0.0, 0.0, 0.0), (0, 0, 0, 0)))

    test.add_test(func((1.0,), (0,)))
    test.add_test(func((1.0, 2.0), (0, 0)))
    test.add_test(func((1.0, 2.0, 3.0), (0, 0, 0)))
    test.add_test(func((1.0, 2.0, 3.0, 4.0), (0, 0, 0, 0)))

    test.add_test(func((1.0,), (1,)))
    test.add_test(func((1.0, 2.0), (2,)))
    test.add_test(func((1.0, 2.0, 3.0), (3, 0, 0, -0.5)))
    test.add_test(func((1.0, 2.0, 3.0, 4.0), (4, -10, 1)))

    test.add_test(func((1.0,), (1, 4, 5, 35.2)))
    test.add_test(func((-10.0, 0.2), (0.3,)))
    test.add_test(func((0.5, -0.2, 13.0, 42), (1, -1, 0)))

    # Test 2-component output
    test.add_test(func((1,), (-1, 0)))
    test.add_test(func((0.4,), (-1, 4)))
    test.add_test(func((1.0, -0.2), (1, 4, 5, 35.2)))
    test.add_test(func((-10.0, 0.2), (0.3,))) # works
    test.add_test(func((-10.0, 0.2), (0.3, 20)))
    test.add_test(func((0.5, -0.2, 13.0, 42), (1, -1)))
    test.add_test(func((1.0, 2.0, 3.0, 4.0), (4, -10)))
    test.add_test(func((1.0, 2.0, 3.0), (4, -10)))
    test.add_test(func((1.0, 2.0), (4, -10)))

# Test poly in stream programs
test = shtest.StreamTest('poly', 2)
test.add_call(shtest.Call(shtest.Call.call, 'poly', 2))
insert_into(test, poly)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test poly in immediate mode
test = shtest.ImmediateTest('poly_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'poly', 2))
insert_into(test, poly)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
