#!/usr/bin/python

import shtest, sys, common
from common import *
from math import *

def ceil_test(p, types=[]):
    if is_array(p):
        result = [ceil(a) for a in p]
    else:
        result = [ceil(p)]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(ceil_test((3.0, 3.1, 3.5, 3.9)))
    test.add_test(ceil_test((-3.0, -3.1, -3.5, -3.9)))
    test.add_test(ceil_test(-0.75))
    test.add_test(ceil_test(0.75))
    test.add_test(ceil_test(0.2))
    test.add_test(ceil_test(-0.2))
    test.add_test(ceil_test((-0.0, 0.0)))
    test.add_test(ceil_test((-12345.6787, 12324.4838)))
    test.add_test(ceil_test((1234567890123456789.0, )))
    test.add_test(ceil_test((-1234567890123456789.0, )))

# Test ceil in stream programs
test = shtest.StreamTest('ceil', 1)
test.add_call(shtest.Call(shtest.Call.call, 'ceil', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test ceil in immediate mode
test = shtest.ImmediateTest('ceil_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'ceil', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
