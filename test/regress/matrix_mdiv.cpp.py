#!/usr/bin/python

# Test the modifying division of matrices

import shtest, sys, matrix_div_test
from matrix_div_test import *

test = shtest.ImmediateTest('mdiv', 2)
test.add_call(shtest.Call(shtest.Call.infix, '/=', 2))
insert_into(test)
test.output(sys.stdout)
