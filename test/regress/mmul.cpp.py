#!/usr/bin/python

import shtest, sys, mul_test

test = shtest.StreamTest('mmul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*=', 2))
mul_test.insert_into(test, 1)
test.output(sys.stdout)
