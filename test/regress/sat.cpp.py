#!/usr/bin/python

import shtest, sys, common

test = shtest.StreamTest('sat', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sat', 1))

test.add_test(shtest.make_test([1, 1, 1, 1], [[3.0, 3.1, 3.5, 3.9]], []))
test.add_test(shtest.make_test([-0.5, -0.0, 0.5, 1.0], [[-0.5, -0.0, 0.5, 1.0]], []))
test.add_test(shtest.make_test([-3.0, -3.1, -3.5, -3.9], [[-3.0, -3.1, -3.5, -3.9]], []))

test.output(sys.stdout)
