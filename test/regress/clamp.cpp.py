#!/usr/bin/python

import shtest, sys, common

test = shtest.StreamTest('clamp', 3)
test.add_call(shtest.Call(shtest.Call.call, 'clamp', 3))

test.add_test(shtest.make_test([0.0, 0.5, 1.0, 1.0], [[0.0, 0.5, 1.0, 1.5], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))
test.add_test(shtest.make_test([0.0, 0.0, 0.0, 0.0], [[-5.2, -1.0, -0.5, -0.0], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))
test.add_test(shtest.make_test([1.0, 1.0, 1.0, 1.0], [[1.0, 1.1, 5.0, 100], [0.0, 0.0, 0.0, 0.0], [1.0, 1.0, 1.0, 1.0]], []))

test.add_test(shtest.make_test([1.0, 1.0, 1.0, 1.2], [[0.0, 0.5, 1.0, 1.5], [1.0, 1.0, 1.0, 1.0], [1.2, 1.2, 1.2, 1.2]], []))
test.add_test(shtest.make_test([-0.5, -0.5, -0.5, -0.4], [[-5.2, -1.0, -0.5, -0.0], [-0.5, -0.5, -0.5, -0.5], [-0.4, -0.4, -0.4, -0.4]], []))
test.add_test(shtest.make_test([5.0, 5.0, 5.0, 10.0], [[1.0, 1.1, 5.0, 100], [5.0, 5.0, 5.0, 5.0], [10.0, 10.0, 10.0, 10.0]], []))

# clamp(N, 1, 1)
test.add_test(shtest.make_test([5.0, 5.0, 10.0, 10.0], [[1.0, 5.0, 10, 100], 5.0, 10.0], []))

test.output(sys.stdout)
