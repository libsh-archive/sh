#!/usr/bin/python

import string

def upzip(a, b):
    if len(a) > len(b):
        return zip(a, b + (b[-1],) * (len(a) - len(b)))
    elif len(a) < len(b):
        return zip(a + (a[-1],) * (len(b) - len(a)), b)
    else:
        return zip(a, b)

if __name__ == "__main__":
    print upzip((0,1,2), (3,))
    print upzip((0,), (3,4,5))
    print upzip((0,1,2), (3,4,5))
    print upzip((0,1,2,4,5,6), (3,4,5))
    print upzip((0,1,2), (3,4,5,4,5,6))

