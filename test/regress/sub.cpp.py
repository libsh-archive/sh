#!/usr/bin/python

import shtest, sys, sub_test

test = shtest.StreamTest('sub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-', 2))
sub_test.insert_into(test)
test.output(sys.stdout)
