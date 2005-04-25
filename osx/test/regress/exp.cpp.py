#!/usr/bin/python

from math import exp

import shtest, sys

def exp_test(p, base, types=[]):
    if base > 0:
        result = [pow(base, a) for a in p]
    else:
        result = [exp(a) for a in p]
    return shtest.make_test(result, [p], types)

def insert_into(test, base=0):
    test.add_test(exp_test((0.0, 1.0, 2.0, 4.0), base))
    test.add_test(exp_test((0.1, 0.25, 0.3, 0.5), base))
    test.add_test(exp_test((2.3, 3.8, -2.0, -3.0), base))
    test.add_test(exp_test((-0.5, -1.0, 2.9, 50.0), base))

# Test exp in stream programs
test = shtest.StreamTest('exp', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test exp2 in stream programs
test = shtest.StreamTest('exp2', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp2', 1))
insert_into(test, 2)
test.output(sys.stdout, False)

# Test exp10 in stream programs
test = shtest.StreamTest('exp10', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp10', 1))
insert_into(test, 10)
test.output(sys.stdout, False)

# Test exp in immediate mode
test = shtest.ImmediateTest('exp_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp', 1))
insert_into(test)
test.output(sys.stdout, False)

# Test exp2 in immediate mode
test = shtest.ImmediateTest('exp2_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp2', 1))
insert_into(test, 2)
test.output(sys.stdout, False)

# Test exp10 in immediate mode
test = shtest.ImmediateTest('exp10_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'exp10', 1))
insert_into(test, 10)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
