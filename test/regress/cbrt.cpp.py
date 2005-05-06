#!/usr/bin/python

import shtest, sys, math

def cbrt(l, types=[]):
    return shtest.make_test([math.pow(x, 1/3.0) for x in l], [l], types)

def insert_into(test):
    test.add_test(cbrt((0.0, 1.0, 2.0, 3.0)))
    test.add_test(cbrt((1.0,)))
    test.add_make_test((3,), [(27,)], ['i', 'i'])

# Test the cube root in stream programs
test = shtest.StreamTest('cbrt', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cbrt', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the cube root in immediate mode
test = shtest.ImmediateTest('cbrt_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cbrt', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
