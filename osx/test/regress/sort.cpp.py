#!/usr/bin/python

import shtest, sys, common

test = shtest.StreamTest('sort', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sort', 1))

test.add_test(shtest.make_test([0.0, 0.1, 0.5, 1.9], [[1.9, 0.1, 0.5, 0.0]], []))
test.add_test(shtest.make_test([-4.4, -3.5, -2.1, -1.5], [[-1.5, -2.1, -3.5, -4.4]], []))
test.add_test(shtest.make_test([1.0, 2.0, 3.0, 4.0], [[2.0, 1.0, 4.0, 3.0]], []))
test.add_test(shtest.make_test([1.0, 2.0, 3.0, 4.0], [[1.0, 2.0, 3.0, 4.0]], []))
test.add_test(shtest.make_test([-2.1, -1.5, 0.0, 6.7], [[6.7, 0, -2.1, -1.5]], []))

test.output(sys.stdout)
