#!/usr/bin/python

import string, sys

cpp_type = {'d': 'double',
            'f': 'float',
            'h': 'half',
            'i': 'int',
            's': 'short',
            'b': 'char',
            'ui': 'unsigned int',
            'us': 'unsinged short',
            'ub': 'unsigned byte' }

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

def make_attrib(size, binding_type, storage_type):
    return 'ShAttrib<' + str(size) + ', ' + binding_type + ', ' + storage_type + '>'

# types are a list of dest then src types ('f' used as default) 
def make_test(values, expected, types=[]):
    types = [cpp_type.has_key(x) and cpp_type[x] or x for x in types] 
    types = types + (len(expected) + 1 - len(types)) * ['float']
    return (values, expected, types)

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

    def add_make_test(self, values, expected, types=[]):
        self.add_test(make_test(values, expected, types))

    def add_test(self, test):
        self.tests.append(test)

    def output(self, out):
        out.write('#include "test.hpp"\n\n')
        out.write('int main(int argc, char** argv) {\n')
        out.write('  using namespace SH;\n\n')
        out.write('  Test test(argc, argv);\n\n')
        programs = {}
        for test in self.tests:
            types = test[2]
            src_arg_types = zip(test[1], types[1:])
            argkey = types[0] + '_' + '_'.join([str(len(arg)) + type for arg, type in src_arg_types])
            if not programs.has_key(argkey):
                programs[argkey] = []
                for i, call in enumerate(self.calls):
                    progname = self.name + '_' + string.ascii_lowercase[i] + '_' + argkey
                    programs[argkey].append(progname)
                    out.write('  ShProgram ' + progname + ' = SH_BEGIN_PROGRAM("gpu:stream") {\n')
                    for j, (arg, type) in enumerate(src_arg_types):
                        out.write('    ' + make_attrib(len(arg), 'SH_INPUT', type) 
                                  + ' ' + string.ascii_lowercase[j] + ';\n')
                    out.write('    ' + make_attrib(len(test[0]), 'SH_OUTPUT', types[0]) +  ' out;\n')
                    out.write('    out = ' + str(call) + ';\n')
                    out.write('  } SH_END;\n\n')
                    out.write('  ' + progname + '.name("' + progname + '");')
            for p in programs[argkey]:
                out.write('  test.run(' + p + ', '
                          + ', '.join([make_attrib(len(arg), 'SH_CONST', types[j + 1]) + '('
                                       + ', '.join([str(a) for a in arg]) + ')' for arg, type in src_arg_types])
                          + ', ' + make_attrib(len(test[0]), 'SH_CONST', types[0]) + '('
                                       + ', '.join([str(a) for a in test[0]]) + ')'
                          + ');\n')
            out.write('\n')
        out.write('}\n')

if __name__ == "__main__":
    foo = StreamTest("add", 2)
    foo.add_call(Call(Call.infix, '+', 2))
    foo.add_call(Call(Call.call, 'add', 2))
    foo.add_make_test((3, 5, 7), [(0, 1, 2), (3, 4, 5)])
    foo.add_make_test((4, 5, 6), [(1,), (3, 4, 5)], ['i', 'int'])
    foo.add_make_test((8, 9, 10), [(1, 2, 3), (7,)])
    foo.add_make_test((8,), [(1,), (7,)])
    foo.add_make_test((0, 0, 0), [(4, 5, 6), (-4, -5, -6)])
    foo.output(sys.stdout)
