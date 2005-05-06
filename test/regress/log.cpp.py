#!/usr/bin/python

from math import log

import shtest, sys

def log_test(p, base, types=[]):
    if base > 0:
        result = [log(a, base) for a in p]
    else:
        result = [log(a) for a in p]
    return shtest.make_test(result, [p], types)

def insert_into(test, base=0):
    test.add_test(log_test((0.0001, 1.0, 2.0, 4.0), base))
    test.add_test(log_test((0.1, 0.25, 0.3, 0.5), base))
    test.add_test(log_test((2.3, 3.8, 10.0, 30.0), base))
    test.add_test(log_test((0.95, 1.1, 2.9, 5000.0), base))

# Test log in stream programs
test = shtest.StreamTest('log', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log', 1))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test log2 in stream programs
test = shtest.StreamTest('log2', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log2', 1))
insert_into(test, 2)
test.output(sys.stdout, False)

# Test log10 in stream programs
test = shtest.StreamTest('log10', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log10', 1))
insert_into(test, 10)
test.output(sys.stdout, False)

# Test log in immediate mode
test = shtest.ImmediateTest('log_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log', 1))
insert_into(test)
test.output(sys.stdout, False)

# Test log2 in immediate mode
test = shtest.ImmediateTest('log2_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log2', 1))
insert_into(test, 2)
test.output(sys.stdout, False)

# Test log10 in immediate mode
test = shtest.ImmediateTest('log10_im', 1)
test.add_call(shtest.Call(shtest.Call.call, 'log10', 1))
insert_into(test, 10)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
