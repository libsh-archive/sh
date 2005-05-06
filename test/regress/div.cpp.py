#!/usr/bin/python

import shtest, sys, common
from common import *

def div(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a / b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p / b for b in q]
    else:
        result = [a / q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=False):
    test.add_test(div((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
    test.add_test(div((1.0, 2.0, 3.0), (7.0,)))
    test.add_test(div((1.0,), (7.0,)))
    test.add_test(div((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

    test.add_test(div((1, 2, 3), (3, 3, 3), ['i', 's', 's']))
    test.add_test(div((0, 1, 2, 3), (4, 4, 4, 4), ['i', 'i', 'i']))
    test.add_test(div((4, 5, 6, 7), (4, 4, 4, 4), ['i', 'i', 'i']))
    test.add_test(div((8, 9, 10, 11), (4, 4, 4, 4), ['i', 'i', 'i']))
    test.add_test(div((1.0, 2.0, 3.0), (3, 3, 3), ['f', 'f', 's']))
    test.add_test(div((1, 2, 3), (3.0, 3.0, 3.0), ['s', 's', 'f']))

    test.add_test(div((-4.0, -5.0, -6.0), 0.3)) # N / 1
    if not modifying:
        test.add_test(div((1.0,), (3.0, 4.0, 5.0)))
        test.add_test(div(1.2, (-4.0, 5.1, 0.6))) # 1 / N

# Test regular division in stream programs
test = shtest.StreamTest('div', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying division in stream programs
test = shtest.StreamTest('mdiv', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

# Test regular division in immediate mode
test = shtest.ImmediateTest('div_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/', 2))
insert_into(test)
test.output(sys.stdout, False)

# Test modifying division in immediate mode
test = shtest.ImmediateTest('mdiv_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
