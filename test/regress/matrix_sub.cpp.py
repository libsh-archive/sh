#!/usr/bin/python

import shtest, sys, matrix_sub_test
from matrix_sub_test import *

test = shtest.ImmediateTest('sub', 2)
test.add_call(shtest.Call(shtest.Call.infix, '-', 2))
insert_into(test)
test.output(sys.stdout)
