#!/usr/bin/python

import shtest, sys, math 

def acos(l, types=[]):
    return shtest.make_test([math.acos(x) for x in l], [l], types)

test = shtest.StreamTest('acos', 1)
test.add_call(shtest.Call(shtest.Call.call, 'acos', 1))
test.add_test(acos((0.0,)))
test.add_test(acos((0.3, 0.5, 0.8, 0.9)))
test.add_test(acos((1 - 0.1, -1 + 0.1, 0.0)))
test.add_test(acos((-0.5, -0.9, -0.1)))
test.add_test(acos((0.5, 0.6, 0.9)))

test.output(sys.stdout)
