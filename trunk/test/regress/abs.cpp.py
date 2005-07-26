#!/usr/bin/python

import shtest, sys, common
from common import *

def abs_test(p, types=[]):
    if is_array(p):
        result = [abs(a) for a in p]
    else:
        result = [abs(p)]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(abs_test((-8.0, -8.9, -8.5, -8.1)))
    test.add_test(abs_test((-0.0, -0.5, 0.0, 0.5)))
    test.add_test(abs_test((8.0, 8.9, 8.5, 8.1)))

# Test abs in stream programs
test = shtest.StreamTest('abs', 1)
test.add_call(shtest.Call(shtest.Call.call, 'abs', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test abs in immediate mode
test = shtest.ImmediateTest('abs_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'abs', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
