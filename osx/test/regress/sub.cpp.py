#!/usr/bin/python

import shtest, sys, common
from common import *

def sub(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a - b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p - b for b in q]
    else:
        result = [a - q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=False):
    test.add_test(sub((3.0, 4.0, 5.0), (0.0, 1.0, 2.0)))
    test.add_test(sub((3.0, 4.0, 5.0), (1.0,)))
        
    test.add_test(sub((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))
    test.add_test(sub((4.0, 5.0, 6.0), (4.0, 5.0, 6.0)))
    test.add_test(sub((0.50, 2.0, -3.3333), 1000))

    if not modifying:
        test.add_test(sub((7.0,), (1.0, 2.0, 3.0)))
        test.add_test(sub((1.0,), (7.0,)))
        test.add_test(sub(-0.5, (0.50, 2.0, -3.3333)))

# Test regular subtraction in stream programs
test = shtest.StreamTest('sub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying subtraction in stream programs
test = shtest.StreamTest('msub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-=', 2))
insert_into(test, 1)
test.output(sys.stdout, False)

# Test regular subtraction in immediate mode
test = shtest.StreamTest('sub_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-', 2))
insert_into(test)
test.output(sys.stdout, False)

# Test modifying subtraction in immediate mode
test = shtest.ImmediateTest('msub_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
