#!/usr/bin/python

import shtest, sys, math

def rnd(l, types=[]):
    return shtest.make_test([round(x) for x in l], [l], types)

test = shtest.StreamTest('rnd', 1)
test.add_call(shtest.Call(shtest.Call.call, 'round', 1))
test.add_test(rnd((0.0, 0.1, 0.5, 0.8)))
test.add_test(rnd((1.0,)))

test.output(sys.stdout)
