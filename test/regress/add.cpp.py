#!/usr/bin/python

import shtest, sys, add_test

test = shtest.StreamTest('add', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+', 2))
add_test.insert_into(test)
test.output(sys.stdout)
