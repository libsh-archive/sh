#!/usr/bin/python

import shtest, sys, common
from common import *
from math import *

def frac_test(p, types=[]):
    if is_array(p):
        result = [a - floor(a) for a in p]
    else:
        result = [a - floor(a)]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(frac_test((-8.0, -8.9, -8.5, -8.1)))
    test.add_test(frac_test((-0.0, -0.5, 0.0, 0.5)))
    test.add_test(frac_test((8.0, 8.9, 8.5, 8.1)))
    test.add_test(frac_test((18.0, 18.9, -18.1)))
    test.add_make_test((0, 0), [(1098908.975123, -12131318.123456)])
    test.add_test(frac_test((1234567890123456789.0, )))
    test.add_test(frac_test((-1234567890123456789.0, )))

# Test frac in stream programs
test = shtest.StreamTest('frac', 1)
test.add_call(shtest.Call(shtest.Call.call, 'frac', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test frac in immediate mode
test = shtest.ImmediateTest('frac_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'frac', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
