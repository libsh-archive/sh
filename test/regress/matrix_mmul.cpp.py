#!/usr/bin/python

# Test the modifying matrix multiplication

import shtest, sys, matrix_mul_test
from matrix_mul_test import *

test = shtest.ImmediateTest('mmul', 2)
test.add_call(shtest.Call(shtest.Call.infix, '*=', 2))
insert_into(test, True)
test.output(sys.stdout, False) # Could set to True if scalar-matrix
                               # and matrix-scalar supported real
                               # scalars instead of ShGeneric<1, T>
