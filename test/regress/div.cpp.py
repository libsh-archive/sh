#!/usr/bin/python

import shtest, sys, common

def div(p, q, types=[]):
    result = [a / b for (a,b) in common.upzip(p, q)]
    return shtest.make_test(result, [p, q], types)

test = shtest.StreamTest('div', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/', 2))
test.add_test(div((0.0, 1.0, 2.0), (3.0, 4.0, 5.0)))
test.add_test(div((1.0,), (3.0, 4.0, 5.0)))
test.add_test(div((1.0, 2.0, 3.0), (7.0,)))
test.add_test(div((1.0,), (7.0,)))
test.add_test(div((4.0, 5.0, 6.0), (-4.0, -5.0, -6.0)))

test.add_test(div((1, 2, 3), (3, 3, 3), ['i', 's', 's']))
test.add_test(div((0, 1, 2, 3), (4, 4, 4, 4), ['i', 'i', 'i']))
test.add_test(div((4, 5, 6, 7), (4, 4, 4, 4), ['i', 'i', 'i']))
test.add_test(div((8, 9, 10, 11), (4, 4, 4, 4), ['i', 'i', 'i']))
test.add_test(div((1.0, 2.0, 3.0), (3, 3, 3), ['f', 'f', 's']))
#test.add_test(div((1, 2, 3), (3.0, 3.0, 3.0), ['f', 's', 'f']))

test.output(sys.stdout)
