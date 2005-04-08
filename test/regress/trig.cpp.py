#!/usr/bin/python

import shtest, sys, math, common

def func(l, op, types=[]):
    return shtest.make_test([op(x) for x in l], [l], types)

def atan2_test(p, q, types=[]):
    result = [math.atan2(a, b) for (a, b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

# for cos() and sin()
def insert_into1(test, op):
    test.add_test(func((0.0, 1.0, 2.0, 3.0), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((math.pi, -math.pi, 0.0), op))
    test.add_test(func((math.pi*2.0, math.pi/2.0, 0.0), op))
    test.add_test(func((-math.pi*2.0, -math.pi/2.0, 0.0), op))
    test.add_test(func((3.0,), op))
    test.add_test(func((-0.5, -1.0, -3.0, -4.0), op))
    test.add_test(func((0.5, 1.5, 2.5), op))

# for acos() and asin()
def insert_into2(test, op):
    test.add_test(func((0.0,), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((1 - 0.1, -1 + 0.1, 0.0), op))
    test.add_test(func((-0.5, -0.9, -0.1), op))
    test.add_test(func((0.5, 0.6, 0.9), op))

# for tan()
def insert_into3(test, op):
    test.add_test(func((0.0, 1.0, 2.0, 3.0), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((math.pi/2 - 0.1, -math.pi/2 + 0.1, 0.0), op))
    test.add_test(func((-0.5, -1.0, -1.1), op))
    test.add_test(func((0.5, 1.5, 2.5), op))

# for atan()
def insert_into4(test, op):
    test.add_test(func((0.0, 1.0, 2.0, 3.0), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((math.pi, -math.pi, 0.0), op))
    test.add_test(func((math.pi*2.0, math.pi/2.0, 0.0), op))
    test.add_test(func((-math.pi*2.0, -math.pi/2.0, 0.0), op))
    test.add_test(func((3.0,), op))
    test.add_test(func((-0.5, -1.0, -3.0, -4.0), op))
    test.add_test(func((0.5, 1.5, 2.5), op))

# for atan2()
def insert_into5(test):
    test.add_test(atan2_test((1.0, 1.0, 1.0, 1.0), (0.0, 1.0, 2.0, 3.0)))
    test.add_test(atan2_test((0.5, 1.5, 3.0, 50.0), (0.0, 1.0, 2.0, 3.0)))
    test.add_test(atan2_test((1.0, 1.0, 1.0, 1.0), (0.3, 0.5, 0.8, 0.9)))
    test.add_test(atan2_test((1.0, 1.0, 1.0), (math.pi, -math.pi, 0.0)))
    test.add_test(atan2_test((math.pi, -math.pi, 0.0), (1.0, 1.0, 1.0)))
    test.add_test(atan2_test((math.pi*2.0, math.pi/2.0, 0.0), (-math.pi*2.0, -math.pi/2.0, 0.0)))
    test.add_test(atan2_test((math.pi*2.0, math.pi/2.0, 1.0), (1.0, 1.0, 1.0)))
    test.add_test(atan2_test((1.0, 1.0, 1.0), (-math.pi*2.0, -math.pi/2.0, 0.0)))
    test.add_test(atan2_test((3.0,), (9.0,)))
    test.add_test(atan2_test((-0.5, -1.0, -3.0, -4.0), (1.0, 1.0, 1.0, 1.0)))
    test.add_test(atan2_test((-0.5, -1.0, -3.0, -4.0), (-1.0, -1.0, -1.0, -1.0)))
    test.add_test(atan2_test((0.5, 1.5, 2.5), (1.0, 1.0, 1.0)))
    test.add_test(atan2_test((1.0, 1.0, 1.0), (0.5, 1.5, 2.5)))

# Test cos in stream programs
test = shtest.StreamTest('cos', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cos', 1))
insert_into1(test, math.cos)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test acos in stream programs
test = shtest.StreamTest('acos', 1)
test.add_call(shtest.Call(shtest.Call.call, 'acos', 1))
insert_into2(test, math.acos)
test.output(sys.stdout, False)

# Test sin in stream programs
test = shtest.StreamTest('sin', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sin', 1))
insert_into1(test, math.sin)
test.output(sys.stdout, False)

# Test asin in stream programs
test = shtest.StreamTest('asin', 1)
test.add_call(shtest.Call(shtest.Call.call, 'asin', 1))
insert_into2(test, math.asin)
test.output(sys.stdout, False)

# Test tan in steam programs
test = shtest.StreamTest('tan', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tan', 1))
insert_into3(test, math.tan)
test.output(sys.stdout, False)

# Test atan in stream programs
test = shtest.StreamTest('atan', 1)
test.add_call(shtest.Call(shtest.Call.call, 'atan', 1))
insert_into4(test, math.atan)
test.output(sys.stdout, False)

# Test atan2 in stream programs
test = shtest.StreamTest('atan2', 2)
test.add_call(shtest.Call(shtest.Call.call, 'atan2', 2))
insert_into5(test)
test.output(sys.stdout, False)

# Test cos in immediate mode
test = shtest.ImmediateTest('cos_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cos', 1))
insert_into1(test, math.cos)
test.output(sys.stdout, False)

# Test acos in immediate mode
test = shtest.ImmediateTest('acos_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'acos', 1))
insert_into2(test, math.acos)
test.output(sys.stdout, False)

# Test sin in immediate mode
test = shtest.ImmediateTest('sin_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sin', 1))
insert_into1(test, math.sin)
test.output(sys.stdout, False)

# Test asin in immediate mode
test = shtest.ImmediateTest('asin_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'asin', 1))
insert_into2(test, math.asin)
test.output(sys.stdout, False)

# Test tan in steam programs
test = shtest.ImmediateTest('tan_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tan', 1))
insert_into3(test, math.tan)
test.output(sys.stdout, False)

# Test atan in immediate mode
test = shtest.ImmediateTest('atan_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'atan', 1))
insert_into4(test, math.atan)
test.output(sys.stdout, False)

# Test atan2 in immediate mode
test = shtest.ImmediateTest('atan2_im', 2)
test.add_call(shtest.Call(shtest.Call.call, 'atan2', 2))
insert_into5(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
