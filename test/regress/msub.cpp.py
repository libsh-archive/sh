#!/usr/bin/python

import shtest, sys, sub_test

test = shtest.StreamTest('msub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-=', 2))
sub_test.insert_into(test, 1)
test.output(sys.stdout)
