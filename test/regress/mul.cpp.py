#!/usr/bin/python

import shtest, sys
from common import *

def mul(p, q, types=[]):
    result = [a * b for (a,b) in upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('mul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*', 2))
test.add_test(mul((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
test.add_test(mul((1.0,), (3.0, 4.0, 5.0)))
test.add_test(mul((1.0, 2.0, 3.0), (7.0,)))
test.add_test(mul((1.0,), (7.0,)))
test.add_test(mul((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))
test.output(sys.stdout)
