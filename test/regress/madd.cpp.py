#!/usr/bin/python

import shtest, sys, common

def madd(p, q, types=[]):
    result = [a + b for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('madd', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+=', 2))

test.add_test(madd((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
test.add_test(madd((1.0, 2.0, 3.0), (7.0,)))
test.add_test(madd((1.0,), (7.0,)))
test.add_test(madd((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

test.output(sys.stdout)
