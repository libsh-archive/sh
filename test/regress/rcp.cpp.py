#!/usr/bin/python

import shtest, sys, common

def rcp_test(p, types=[]):
    result = [1.0 / a for a in p]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(rcp_test((0.5, 1.0, 2.0, 4.0)))
    test.add_test(rcp_test((0.1, 0.25, 0.3, 0.5)))
    test.add_test(rcp_test((2.3, 3.8, 2.0, 3.0)))
    test.add_test(rcp_test((0.5, 1.0, 2.9, 500.0)))

    test.add_test(rcp_test((-0.5, -1.0, -2.0, -4.0)))
    test.add_test(rcp_test((-0.1, -0.25, -0.3, -0.5)))
    test.add_test(rcp_test((-2.3, -3.8, -2.0, -3.0)))
    test.add_test(rcp_test((-0.5, -1.0, -2.9, -500.0)))

# Test reciprocal function in stream programs
test = shtest.StreamTest('rcp', 1)
test.add_call(shtest.Call(shtest.Call.call, 'rcp', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test reciprocal function in immediate mode
test = shtest.ImmediateTest('rcp_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'rcp', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
