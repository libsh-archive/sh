#!/usr/bin/python

import shtest, sys, mod_test
from mod_test import *

test = shtest.StreamTest('mmod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%=', 2))
mod_test.insert_into(test, 1)
test.output(sys.stdout)
