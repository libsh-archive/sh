#!/usr/bin/python

import shtest, sys, common

def mmod(p, q, types=[]):
    result = [a % b for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('mmod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%=', 2))

test.add_test(mmod((3, 3, 3), (4, 3, 2), ['i', 'i', 'i']))
test.add_test(mmod((-3, -3, -3), (4, 3, 2), ['i', 'i', 'i']))
test.add_test(mmod((3, 3, 3), (-4, -3, -2), ['i', 'i', 'i']))
test.add_test(mmod((-3, -3, -3), (-4, -3, -2), ['i', 'i', 'i']))
    
test.add_test(mmod((3.0, 3.0, 3.0), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
test.add_test(mmod((-3.0, -3.0, -3.0), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
test.add_test(mmod((3.0, 3.0, 3.0), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
test.add_test(mmod((-3.0, -3.0, -3.0), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))

test.add_test(mmod((3.1, 3.5, 3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
test.add_test(mmod((-3.1, -3.5, -3.9), (4.0, 3.0, 2.0), ['f', 'f', 'f']))
test.add_test(mmod((3.1, 3.5, 3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))
test.add_test(mmod((-3.1, -3.5, -3.9), (-4.0, -3.0, -2.0), ['f', 'f', 'f']))

test.output(sys.stdout)
