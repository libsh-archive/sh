#!/usr/bin/python
import string, sys, datetime, math, operator

tests = [
    ['ADD', ['v', 'v'], 'v', ['A + B'],
     [[[-3.4, 4.5, 9], [3.7, -4.9, 12], [0.3, -0.4, 21]]]],
    
    ['MUL', ['sv', 'sv'], 'v', ['A * B'],
     [[[1], [1], [1]],
      [[1, 2, 3], [1], [1, 2, 3]],
      [[1], [1, 2, 3], [1, 2, 3]],
      [[1, 2, 3], [4, 5, 6], [4, 10, 18]],
      [[-1, 2, -3], [4, -5, -6], [-4, -10, 18]],
      [[1, 2, 3], [0, 0, 0], [0, 0, 0]]]
      ],
    
    ['DIV', ['v', 'sv'], 'v', ['A / B'], []],

    ['SLT', ['v', 'sv'], 'v', ['A < B'], []],
    ['SLE', ['v', 'sv'], 'v', ['A <= B'], []],
    ['SGT', ['v', 'sv'], 'v', ['A > B'], []],
    ['SGE', ['v', 'sv'], 'v', ['A >= B'], []],
    ['SEQ', ['v', 'sv'], 'v', ['A == B'], []],
    ['SNE', ['v', 'sv'], 'v', ['A != B'], []],

    ['ABS', ['v'], 'v', ['abs(A)'], []],
    ['ACOS', ['v'], 'v', ['acos(A)'], []],
    ['ASIN', ['v'], 'v', ['asin(A)'], []],
    ['ATAN', ['v'], 'v', ['atan(A)'], []],
    ['ATAN2', ['v', 'v'], 'v', ['atan2(A, B)'], []],
    ['CEIL', ['v'], 'v', ['ceil(A)'], []],
    ['COS', ['v'], 'v', ['cos(A)'],
     [[l, [math.cos(x) for x in l]]
       for l in [[0.0, 1.0, 2.0],
                 [0.5, 0.8, 0.9],
                 [math.pi, math.pi/2.0, math.pi*2.0],
                 [-math.pi, -math.pi/2.0, -math.pi*2.0],
                 [3.0, 4.0, 5.0],
                 [-1.0, -3.0, -4.0],
                 [0.5, 1.5, 2.5]]]
     ],
    
    ['DOT', ['v', 'v'], '1', ['dot(A, B)', '(A | B)'],
     [[[1.0, 1.0, 1.0], [0.0, 0.0, 0.0], [0.0]],
      [[1.0, 1.0, 1.0], [1.0, 0.0, -1.0], [0.0]],
      [[1.0, 1.0, 1.0], [1.0, 0.0, -1.0], [0.0]],
      [[1.0, 1.0, 1.0], [1.0, 1.0, 1.0], [3.0]],
      [[-1.0, -1.0, -1.0], [1.0, 1.0, 1.0], [-3.0]],
      [[0.0, 0.4, 0.8], [0.8, 1.0, 1.2], [0.4 + 0.8*1.2]]
      ]
     ],
    
    ['EXP', ['v'], 'v', ['exp(A)'],
     [[l, [math.exp(x) for x in l]]
       for l in [[0.0, 1.0, 2.0],
                 [3.0, 4.0, 5.0],
                 [-1.0, -3.0, -4.0],
                 [0.5, 1.5, 2.5]]]
     ],
    ['EXP2', ['v'], 'v', ['exp2(A)'],
     [[l, [math.pow(2.0,x) for x in l]]
       for l in [[0.0, 1.0, 2.0],
                 [3.0, 4.0, 5.0],
                 [-1.0, -3.0, -4.0],
                 [0.5, 1.5, 2.5]]]
     ],
    ['EXP10', ['v'], 'v', ['exp10(A)'],
     [[l, [math.pow(10.0,x) for x in l]]
       for l in [[0.0, 1.0, 2.0],
                 [3.0, 4.0, 5.0],
                 [-1.0, -3.0, -4.0],
                 [0.5, 1.5, 2.5]]]
     ],
    ['FLR', ['v'], 'v', ['floor(A)'], []],
    ['FRAC', ['v'], 'v', ['frac(A)'], []],
    ['LRP', ['vs', 'v', 'v'], 'v', ['lerp(A, B, C)'], []],
    ['MAD', ['vs', 'v', 'v'], 'v', ['mad(A, B, C)'], []], # should really be vs vs v... hrm
    ['MAX', ['v', 'v'], 'v', ['max(A, B)'], []],
    ['MIN', ['v', 'v'], 'v', ['min(A, B)'], []],
    ['MOD', ['v', 'v'], 'v', ['mod(A, B)'],
     [[a, b, [math.fmod(x,y) for (x, y) in zip(a, b)]]
       for (a, b) in [([1.0, 1.2, 1.6], [1.0, 1.0, 1.0]),
                      ([0.0, 0.2, 0.6], [1.0, 1.0, 1.0]),
                      ([-1.0, -1.2, -1.6], [1.0, 1.0, 1.0]),
                      ([4.2, 6.7, 12.33], [2.2, 4.3, 5.7]),
                      ]]
     ],
    ['LOG', ['v'], 'v', ['log(A)'],
     [[l, [math.log(x) for x in l]]
       for l in [[0.1, 0.3, 0.7],
                 [1.0, 2.0, 3.0],
                 [4.0, 5.0, 12.0],
                 [1.4, 2.7, 3.9]]]
     ],
    ['LOG2', ['v'], 'v', ['log2(A)'], 
     [[l, [math.log(x, 2.0) for x in l]]
       for l in [[0.1, 0.3, 0.7],
                 [1.0, 2.0, 3.0],
                 [4.0, 5.0, 12.0],
                 [1.4, 2.7, 3.9]]]
    ],
    ['LOG10', ['v'], 'v', ['log10(A)'],
     [[l, [math.log10(x) for x in l]]
       for l in [[0.1, 0.3, 0.7],
                 [1.0, 2.0, 3.0],
                 [4.0, 5.0, 12.0],
                 [1.4, 2.7, 3.9]]]
     ],
    
    ['POW', ['v', 'sv'], 'v', ['pow(A, B)'],
     [[[1.0, 1.0, 1.0], [1.0], [1.0, 1.0, 1.0]],
      [[1.0, 1.0, 1.0], [1.0, 1.0, 1.0], [1.0, 1.0, 1.0]]]
     ],

    ['SIN', ['v'], 'v', ['sin(A)'], []],
    ['SQRT', ['v'], 'v', ['sqrt(A)'], []],

    ['TAN', ['v'], 'v', [],
     [[l, [math.tan(x) for x in l]]
       for l in [[0.0, 1.0, 2.0],
                 [0.5, 0.8, 0.9],
                 [math.pi, math.pi/2.0, math.pi*2.0],
                 [-math.pi, -math.pi/2.0, -math.pi*2.0],
                 [3.0, 4.0, 5.0],
                 [-1.0, -3.0, -4.0],
                 [0.5, 1.5, 2.5]]]
     ],

    ['NORM', ['v'], 'v', ['normalize(A)'],
     [[l, [x / math.sqrt(reduce(lambda a, b: a+b, [x*x for x in l])) for x in l]]
      for l in [[1.0, 1.0, 1.0],
                [-1.0, -1.0, -1.0],
                [1.0, 0.0, 0.0],
                [-1.0, 0.0, 0.0],
                [-1.0, 0.0, 1.0],
                [0.5, 0.5, 0.5],
                [10.0, 5.0, 1.0]]]
     ],
    
    ['XPD', ['3', '3'], '3', ['cross(A, B)', 'A ^ B'], []],

    # TEX
    # TEXI
     
    ['COND', ['vs', 'v', 'v'], 'v', ['cond(A, B, C)'], []]

    # KIL
    # OPTBRA
    # FETCH

    ] # tests

