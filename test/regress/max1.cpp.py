#!/usr/bin/python

# Tests the componentwise maximum function

import shtest, sys, common

test = shtest.StreamTest('max', 1)
test.add_call(shtest.Call(shtest.Call.call, 'max', 1))

test.add_test(shtest.make_test([1.9], [[0, 0.1, 0.5, 1.9]], []))
test.add_test(shtest.make_test([4.0], [[1.0, 2.0, 3.0, 4.0]], []))
test.add_test(shtest.make_test([-1.0], [[-1.0, -2.1, -3.5, -4.4]], []))

test.output(sys.stdout)
