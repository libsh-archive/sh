#!/usr/bin/python

import shtest, sys, common

def dec(p, types=[]):
    result = [a - 1 for (a) in (p)]
    return shtest.make_test(result, [p], types)

test = shtest.StreamTest('dec', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '--', 1))

test.add_test(dec((-1.5, 0.0, 0.5, 3.0)))

test.output(sys.stdout)
