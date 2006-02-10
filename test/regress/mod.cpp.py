#!/usr/bin/python

import shtest, sys, common
from common import *

def mod(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a % b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p % b for b in q]
    else:
        result = [a % q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=False):
    test.add_test(mod((3, 3), (4, 2), ['i', 'i']))
    test.add_test(mod((-3, -3, -3), (4, 3, 2), ['i', 'i', 'i']))
    test.add_test(mod((3, 3, 3), (-4, -3, -2), ['i', 'i', 'i']))
    test.add_test(mod((-3, -3), (-4, -2), ['i', 'i']))
    
    test.add_test(mod((3.0, 3.0), (4.0, 2.0), ['f', 'f']))
    test.add_test(mod((-3.0, -3.0, -3.0), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((3.0, 3.0, 3.0), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.0, -3.0), (-4.0, -2.0), ['f', 'f']))

    test.add_test(mod((3.1, 3.5, 3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.1, -3.5, -3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((3.1, 3.5, 3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.1, -3.5, -3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))

    #test.add_test(mod((10, -10), (10, -10), ['f', 'f'])) # Broken on old ATI cards
    test.add_test(mod((2, -2, 0.5, -0.5), (2, -2, 0.5, -0.5), ['f', 'f', 'f', 'f']))
    test.add_test(mod((64, -64), (64, -64), ['i', 'i']))

    test.add_test(mod((1.0, 5.0, 2.4), 0.5))
    test.add_test(mod((1.0, 5.0, 2.4), -0.5))
    if not modifying:
        test.add_test(mod(3.0, (1.0, 5.0, 2.4)))
        test.add_test(mod(-3.0, (1.0, 5.0, 2.4)))

# Test regular mod in stream programs
test = shtest.StreamTest('mod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%', 2))
insert_into(test)
test.output_header(sys.stdout)
test.output(sys.stdout, False)

# Test modifying mod in stream programs
test = shtest.StreamTest('mmod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

# Test regular mod in immediate mode
test = shtest.ImmediateTest('mod_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%', 2))
insert_into(test)
test.output(sys.stdout, False)

# Test modifying mod in immediate mode
test = shtest.ImmediateTest('mmod_im', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%=', 2))
insert_into(test, True)
test.output(sys.stdout, False)

test.output_footer(sys.stdout)
