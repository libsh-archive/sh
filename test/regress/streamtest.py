#!/usr/bin/python

import string, sys

class Call:
    prefix, postfix, infix, call = range(4)
    def __init__(self, kind, name, arity):
        self.type = kind
        self.name = name
        self.arity = arity
        if (self.type == Call.prefix or self.type == Call.postfix) and arity != 1:
            raise "Invalid arity for prefix/postfix call"
        if (self.type == Call.infix) and arity != 2:
            raise "Invalid arity for infix call"

    def __str__(self):
        r = ''
        args = [string.ascii_lowercase[i] for i in range(self.arity)]
        if self.type == Call.prefix:
            r = self.name + " " + args[0]
        elif self.type == Call.postfix:
            r = args[0] + " " + self.name + " "
        elif self.type == Call.infix:
            r = args[0] + " " + self.name + " " + args[1]
        elif self.type == Call.call:
            r = self.name + '(' + ', '.join(args) + ')'
        return r

class StreamTest:
    def __init__(self, name, arity):
        self.name = name
        self.arity = arity
        self.calls = []
        self.tests = []

    def add_call(self, call):
        if call.arity != self.arity:
            raise "Invalid arity in call"
        self.calls.append(call)

    def add_test(self, values, expected):
        self.tests.append((expected, values))

    def add_test(self, test):
        self.tests.append(test)

    def output(self, out):
        out.write('#include "test.hpp"\n\n')
        out.write('int main(int argc, char** argv) {\n')
        out.write('  using namespace SH;\n\n')
        out.write('  Test test(argc, argv);\n\n')
        programs = {}
        for test in self.tests:
            arglengths = '_'.join([str(len(arg)) for arg in test[1]])
            if not programs.has_key(arglengths):
                programs[arglengths] = []
                for i, call in enumerate(self.calls):
                    progname = self.name + '_' + string.ascii_lowercase[i] + '_' + arglengths
                    programs[arglengths].append(progname)
                    out.write('  ShProgram ' + progname + ' = SH_BEGIN_PROGRAM("gpu:stream") {\n')
                    for j, arg in enumerate(test[1]):
                        out.write('    ShAttrib<' + str(len(arg)) + ', SH_INPUT> '
                                  + string.ascii_lowercase[j] + ';\n')
                    out.write('    ShAttrib<' + str(len(test[0])) + ', SH_OUTPUT> out;\n')
                    out.write('    out = ' + str(call) + ';\n')
                    out.write('  } SH_END;\n\n')
            for p in programs[arglengths]:
                out.write('  test.run(' + p + ', '
                          + ', '.join(['ShAttrib<' + str(len(arg)) + ', SH_CONST>('
                                       + ', '.join([str(a) for a in arg]) + ')' for arg in test[1]])
                          + ', ShAttrib<' + str(len(test[0])) + ', SH_CONST>('
                                       + ', '.join([str(a) for a in test[0]]) + ')'
                          + ');\n')
            out.write('\n')
        out.write('}\n')

if __name__ == "__main__":
    foo = StreamTest("add", 2)
    foo.add_call(Call(Call.infix, '+', 2))
    foo.add_call(Call(Call.call, 'add', 2))
    foo.add_test([(0, 1, 2), (3, 4, 5)], (3, 5, 7))
    foo.add_test([(1,), (3, 4, 5)], (4, 5, 6))
    foo.add_test([(1, 2, 3), (7,)], (8, 9, 10))
    foo.add_test([(1,), (7,)], (8,))
    foo.add_test([(4, 5, 6), (-4, -5, -6)], (0, 0, 0))
    foo.output(sys.stdout)
