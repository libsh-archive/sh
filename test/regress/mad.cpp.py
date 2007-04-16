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
    elif is_array(p) and is_array(q):
        result = [a * b + r for (a,b) in upzip(p, q)]
    return shtest.make_test(result, [p, q, r], types)

def insert_into(test):
    # Testing with N-tuples(N) & 1-tuples(1)
    test.add_test(mad((1.0,), (7.0,), (-5000.0,)))                                          # mad(1, 1, 1)
    test.add_test(mad((4.0, 5.0), (-4.0, -5.0), (0.0, 0.0)))                                # mad(2, 2, 2)
    test.add_test(mad((0.0, 1.0, 2.0), (3.0, 4.0, 5.0), (1.0, 1.1, 1.2)))                   # mad(3, 3, 3)	
    test.add_test(mad((4.0, 5.0, 6.0, 7.0), (-4.0, -5.0, 6.0, 7.0), (0.0, 0.0, 6.0, 7.0)))  # mad(4, 4, 4)

    test.add_test(mad((0.0, 1.0, 2.0), (3.0, 4.0, 5.0), (1.0,)))                            # mad(N, N, 1)
    test.add_test(mad((0.00001, 1.0, 20.0), (-3.0,), (1.0, 1.1, 1.2)))                      # mad(N, 1, N)
    test.add_test(mad((0.0, 1.0, 2.0), (3.0,), (1.0,)))                                     # mad(N, 1, 1)
    test.add_test(mad((70.0,), (1.0, 2.2, 3.0), (-1.0, 0.5, 10.0)))                         # mad(1, N, N)
    test.add_test(mad((0.0,), (3.0, 4.0, 5.0), (1.0,)))                                     # mad(1, N, 1)
    test.add_test(mad((1.0,), (5.0003,), (1.0, 1.99991, 1.2)))                              # mad(1, 1, N)

		# Testing with integers
    test.add_test(mad((1, 2, 3), (2, 3, -40), (1.0,), ['i', 'i', 'f']))                     # mad(N, N, 1)
    test.add_test(mad((0.00001, 1.0, 20.0), (-999,), (1.0, 1.1, 1.2), ['f', 'i', 'f']))     # mad(N, 1, N)
		
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
