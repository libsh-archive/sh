#!/usr/bin/python

import streamtest, sys, common

def mul(p, q):
    result = [a * b for (a,b) in common.upzip(p, q)]
    return (result, [p, q])

test = streamtest.StreamTest('mul', 2)
test.add_call(streamtest.Call(streamtest.Call.infix, '*', 2))
test.add_test(mul((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
test.add_test(mul((1.0,), (3.0, 4.0, 5.0)))
test.add_test(mul((1.0, 2.0, 3.0), (7.0,)))
test.add_test(mul((1.0,), (7.0,)))
test.add_test(mul((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))
test.output(sys.stdout)
