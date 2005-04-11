#!/usr/bin/python

import shtest, sys, common

def sign(a):
    if a > 0:
        return 1
    elif a < 0:
        return -1
    else:
        return 0

def sign_test(p, types=[]):
    result = [sign(a) for a in p]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(sign_test((-8.0, -8.9, -8.5, -8.1)))
    test.add_test(sign_test((-0.0, -0.5, 0.0, 0.5)))
    test.add_test(sign_test((8.0, 8.9, 8.5, 8.1)))

# Test the sign function in stream programs
test = shtest.StreamTest('sign', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sign', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the sign function in immediate mode
test = shtest.ImmediateTest('sign_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sign', 1))
insert_into(test)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
