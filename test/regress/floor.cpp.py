#!/usr/bin/python

import shtest, sys, common

test = shtest.StreamTest('floor', 1)
test.add_call(shtest.Call(shtest.Call.call, 'floor', 1))

test.add_test(shtest.make_test([3.0, 3.0, 3.0, 3.0], [[3.0, 3.1, 3.5, 3.9]], []))
test.add_test(shtest.make_test([-3.0, -4.0, -4.0, -4.0], [[-3.0, -3.1, -3.5, -3.9]], []))

test.output(sys.stdout)
