#!/usr/bin/python

import shtest, sys, add_test

test = shtest.ImmediateTest('add - immediate', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+', 2))
test.add_call(shtest.Call(shtest.Call.destcall, 'shADD', 2))
add_test.insert_into(test)
test.output(sys.stdout)
