#!/usr/bin/python

import shtest, sys

#for smoothstep()
def insert_into1(test):
    test.add_make_test((0.041015,), [(-10.5,), (110.0,), (4.2,)], ['f', 'f', 'f', 'f'])		
    test.add_make_test((0.736594,), [(99.9,), (-50.5,), (0.1,)], ['f', 'f', 'f', 'f'])
    test.add_make_test((0.000246928, 0.00132811), [(0.1, -0.1), (99.0, 99.0), (1.0, 2.0)], ['f', 'f', 'f', 'f'])
    test.add_make_test((0.0, 0.0, 1.0), [(10.0, 21.2, 3.0), (1.0, 22.3, 3.0), (11.0, 2.0, 43.0)], ['f', 'f', 'f', 'f'])
    test.add_make_test((1.0, 1.0, 1.0, 0.0), [(1.0, 2.1, 13.0, 4.0), (1.0, 12.0, 3.0, 14.0), (11.0, 12.0, 3.0, 4.0)], ['f', 'f', 'f', 'f'])

# Fails on arb/glsl:
#    test.add_make_test((1.0, 1.0, 1.0, 0.0), [(1.0, 2.1, 13.0, 4.0), (1.0, 12.0, 3.0, 14.0), (11.0, 12.0, 3.0, 4.0)], ['fui', 'f', 'f', 'f'])
		
# for smoothpulse()
def insert_into2(test):
    test.add_make_test((-0.0327684,), [(0.5,), (10.0,), (-4.2,), (-0.01,)], ['f', 'f', 'f', 'f', 'f'])		
    test.add_make_test((-0.266927,), [(99.9,), (-50.5,), (0.1,), (0.5)], ['f', 'f', 'f', 'f', 'f'])
    test.add_make_test((-1.0, 0.0233821), [(5.1, -1.1), (99.0, -99.9), (1.0, 2.0), (-99.0, -10.1)], ['f', 'f', 'f', 'f', 'f'])
    test.add_make_test((1.0, -0.999677, 0.999832), [(10.0, 21.2, 3.0), (1.0, 22.3, 3.0), (11.0, 2.0, 43.0), (-1.0, 2.2, 3.3)], ['f', 'f', 'f', 'f', 'f'])
    test.add_make_test((0.0, 0.0, -0.104, -1.0), [(1.4, -2.1, 13.0, 4.4), (1.5, 12.0, -3.0, 14.0), (11.1, 12.0, 3.0, 4.0), (-22.2, -32.2, 0.2, 2.2)], ['f', 'f', 'f', 'f', 'f'])

#for linstep()
def insert_into3(test):
    test.add_make_test((1.0,), [(0.5,), (10.0,), (-4.2,)], ['f', 'f', 'f', 'f'])		
    test.add_make_test((1.0,), [(99.9,), (-50.5,), (0.1,)], ['f', 'f', 'f', 'f'])
    test.add_make_test((0.8, 1.0), [(0.1, -0.1), (1.0, 2.0), (-3.0, -4.0)], ['f', 'f', 'f', 'f'])
    test.add_make_test((0.5, 0.5, 0.5), [(1.0, 2.0, 3.0), (1.0, 2.0, 3.0), (1.0, 2.0, 3.0)], ['f', 'f', 'f', 'f'])
    test.add_make_test((0.501961, 0.0, 0.501961, 0.0), [(1.0, -2.0, 3.0, -4.0), (1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0)], ['fui', 'f', 'f', 'f'])

# Test the smoothstep function in stream programs
test = shtest.StreamTest('smoothstep', 3)
test.add_call(shtest.Call(shtest.Call.call, 'smoothstep', 3))
insert_into1(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the smoothpulse function in stream programs
test = shtest.StreamTest('smoothpulse', 4)
test.add_call(shtest.Call(shtest.Call.call, 'smoothpulse', 4))
insert_into2(test)
test.output(sys.stdout, False)

# Test the linstep function in stream programs
test = shtest.StreamTest('linstep', 3)
test.add_call(shtest.Call(shtest.Call.call, 'linstep', 3))
insert_into3(test)
test.output(sys.stdout, False)

# Test the smoothstep function in immediate mode
test = shtest.ImmediateTest('smoothstep_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'smoothstep', 3))
insert_into1(test)
test.output(sys.stdout, False)

# Test the smoothpulse function in immediate mode
test = shtest.ImmediateTest('smoothpulse_im', 4)
test.add_call(shtest.Call(shtest.Call.call, 'smoothpulse', 4))
insert_into2(test)
test.output(sys.stdout, False)

# Test the linstep function in immediate mode
test = shtest.ImmediateTest('linstep_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'linstep', 3))
insert_into3(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
