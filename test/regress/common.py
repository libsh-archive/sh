#!/usr/bin/python

import string

def upzip(a, b):
    if len(a) > len(b):
        return zip(a, b + (b[-1],) * (len(a) - len(b)))
    elif len(a) < len(b):
        return zip(a + (a[-1],) * (len(b) - len(a)), b)
    else:
        return zip(a, b)

def upzip3(a, b, c):
    out_a = a
    out_b = b
    out_c = c
    if len(out_a) > len(out_b):
        out_b = out_b + (out_b[-1],) * (len(out_a) - len(out_b))
    elif len(out_a) < len(out_b):
        out_a = out_a + (out_a[-1],) * (len(out_b) - len(out_a))

    if len(out_b) > len(out_c):
        out_c = out_c + (out_c[-1],) * (len(out_b) - len(out_c))
    elif len(out_b) < len(out_c):
        out_b = out_b + (out_b[-1],) * (len(out_c) - len(out_b))
        if len(out_a) > len(out_b):
            out_b = out_b + (out_b[-1],) * (len(out_a) - len(out_b))
        elif len(out_a) < len(out_b):
            out_a = out_a + (out_a[-1],) * (len(out_b) - len(out_a))
    
    return zip(out_a, out_b, out_c)

if __name__ == "__main__":
    print upzip((0,1,2), (3,))
    print upzip((0,), (3,4,5))
    print upzip((0,1,2), (3,4,5))
    print upzip((0,1,2,4,5,6), (3,4,5))
    print upzip((0,1,2), (3,4,5,4,5,6))

    print upzip3((0,1,2), (3,4,5), (6,))
    print upzip3((0,1,), (3,4,5), (6,7,8))
    print upzip3((0,1,), (3,), (6,7))
    print upzip3((0,1,), (3,), (6,7,8))
    print upzip3((0,1,), (3,), (6,7,8,9,10,11))

