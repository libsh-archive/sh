#!/usr/bin/python

import shtest, sys, lerp_test
from lerp_test import *

test = shtest.ImmediateTest('lerp', 3)
test.add_call(shtest.Call(shtest.Call.call, 'lerp', 3))
insert_into(test)
test.output(sys.stdout)
