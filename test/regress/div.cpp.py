#!/usr/bin/python

import shtest, sys, div_test

test = shtest.StreamTest('div', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/', 2))
div_test.insert_into(test)
test.output(sys.stdout)
