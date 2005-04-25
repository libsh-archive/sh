#!/usr/bin/python

import shtest, sys, math 

def tan(l, types=[]):
    return shtest.make_test([math.tan(x) for x in l], [l], types)

test = shtest.StreamTest('tan', 1)
test.add_call(shtest.Call(shtest.Call.call, 'tan', 1))
test.add_test(tan((0.0, 1.0, 2.0, 3.0)))
test.add_test(tan((0.3, 0.5, 0.8, 0.9)))
test.add_test(tan((math.pi/2 - 0.1, -math.pi/2 + 0.1, 0.0)))
test.add_test(tan((-0.5, -1.0, -1.1)))
test.add_test(tan((0.5, 1.5, 2.5)))

test.output(sys.stdout)
