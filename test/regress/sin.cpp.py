#!/usr/bin/python

import shtest, sys, math 

def sin(l, types=[]):
    return shtest.make_test([math.sin(x) for x in l], [l], types)

test = shtest.StreamTest('sin', 1)
test.add_call(shtest.Call(shtest.Call.call, 'sin', 1))
test.add_test(sin((0.0, 1.0, 2.0, 3.0)))
test.add_test(sin((0.3, 0.5, 0.8, 0.9)))
test.add_test(sin((math.pi, -math.pi)))
test.add_test(sin((math.pi*2.0, math.pi/2.0)))
test.add_test(sin((-math.pi*2.0, -math.pi/2.0)))
test.add_test(sin((3.0,)))
test.add_test(sin((-0.5, -1.0, -3.0, -4.0)))
test.add_test(sin((0.5, 1.5, 2.5)))

test.output(sys.stdout)
