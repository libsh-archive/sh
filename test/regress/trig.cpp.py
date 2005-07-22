#!/usr/bin/python

import shtest, sys, math, common, cmath

def func(l, op, types=[], epsilon=0):
    return shtest.make_test([op(x) for x in l], [l], types, epsilon)

def atan2_test(p, q, types=[]):
    result = [math.atan2(b, a) for (a, b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

# for cos(), sin(), cosh() and sinh()
def insert_into1(test, op):
    test.add_test(func((0.0, 1.0, 2.0, 3.0), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((math.pi, -math.pi, 0.0), op))
    test.add_test(func((math.pi*2.0, -math.pi*2.0), op, [], 0.06))
    test.add_test(func((math.pi/2.0, -math.pi/2.0), op, [], 0.0005))
    test.add_test(func((3.0,), op))
    test.add_test(func((-0.5, -1.0, -3.0, -4.0), op))
    test.add_test(func((0.5, 1.5, 2.5), op))

# for acos(), asin() and atanh()
def insert_into2(test, op):
    test.add_test(func((0.0,), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((1 - 0.1, -1 + 0.1, 0.0), op))
    test.add_test(func((-0.5, -0.9, -0.1), op))
    test.add_test(func((0.5, 0.6, 0.9), op))

# for tan() and tanh()
def insert_into3(test, op):
    test.add_test(func((0.0, 1.0, 2.0, 3.0), op))
    test.add_test(func((0.3, 0.5, 0.8, 0.9), op))
    test.add_test(func((math.pi/2 - 0.1, -math.pi/2 + 0.1, 0.0), op))
    test.add_test(func((-0.5, -1.0, -1.1), op))
    test.add_test(func((0.5, 1.5, 2.5), op))

# for atan(), asinh()
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
    #test.add_test(atan2_test((math.pi, -math.pi), (1.0, 1.0)))
    test.add_test(atan2_test((math.pi*2.0, math.pi/2.0), (-math.pi*2.0, -math.pi/2.0)))
    test.add_test(atan2_test((math.pi*2.0, math.pi/2.0, 1.0), (1.0, 1.0, 1.0)))
    test.add_test(atan2_test((1.0, 1.0, 1.0), (-math.pi*2.0, -math.pi/2.0, 0.0)))
    test.add_test(atan2_test((3.0,), (9.0,)))
    #test.add_test(atan2_test((0.5, -1.0, -3.0, -3.0), (-1.0, 1.0, 1.0, -1.0)))
    test.add_test(atan2_test((0.5, 1.5, 2.5), (1.0, 1.0, 1.0)))
    test.add_test(atan2_test((1.0, 1.0, 1.0), (0.5, 1.5, 2.5)))

# for acosh()
def insert_into6(test, op):
    test.add_test(func((1.0,), op))
    test.add_test(func((50.3, 2000.5), op))
    test.add_test(func((1.3, 2.5, 3.8, 10.9), op))
    test.add_test(func((1 + 0.001, 1 + 0.01, 1.0 + 0.1), op))

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

# Test tan in stream programs
test = shtest.StreamTest('tan', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tan', 1))
insert_into3(test, math.tan)
test.output(sys.stdout, False)

# Test cosh in stream programs
test = shtest.StreamTest('cosh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cosh', 1))
insert_into1(test, math.cosh)
test.output(sys.stdout, False)

# Test acosh in stream programs
test = shtest.StreamTest('acosh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'acosh', 1))
insert_into6(test, cmath.acosh)
test.output(sys.stdout, False)

# Test sinh in stream programs
test = shtest.StreamTest('sinh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sinh', 1))
insert_into1(test, math.sinh)
test.output(sys.stdout, False)

# Test asinh in stream programs
test = shtest.StreamTest('asinh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'asinh', 1))
insert_into4(test, cmath.asinh)
test.output(sys.stdout, False)

# Test tanh in stream programs
test = shtest.StreamTest('tanh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tanh', 1))
insert_into3(test, math.tanh)
test.output(sys.stdout, False)

# Test atanh in stream programs
test = shtest.StreamTest('atanh', 1)
test.add_call(shtest.Call(shtest.Call.call, 'atanh', 1))
insert_into2(test, cmath.atanh)
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

# Test tan in immediate mode
test = shtest.ImmediateTest('tan_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tan', 1))
insert_into3(test, math.tan)
test.output(sys.stdout, False)

# Test cosh in immediate mode
test = shtest.ImmediateTest('cosh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cosh', 1))
insert_into1(test, math.cosh)
test.output(sys.stdout, False)

# Test acosh in immediate mode
test = shtest.ImmediateTest('acosh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'acosh', 1))
insert_into6(test, cmath.acosh)
test.output(sys.stdout, False)

# Test sinh in immediate mode
test = shtest.ImmediateTest('sinh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sinh', 1))
insert_into1(test, math.sinh)
test.output(sys.stdout, False)

# Test asinh in immediate mode
test = shtest.ImmediateTest('asinh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'asinh', 1))
insert_into4(test, cmath.asinh)
test.output(sys.stdout, False)

# Test tanh in immediate mode
test = shtest.ImmediateTest('tanh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tanh', 1))
insert_into3(test, math.tanh)
test.output(sys.stdout, False)

# Test atanh in immediate mode
test = shtest.ImmediateTest('atanh_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'atanh', 1))
insert_into2(test, cmath.atanh)
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
