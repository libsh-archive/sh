#!/usr/bin/python

import shtest, common
from common import *

def sub(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [a - b for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [p - b for b in q]
    else:
        result = [a - q for a in p]
    return shtest.make_test(result, [p, q], types)

def insert_into(test, modifying=0):
    test.add_test(sub((3.0, 4.0, 5.0), (0.0, 1.0, 2.0)))
    test.add_test(sub((3.0, 4.0, 5.0), (1.0,)))
        
    test.add_test(sub((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))
    test.add_test(sub((4.0, 5.0, 6.0), (4.0, 5.0, 6.0)))
    test.add_test(sub((0.50, 2.0, -3.3333), 1000))

    if not modifying:
        test.add_test(sub((7.0,), (1.0, 2.0, 3.0)))
        test.add_test(sub((1.0,), (7.0,)))
        test.add_test(sub(-0.5, (0.50, 2.0, -3.3333)))
    
