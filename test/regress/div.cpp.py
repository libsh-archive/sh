#!/usr/bin/python

import streamtest, sys, common

def div(p, q):
    result = [a / b for (a,b) in common.upzip(p, q)]
    return (result, [p, q])

test = streamtest.StreamTest('div', 2)
test.add_call(streamtest.Call(streamtest.Call.infix, '/', 2))
test.add_test(div((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
test.add_test(div((1.0,), (3.0, 4.0, 5.0)))
test.add_test(div((1.0, 2.0, 3.0), (7.0,)))
test.add_test(div((1.0,), (7.0,)))
test.add_test(div((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))
test.output(sys.stdout)
