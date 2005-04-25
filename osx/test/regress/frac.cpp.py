#!/usr/bin/python

import shtest, sys, common

test = shtest.StreamTest('frac', 1)
test.add_call(shtest.Call(shtest.Call.call, 'frac', 1))

test.add_test(shtest.make_test([0, 0.1, 0.5, 0.9], [[-8.0, -8.9, -8.5, -8.1]], []))
test.add_test(shtest.make_test([0, 0.5, 0, 0.5], [[-0.0, -0.5, 0.0, 0.5]], []))
test.add_test(shtest.make_test([0, 0.9, 0.5, 0.1], [[8.0, 8.9, 8.5, 8.1]], []))

test.output(sys.stdout)
