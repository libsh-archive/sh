#!/usr/bin/python

import shtest, sys, common, lit_test

test = shtest.ImmediateTest('lit', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lit', 3))
lit_test.addtests(test)

test.output(sys.stdout)
