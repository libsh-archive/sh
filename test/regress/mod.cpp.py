#!/usr/bin/python

import shtest, sys, mod_test
from mod_test import *

test = shtest.StreamTest('mod', 2)
test.add_call(shtest.Call(shtest.Call.infix, '%', 2))
mod_test.insert_into(test)
test.output(sys.stdout)
