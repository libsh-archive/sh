#!/usr/bin/python

import shtest, sys, common
from common import *

def add(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a + b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p + b for b in q]
    else:
        result = [a + q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=False):
    test.add_test(add((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
    test.add_test(add((1.0, 2.0, 3.0), (7.0,)))
    test.add_test(add((1.0,), (7.0,)))
    test.add_test(add((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

    test.add_test(add((0, 1, 2), (3, 4, 5), ['i', 'i', 'i']))
    test.add_test(add((1, 2, 3), (7,), ['i', 's', 'b']))
    test.add_test(add((1,), (7,), ['i', 'i', 'i']))
    test.add_test(add((4, 5, 6), (-4, -5, -6), ['i', 'i', 'i']))
    
    test.add_make_test((0, 0.5, 1, 1), [(-4, 0.3, 1.0, 2.0), (-2, 0.2, 0.0, 2.0)], ['fui', 'f', 'f'])

    # type conversions
    test.add_test(add((0.5, 10.8, 200, 50000), (-1.5, 25.2, -500, 55000), ['i', 'f', 'f']))
    test.add_test(add((0.5, 10.8, 200, 50000), (1.5, 25.2, 500, 55000), ['ui', 'f', 'f']))
    test.add_test(add((0, 10, 200, 50000), (-1, 25, -500, 55000), ['f', 'i', 'i']))
    test.add_test(add((0, 10, 200, 50000), (1, 25, 500, 55000), ['f', 'ui', 'ui']))
    
    test.add_make_test((0, 1, 2), [(1.3, 2.97, -5.4), (-.31, -1.5, 8)], ['i', 'f', 'f'])

    test.add_test(add((3, 4.2, 50.0, -503.47), -8.2)) # N + 1

    if not modifying:
        test.add_test(add((1.0,), (3.0, 4.0, 5.0)))
        test.add_test(add((1,), (3, 4, 5), ['s', 's', 's']))
        test.add_test(add(1, (3, 4.2, 50.0, -503.47))) # 1 + N

# Test regular add in stream programs
test = shtest.StreamTest('add', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+', 2))
insert_into(test)

test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying add in stream programs
test = shtest.StreamTest('madd', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+=', 2))
insert_into(test, True)
test.output(sys.stdout,False)

# Test regular add in immediate mode
test = shtest.ImmediateTest('add_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+', 2))
insert_into(test)
test.output(sys.stdout,False)

# Test modifying add in immediate mode
test = shtest.ImmediateTest('madd_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
