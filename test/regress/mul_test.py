#!/usr/bin/python

import shtest, common
from common import *

def mul(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a * b for (a,b) in upzip(p, q)]
    elif is_array(q):
        result = [p * b for b in q]
    else:
        result = [a * q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=0):
    test.add_test(mul((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
    test.add_test(mul((1.0, 2.0, 3.0), (7.0,)))
    test.add_test(mul((1.0,), (7.0,)))
    test.add_test(mul((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

    # 1 * N
    if not modifying:
        test.add_test(mul((1.0,), (3.0, 4.0, 5.0)))
        test.add_test(mul(0.5, (10.0, 5679.0, -0.5)))
        test.add_test(mul(2.3, (10.0, 5679.0, -0.5)))
        test.add_test(mul(0.0, (10.0, 5679.0, -0.5)))

    # N * 1
    test.add_test(mul((10.0, 5679.0, -0.5), -0.1))
    test.add_test(mul((10.0, 5679.0, -0.5), -1.4))
