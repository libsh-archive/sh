#!/usr/bin/python

import shtest, sys, common

def lit(a, b, c, types=[]):
    x = max(a[0], 0)
    y = max(b[0], 0)
    w = min(max(c[0], -128.0), 128.0)
    third = 0
    if x > 0:
        third = pow(y, w)
    result = [1, x, third, 1]
    return shtest.make_test(result, [a, b, c], types)

test = shtest.StreamTest('lit', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lit', 3))

test.add_test(lit((0.75,),(0.5,),(10.0,)))
test.add_test(lit((0.5,),(0.5,),(50.0,)))
test.add_test(lit((0.5,),(0.75,),(100.0,)))
test.add_test(lit((0.5,),(0.75,),(100.0,)))
test.add_test(lit((0.01,),(0.01,),(30.0,)))
test.add_test(lit((0.99,),(0.99,),(5.0,)))

test.add_test(lit((1.0,),(-3.0,),(0.5,)))
test.add_test(lit((1.0,),(-3.0,),(2.5,)))
test.add_test(lit((0.5,),(-1.5,),(5.5,)))

test.add_test(lit((1.0,),(1.0,),(1.0,)))
test.add_test(lit((-1.0,),(-1.0,),(-1.0,)))
test.add_test(lit((2.0,),(1.0,),(0.0,)))
test.add_test(lit((2.0,),(0.0,),(0.0,)))
test.add_test(lit((2.0,),(0.5,),(0.0,)))
test.add_test(lit((0.0,),(0.0,),(1.0,)))
test.add_test(lit((0.0,),(0.0,),(2.0,)))
test.add_test(lit((0.0,),(1.0,),(1.0,)))
test.add_test(lit((0.0,),(1.0,),(2.0,)))

test.add_test(lit((1.0,),(1.0,),(1.0,)))
test.add_test(lit((2.0,),(1.0,),(1.0,)))
test.add_test(lit((1.0,),(0.0,),(1.0,)))
test.add_test(lit((2.0,),(0.0,),(1.0,)))
test.add_test(lit((2.0,),(0.5,),(1.0,)))
test.add_test(lit((0.0,),(0.0,),(1.0,)))
test.add_test(lit((0.0,),(1.0,),(1.0,)))

test.add_test(lit((0.0,),(1.0,),(-128,)))
test.add_test(lit((0.0,),(1.0,),(-130,)))
test.add_test(lit((0.0,),(1.0,),(-5000,)))
test.add_test(lit((0.0,),(1.0,),(128,)))
test.add_test(lit((0.0,),(1.0,),(130,)))
test.add_test(lit((0.0,),(1.0,),(5000,)))

test.add_test(lit((0.0,),(0.5,),(-128,)))
test.add_test(lit((0.0,),(0.5,),(-130,)))
test.add_test(lit((0.0,),(0.5,),(-5000,)))
test.add_test(lit((0.0,),(0.5,),(128,)))
test.add_test(lit((0.0,),(0.5,),(130,)))
test.add_test(lit((0.0,),(0.5,),(5000,)))

test.add_test(lit((0.0,),(0.01,),(-128,)))
test.add_test(lit((0.0,),(0.01,),(-130,)))
test.add_test(lit((0.0,),(0.01,),(-5000,)))
test.add_test(lit((0.0,),(0.01,),(128,)))
test.add_test(lit((0.0,),(0.01,),(130,)))
test.add_test(lit((0.0,),(0.01,),(5000,)))

test.output(sys.stdout)
