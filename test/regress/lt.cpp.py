#!/usr/bin/python

# Tests the "less than" operator

import shtest, sys, common
from common import *

def lt(p, q, types=[]):
    if is_array(p) and is_array(q):
        result = [(a < b) + 0 for (a,b) in common.upzip(p, q)]
    elif is_array(q):
        result = [(p < b) + 0 for b in q]
    else:
        result = [(a < q) + 0 for a in p]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('lt', 2)
test.add_call(shtest.Call(shtest.Call.infix, '<', 2))

test.add_test(lt((1.0,), (2.0,)))
test.add_test(lt((1.0,), (1.0,)))
test.add_test(lt((3.0,), (1.0,)))

test.add_test(lt((1.0, 2.0, 3.0), (2.0, 1.0, 3.0)))
test.add_test(lt((-1.0, -2.0, -3.0), (-2.0, -1.0, -3.0)))
test.add_test(lt((1.0, -2.0, 3.0), (-2.0, 1.0, -3.0)))

test.add_test(lt((0.0, 1.0, -0.5, 0.1), (0.0, 0.0, 1.0, -1.0)))

test.add_test(lt(0.0, (-1.0, 0.0, 1.0)))
test.add_test(lt(2.0, (-1.0, 0.0, 1.0)))
test.add_test(lt((-1.0, 0.0, 1.0), -2.0))
test.add_test(lt((-1.0, 0.0, 1.0), 0.0))

test.output(sys.stdout)
