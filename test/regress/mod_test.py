#!/usr/bin/python

import shtest, common
from common import *

def mod(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a % b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p % b for b in q]
    else:
        result = [a % q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=0):
    test.add_test(mod((3, 3, 3), (4, 3, 2), ['i', 'i', 'i']))
    test.add_test(mod((-3, -3, -3), (4, 3, 2), ['i', 'i', 'i']))
    test.add_test(mod((3, 3, 3), (-4, -3, -2), ['i', 'i', 'i']))
    test.add_test(mod((-3, -3, -3), (-4, -3, -2), ['i', 'i', 'i']))
    
    test.add_test(mod((3.0, 3.0, 3.0), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.0, -3.0, -3.0), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((3.0, 3.0, 3.0), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.0, -3.0, -3.0), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))

    test.add_test(mod((3.1, 3.5, 3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.1, -3.5, -3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
    test.add_test(mod((3.1, 3.5, 3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
    test.add_test(mod((-3.1, -3.5, -3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))

    test.add_test(mod((1.0, 5.0, 2.4), 0.5))
    test.add_test(mod((1.0, 5.0, 2.4), -0.5))
    if not modifying:
        test.add_test(mod(3.0, (1.0, 5.0, 2.4)))
        test.add_test(mod(-3.0, (1.0, 5.0, 2.4)))
