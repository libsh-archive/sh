#!/usr/bin/python

import shtest, sys, math

def cos(l, types=[]):
    return shtest.make_test([math.cos(x) for x in l], [l], types)

test = shtest.StreamTest('cos', 1)
test.add_call(shtest.Call(shtest.Call.call, 'cos', 1))
test.add_test(cos((0.0, 1.0, 2.0, 3.0)))
test.add_test(cos((0.3, 0.5, 0.8, 0.9)))
test.add_test(cos((math.pi, -math.pi, 0.0)))
test.add_test(cos((math.pi*2.0, math.pi/2.0, 0.0)))
test.add_test(cos((-math.pi*2.0, -math.pi/2.0, 0.0)))
test.add_test(cos((3.0,)))
test.add_test(cos((-0.5, -1.0, -3.0, -4.0)))
test.add_test(cos((0.5, 1.5, 2.5)))

test.output(sys.stdout)
