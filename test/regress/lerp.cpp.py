#!/usr/bin/python

from common import *

import shtest, sys, common

def lerp(p, q, r, types=[]):
    result = [a * b + (1 - a) * c for (a,b,c) in upzip3(p, q, r)]
    return shtest.make_test(result, [p, q, r], types)

test = shtest.StreamTest('lerp', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lerp', 3))

test.add_test(lerp((0.0, 0.0, 0.0), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))
test.add_test(lerp((0.5, 0.5, 0.5), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))
test.add_test(lerp((1.0, 1.0, 1.0), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))

test.add_test(lerp((0.1, 0.3, 0.9), (4.0, -2.0, 10.0), (10.0, 0.5, 5.23)))
test.add_test(lerp((0.1, 0.3, 0.9), (40.0, -20.0, 100.0), (10.0, 0.5, 5.23)))
test.add_test(lerp((0.1, 0.3, 0.9), (400.0, -200.0, 1000.0), (10.0, 0.5, 5.23)))

test.add_test(lerp((0.1,), (40.0,), (10.0,)))
test.add_test(lerp((0.5, 0.85, 0.1, 1.0), (5.0, 1.0, 0.4, 30.0), (6.2, 2.0, 600.0, 12.3333)))

test.output(sys.stdout)
