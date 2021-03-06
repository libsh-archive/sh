#!/usr/bin/python

import string
from math import sqrt

def cross_product(a, b):
    return (a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]);

def dot_product(a, b):
    return sum([x*y for (x,y) in zip(a,b)])

# Return a new matrix based on the original one but expanded to the
# given dimensions.  It can either be expanded to a zero matrix or an
# identity matrix (the default).
def matrix_expand(m, nrows, ncols, identity):
    nrows_old = len(m)
    ncols_old = len(m[0])

    res = [[0] * ncols for i in range(nrows)]
    if identity:
        for i in range(nrows):
            for j in range(ncols):
                if i == j: res[i][j] = 1
                
    for i in range(nrows_old):
        for j in range(ncols_old):
            res[i][j] = m[i][j]

    return res

def length(a):
    s = 0
    for x in a:
        s += x*x
    return [sqrt(s)]

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

def is_array(a):
    return (type(a) == tuple) or (type(a) == list)

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
