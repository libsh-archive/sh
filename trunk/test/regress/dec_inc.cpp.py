#!/usr/bin/python

import shtest, sys, common

def inc(p, types=[]):
    result = [a + 1 for (a) in (p)]
    return shtest.make_test(result, [p], types)

def dec(p, types=[]):
    result = [a - 1 for (a) in (p)]
    return shtest.make_test(result, [p], types)

def insert_into(test, func):
    test.add_test(func((-1.5, 0.0, 0.5, 3.0)))

# Test the decrement operation in stream programs
test = shtest.StreamTest('inc', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '++', 1))
insert_into(test, inc)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test the decrement operation in stream programs
test = shtest.StreamTest('dec', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '--', 1))
insert_into(test, dec)
test.output(sys.stdout, False)

# Test the decrement operation in immediate mode
test = shtest.ImmediateTest('inc_im', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '++', 1))
insert_into(test, inc)
test.output(sys.stdout, False)

# Test the decrement operation in immediate mode
test = shtest.ImmediateTest('dec_im', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '--', 1))
insert_into(test, dec)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