# Thanks to
# 2002 by Erwin S. Andreasen -- http://www.andreasen.org/misc.shtml
# for the cartesian product function.
# This function is in the public domain.
def cartesianProduct(sequence):
    "Generate a combination of each elements the sequence"
    if not sequence:
        yield []
    else:
        first, rest = sequence[0], sequence[1:]
        for x in first:
            for y in cartesianProduct(rest):
                yield [x] + y

def varsize(s):
    if s == 'v': return '3'
    if s == 's': return '1'
    return s

for test in tests:
    filename = test[0] + '.cpp'
    print test[0]
    f = open(filename, 'w')
    f.write('// Test cases for ' + test[0] + '\n' +
            '// Generated by ' + sys.argv[0] + ' at ' + str(datetime.datetime.today()) + '\n' +
            '\n')
    f.write('''#include <sh.hpp>
#include "test.hpp"

using namespace SH;

int main(int argc, char** argv) {
  Test test(argc, argv);
''')
    terms = test[1]
    ops = test[3]
    ops = map(lambda x: 'out = ' + x, ops)
    ops.append('sh' + test[0] + '(out, ' +
               ', '.join([string.ascii_uppercase[i] for i in range(len(terms))]) + ')')
    cases = test[4]
    for combination in cartesianProduct(terms):
        for oi, op in enumerate(ops):
            prgname = test[0] + ''.join(combination) + str(oi);
            f.write('  ShProgram ' + prgname + ' = SH_BEGIN_PROGRAM("gpu:stream") {\n')
            max = 1
            for i, size in enumerate(combination):
                f.write('    ShAttrib<' + varsize(size) + ', SH_INPUT> ' + string.ascii_uppercase[i]
                    + ';\n')
                nsize = int(varsize(size))
                if nsize > max: max = nsize
            outsize = test[2]
            if outsize == 'v': outsize = str(max)
            f.write('    ShAttrib<' + outsize + ', SH_OUTPUT> out;\n')
            f.write('    ' + op + ';\n')
            f.write('  } SH_END;\n')
            f.write('  ' + prgname + '.name("' + op + ' [' + ' x '.join(combination) + ']");\n')
            f.write('\n')
            for case in cases:
                if len(case) - 1 != len(combination): continue
                if not reduce(operator.__and__,
                              [len(x[0]) == int(varsize(x[1]))
                               for x in zip(case[:-1], combination)], 1):
                    continue
                f.write('  test.run(' + prgname + ', ')
                f.write(', '.join(['ShAttrib' + str(len(x)) + 'f(' + ', '.join([str(y) for y in x]) + ')' for x in case]) + ');\n')
            f.write('\n')
    f.write('''}
''')


