#!/usr/bin/python

import shtest, sys, common
from common import *

def lerp(p, q, r, types=[]):
    if is_array(p) and is_array(q) and is_array(r):
        result = [a * b + (1 - a) * c for (a,b,c) in upzip3(p, q, r)]
    elif is_array(q) and is_array(r):
        result = [p * b + (1 - p) * c for (b,c) in upzip(q, r)]
    else:
        result = [p * q + (1 - p) * r]
    return shtest.make_test(result, [p, q, r], types)

def insert_into(test):
    test.add_test(lerp((0.0, 0.0, 0.0), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))
    test.add_test(lerp((0.5, 0.5, 0.5), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))
    test.add_test(lerp((1.0, 1.0, 1.0), (0.1, 0.4, 0.8), (1.0, -10.0, 5.0)))

    test.add_test(lerp((0.1, 0.3, 0.9), (4.0, -2.0, 10.0), (10.0, 0.5, 5.23)))
    test.add_test(lerp((0.1, 0.3, 0.9), (40.0, -20.0, 100.0), (10.0, 0.5, 5.23)))
    test.add_test(lerp((0.1, 0.3, 0.9), (400.0, -200.0, 1000.0), (10.0, 0.5, 5.23)))

    test.add_test(lerp(0.5, -3.0, 1.0))
    test.add_test(lerp((0.1,), (40.0,), (10.0,)))
    test.add_test(lerp((0.5, 0.85, 0.1, 1.0), (5.0, 1.0, 0.4, 30.0), (6.2, 2.0, 600.0, 12.3333)))

    # lerp(1, N, N)
    test.add_test(lerp(0.1, (4300.2, -0.2, 1000, 0.4), (10.0, 12.0, 1.0, 0.0)))
    test.add_test(lerp(10.0, (40.0, -2.0, 0.0), (10.0, 12.0, -5.2)))
