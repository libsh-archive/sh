#!/usr/bin/python

import shtest, sys, common

def lit(a, b, c, types=[]):
    third = 0
    if a[0] > 0 and b[0] < 0 and int(c[0]) == c[0]:
        third = pow(b[0], c[0])
    result = [1, max(a[0],0), third, 1]
    return shtest.make_test(result, [a, b, c], types)

test = shtest.StreamTest('lit', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lit', 3))

test.add_test(lit((1.0,),(-3.0,),(0.5,)))
test.add_test(lit((1.0,),(-3.0,),(2.5,)))
test.add_test(lit((1.0,),(-3.0,),(0.0,)))

test.add_test(lit((1.0,),(2.0,),(3.0,)))
test.add_test(lit((-1.0,),(-2.0,),(-3.0,)))
test.add_test(lit((1.0,),(-2.0,),(3.0,)))
test.add_test(lit((-1.0,),(2.0,),(-3.0,)))

test.add_test(lit((0.5,),(1.5,),(5.5,)))
test.add_test(lit((-0.5,),(-1.5,),(-5.5,)))
test.add_test(lit((0.5,),(-1.5,),(5.5,)))
test.add_test(lit((-0.5,),(1.5,),(-5.5,)))

test.add_test(lit((1.0,),(1.0,),(1.0,)))
test.add_test(lit((-1.0,),(-1.0,),(-1.0,)))
test.add_test(lit((1.0,),(1.0,),(0.0,)))
test.add_test(lit((1.0,),(0.0,),(0.0,)))
test.add_test(lit((0.0,),(0.0,),(0.0,)))
test.add_test(lit((0.0,),(0.0,),(1.0,)))
test.add_test(lit((0.0,),(1.0,),(1.0,)))
test.add_test(lit((0.0,),(1.0,),(0.0,)))

test.output(sys.stdout)
