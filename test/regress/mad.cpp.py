#!/usr/bin/python

import shtest, sys, common
from common import *

def mad(p, q, r, types=[]):
    if is_array(p) and is_array(q) and is_array(r):
        result = [a * b + c for (a,b,c) in upzip3(p, q, r)]
    elif is_array(q) and is_array(r):
        result = [p * b + c for (b,c) in upzip(q, r)]
    elif is_array(p) and is_array(r):
        result = [a * q + c for (a,c) in upzip(p, r)]
    return shtest.make_test(result, [p, q, r], types)

def insert_into(test):
    test.add_test(mad((0.0, 1.0, 2.0), (3.0, 4.0, 5.0), (1.0, 1.1, 1.2)))
    test.add_test(mad((1.0,), (7.0,), (-5000.0,)))
    test.add_test(mad((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0), (0.0, 0.0, 0.0)))

    test.add_test(mad(7.0, (1.0, 2.0, 3.0), (-1.0, 0.5, 10.0))) # mad(1, N, N)
    test.add_test(mad((1.0, 2.0, 3.0), 7.0, (-1.0, 0.5, 10.0))) # mad(N, 1, N)

# Test mad in stream programs
test = shtest.StreamTest('mad', 3)
test.add_call(shtest.Call(shtest.Call.call, 'mad', 3))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test mad in immediate mode
test = shtest.ImmediateTest('mad_im', 3)
test.add_call(shtest.Call(shtest.Call.call, 'mad', 3))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
