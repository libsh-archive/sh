#!/usr/bin/python

import shtest, sys, common

def add(p, q, types=[]):
    result = [a + b for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

def insert_into(test):
		test.add_test(add((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
		test.add_test(add((1.0,), (3.0, 4.0, 5.0)))
		test.add_test(add((1.0, 2.0, 3.0), (7.0,)))
		test.add_test(add((1.0,), (7.0,)))
		test.add_test(add((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

		test.add_test(add((0, 1, 2), (3, 4, 5), ['i', 'i', 'i']))
		test.add_test(add((1,), (3, 4, 5), ['s', 's', 's']))
		test.add_test(add((1, 2, 3), (7,), ['i', 's', 'b']))
		test.add_test(add((1,), (7,), ['i', 'i', 'i']))
		test.add_test(add((4, 5, 6), (-4, -5, -6), ['i', 'i', 'i']))

		test.add_make_test((0, 0.5, 1, 1), [(-4, 0.3, 1.0, 2.0), (-2, 0.2, 0.0, 2.0)], ['fui', 'f', 'f'])

		test.add_make_test((0, 1, 2), [(1.3, 2.97, -5.4), (-.31, -1.5, 8)], ['i', 'f', 'f'])
