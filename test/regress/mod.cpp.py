#!/usr/bin/python

import shtest, sys, common

def mod(p, q, types=[]):
    result = [a % b for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def insert_into(test):
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

test = shtest.StreamTest('mod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%', 2))
insert_into(test)
test.output(sys.stdout)
