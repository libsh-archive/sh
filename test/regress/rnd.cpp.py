#!/usr/bin/python

import shtest, sys, math

def rnd(l, types=[]):
    return shtest.make_test([round(x) for x in l], [l], types)

test = shtest.StreamTest('rnd', 1)
test.add_call(shtest.Call(shtest.Call.call, 'round', 1))
test.add_test(rnd((0.0, 0.1, 0.5, 0.8)))
test.add_test(rnd((1001.5, 1000.5, 1.67)))
#test.add_test(rnd((-1001.5, -1000.5, -0.5))) # Python's round() and Sh's RND are defined differently
test.add_test(rnd((-1.51, -1.49, -1.1, -1.9)))
test.add_test(rnd((-0.0, -0.1, -0.6, -0.25)))
test.add_test(rnd((1.0,)))

test.output(sys.stdout)
