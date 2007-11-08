#!/usr/bin/python

import sys
from PIL import Image

input = Image.open(sys.argv[1]) 

input = input.crop((2, 2, input.size[0] - 4, input.size[1] - 4)) 
total = 0 
count = 0
for x in range(0, input.size[0]):
  for y in range(0, input.size[1]):
      total += input.getpixel((x, y))[0]
      count += 1

print "%s,%f" % (sys.argv[1], float(total) / count)
    
