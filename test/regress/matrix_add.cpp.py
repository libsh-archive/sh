#!/usr/bin/python

import shtest, sys, matrix_add_test
from matrix_add_test import *

test = shtest.ImmediateTest('add', 2)
test.add_call(shtest.Call(shtest.Call.infix, '+', 2))
insert_into(test)
test.output(sys.stdout)

