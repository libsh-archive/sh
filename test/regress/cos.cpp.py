#!/usr/bin/python

import streamtest, sys, common, math

def cos(l):
    return ([math.cos(x) for x in l], [l])

test = streamtest.StreamTest('cos', 1)
test.add_call(streamtest.Call(streamtest.Call.call, 'cos', 1))
test.add_test(cos((0.0, 1.0, 2.0, 3.0)))
test.add_test(cos((0.3, 0.5, 0.8, 0.9)))
test.add_test(cos((math.pi, -math.pi)))
test.add_test(cos((math.pi*2.0, math.pi/2.0)))
test.add_test(cos((-math.pi*2.0, -math.pi/2.0)))
test.add_test(cos((3.0,)))
test.add_test(cos((-0.5, -1.0, -3.0, -4.0)))
test.add_test(cos((0.5, 1.5, 2.5)))

test.output(sys.stdout)
