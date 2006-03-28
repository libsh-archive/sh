#!/usr/bin/python

import shtest, sys, common
from common import *
from math import *

def floor_test(p, types=[]):
    if is_array(p):
        result = [floor(a) for a in p]
    else:
        result = [floor(p)]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(floor_test((3.0, 3.1, 3.5, 3.9)))
    test.add_test(floor_test((-3.0, -3.1, -3.5, -3.9)))
    test.add_test(floor_test(-0.75))
    test.add_test(floor_test(0.75))
    test.add_test(floor_test(0.2))
    test.add_test(floor_test(-0.2))
    test.add_test(floor_test((-0.0, 0.0)))
    test.add_test(floor_test((-12345.6787, 12324.4838)))
    test.add_test(floor_test((1234567890123456789.0, )))
    test.add_test(floor_test((-1234567890123456789.0, )))

# Test floor in stream programs
test = shtest.StreamTest('floor', 1)
test.add_call(shtest.Call(shtest.Call.call, 'floor', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test floor in immediate mode
test = shtest.ImmediateTest('floor_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'floor', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
