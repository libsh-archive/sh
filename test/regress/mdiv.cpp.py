#!/usr/bin/python

import shtest, sys, div_test

test = shtest.StreamTest('mdiv', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/=', 2))
div_test.insert_into(test, 1)
test.output(sys.stdout)
