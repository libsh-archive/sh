#!/usr/bin/python

import shtest, sys, mul_test

test = shtest.StreamTest('mul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*', 2))
mul_test.insert_into(test)
test.output(sys.stdout)
