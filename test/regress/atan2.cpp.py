#!/usr/bin/python

import shtest, sys, math, common

def atan2_test(p, q, types=[]):
    result = [math.atan2(a, b) for (a, b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('atan2', 2)
test.add_call(shtest.Call(shtest.Call.call, 'atan2', 2))

test.add_test(atan2_test((1.0, 1.0, 1.0, 1.0), (0.0, 1.0, 2.0, 3.0)))
test.add_test(atan2_test((0.5, 1.5, 3.0, 50.0), (0.0, 1.0, 2.0, 3.0)))
test.add_test(atan2_test((1.0, 1.0, 1.0, 1.0), (0.3, 0.5, 0.8, 0.9)))
test.add_test(atan2_test((1.0, 1.0, 1.0), (math.pi, -math.pi, 0.0)))
test.add_test(atan2_test((math.pi, -math.pi, 0.0), (1.0, 1.0, 1.0)))
test.add_test(atan2_test((math.pi*2.0, math.pi/2.0, 0.0), (-math.pi*2.0, -math.pi/2.0, 0.0)))
test.add_test(atan2_test((math.pi*2.0, math.pi/2.0, 1.0), (1.0, 1.0, 1.0)))
test.add_test(atan2_test((1.0, 1.0, 1.0), (-math.pi*2.0, -math.pi/2.0, 0.0)))
test.add_test(atan2_test((3.0,), (9.0,)))
test.add_test(atan2_test((-0.5, -1.0, -3.0, -4.0), (1.0, 1.0, 1.0, 1.0)))
test.add_test(atan2_test((-0.5, -1.0, -3.0, -4.0), (-1.0, -1.0, -1.0, -1.0)))
test.add_test(atan2_test((0.5, 1.5, 2.5), (1.0, 1.0, 1.0)))
test.add_test(atan2_test((1.0, 1.0, 1.0), (0.5, 1.5, 2.5)))

test.output(sys.stdout)
