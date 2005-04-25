#!/usr/bin/python

import shtest, sys, common
from common import *

def neg(p, types=[]):
    if is_array(p):
        result = [-a for a in p]
    else:
        result = [-p]
    return shtest.make_test(result, [p], types)

def insert_into(test):
    test.add_test(neg((0.0, 0.0, 0.0)))
    test.add_test(neg((1.0, 2.0, 3.0)))
    test.add_test(neg((-1.0, -2.0, -3.0)))
    test.add_test(neg((0.5, -1.0, -2.0, -3.0)))
    test.add_test(neg((5000)))
    test.add_test(neg((-0.3333)))

test = shtest.StreamTest('neg', 1)
test.add_call(shtest.Call(shtest.Call.prefix, '-', 1))
insert_into(test)
test.output(sys.stdout)

