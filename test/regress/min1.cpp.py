#!/usr/bin/python

# Tests the componentwise minimum function

import shtest, sys, common

test = shtest.StreamTest('min', 1)
test.add_call(shtest.Call(shtest.Call.call, 'min', 1))

test.add_test(shtest.make_test([0.0], [[0, 0.1, 0.5, 1.9]], []))
test.add_test(shtest.make_test([1.0], [[1.0, 2.0, 3.0, 4.0]], []))
test.add_test(shtest.make_test([-4.4], [[-1.0, -2.1, -3.5, -4.4]], []))

test.output(sys.stdout)
