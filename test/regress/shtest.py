#!/usr/bin/python

import string, sys

from common import * 

value_type_enum = {'d': 'double',
            'f': 'float',
            'h': 'ShHalf',

            'i': 'int',
            's': 'short',
            'b': 'char',
            'ui': 'unsigned int',
            'us': 'unsigned short',
            'ub': 'unsigned char',

            'fi': 'ShFracInt',
            'fs': 'ShFracShort',
            'fb': 'ShFracByte',
            'fui': 'ShFracUInt',
            'fus': 'ShFracUShort',
            'fub': 'ShFracUByte' 
            }

enum_value_type = dict(zip(value_type_enum.values(), value_type_enum.keys()))

def make_attrib(size, binding_type, value_type):
    return 'ShAttrib<' + str(size) + ', ' + binding_type + ', ' + value_type + '>'

# types are a list of dest then src types ('f' used as default) 
def make_test(expected, values, types=[]):
    types = [value_type_enum.has_key(x) and value_type_enum[x] or x for x in types] 
    types = types + (len(values) + 1 - len(types)) * ['float']
    return (expected, values, types)

# Handles different styles of function calls that assign their value to a
# variable out
# prefix: out = call arg[0]
# postfix: out = arg[0] call 
# infix: out = arg[0] call arg[1]
# lookup: out = call[arg[0]]
# call: out = call(args)
# destcall: call(out, args)
class Call:
    prefix, postfix, infix, lookup, call, destcall = range(6)
    styles = ['prefix', 'postfix', 'infix', 'lookup', 'call', 'destcall']
    def __init__(self, kind, name, arity, dest="out"):
        self.type = kind
        self.name = name
        self.arity = arity
        self.dest = dest
        if (self.type == Call.prefix or self.type == Call.postfix or self.type == Call.lookup) and arity != 1:
            raise "Invalid arity for prefix/postfix/lookup call"
        if (self.type == Call.infix) and arity != 2:
            raise "Invalid arity for infix call"

    def __str__(self):
        r = ''
        args = [string.ascii_lowercase[i] for i in range(self.arity)]
        if self.type == Call.prefix:
            r = 'out = ' + self.name + " " + args[0]
        elif self.type == Call.postfix:
            r = 'out = ' + args[0] + " " + self.name + " "
        elif self.type == Call.infix:
            r = 'out = ' + args[0] + " " + self.name + " " + args[1]
        elif self.type == Call.lookup:
            r = 'out = ' + self.name + '[' + args[0] + ']' 
        elif self.type == Call.call:
            r = 'out = ' + self.name + '(' + ', '.join(args) + ')'
        elif self.type == Call.destcall:
            r = self.name + '(out, ' +  ', '.join(args) + ')'
        return r
    def key(self):
        r = self.styles[self.type]
        if self.type == Call.lookup or self.type == Call.call or self.type == Call.destcall:
          r += '_' + self.name
        return r

# Handles 2D/RECT textures (put the appropriate Sh type in textype)
class ImageTexture:
    def __init__(self, textype, name, filename):
        self.textype = textype
        self.name = name
        self.filename = filename

    def __str__(self):
        # @todo type move this into test.hpp or test.cpp
        r =   "  " + self.textype + " " + self.name + ";\n"; 
        r +=   "  {\n" 
        r +=  "    ShImage image;\n"
        r +=  "    image.loadPng(\"" + self.filename + "\");\n"
        r +=  "    " + self.name + ".size(image.width(), image.height());\n" 
        r +=  "    " + self.name + ".memory(image.memory());\n" 
        r +=  "  }\n"
        return r

# Generates a 2D/3D texture using the provided code snippet
# to fill in the array (by generating host data type values)
class GenTexture:
    def __init__(self, textype, tex_value_type, tex_size, dims, name, code):
        self.textype = textype
        self.tex_value_type = tex_value_type
        self.tex_size = tex_size
        self.dims = dims
        self.name = name
        self.code = code 

    def size(self):
        return "(" + str(self.tex_size) + " * " + " * ".join([str(x) for x in self.dims]) + ")"

    def mem(self): 
        return self.name + "_memdata"

    def __str__(self):
        r =  "  " + self.textype + " " + self.name + "(" + ",".join([str(x) for x in self.dims]) + ");\n"; 
        mem_type = "ShDataVariant<" + self.tex_value_type + ", SH_MEM>";
        r += "  ShPointer<" + mem_type + " > " + self.mem() + " = new " + mem_type + "(" + self.size() + ");\n"  
        r += "  {\n" 
        host_type = "ShDataVariant<" + self.tex_value_type + ", SH_HOST>";
        r += "    ShPointer<" + host_type + " > data = new " + host_type + "(" + self.size() + ");\n"  
        r += "    int index = 0;\n"
        loops = "" # todo should be a better way to reverse dims...
        for i, dim in enumerate(self.dims):
            loopvar = string.ascii_lowercase[i+8]; 
            loops = "    for(int " + loopvar + " = 0; " + loopvar + " < " + str(dim) + "; ++" + loopvar + ")\n" + loops
        r += loops
        r += '    for(int elem = 0; elem < ' + str(self.tex_size) + '; ++elem, ++index)\n' 
        r += "    {\n"
        r += "       (*data)[index] = " + self.code + ";\n" 
        r += "    }\n"
        r += "    " + self.mem() + "->set(data);\n"
        r += "    ShPointer<ShHostMemory> hostmem = new ShHostMemory(" + self.size() + " * " + self.mem() + "->datasize(), " + self.mem() + "->array());\n"
        r += "    " + self.name + ".memory(hostmem);\n"
        r += "  }\n"
        return r

# A Test class that associates a set of call styles with a set of tests cases,

class Test:
    def __init__(self, name, arity):
        self.name = name
        self.arity = arity
        self.calls = []
        self.textures = []
        self.tests = []

    # add a call to the current list
    def add_call(self, call):
        if call.arity != self.arity:
            raise "Invalid arity in call"
        self.calls.append(call)

    # clear current call list
    def clear_call(self):
        self.calls = []

    def add_texture(self, texture):
        self.textures.append(texture)

    # add a test case to use against the current call list
    def add_make_test(self, expected, values, types=[]):
        self.add_test(make_test(expected, values, types))

    # add a test case to use against the current call list
    def add_test(self, test):
        self.tests.append((test, self.calls))

    def output_textures(self, out):
        for texture in self.textures:
            out.write(str(texture) + '\n\n')

    def output_header(self, out):
        out.write('#include "test.hpp"\n\n')
        out.write('int main(int argc, char** argv) {\n')
        out.write('  using namespace SH;\n\n')
        out.write('  try {\n')
        out.write('  Test test(argc, argv);\n\n')
        self.output_textures(out)

    def output_footer(self, out):
        out.write("""  } catch (const ShException &e) {
    std::cout << "Caught Sh Exception." << std::endl << e.message() << std::endl;
  } catch (const std::exception& e) {
      std::cerr << "Caught C++ exception: " << e.what() << std::endl;
  }
}""")
        out.write("\n")

class StreamTest(Test):
    def __init__(self, name, arity):
        Test.__init__(self, name, arity)

    def output(self, out):
        self.output_header(out)
        programs = {}
        for test, testcalls in self.tests:
            types = test[2]
            src_arg_types = zip(test[1], types[1:])
            for i, call in enumerate(testcalls):
                argkey = enum_value_type[types[0]] + '_' + '_'.join([str(len(arg)) + enum_value_type[type] for arg, type in src_arg_types]) + '_' + call.key() 
                if not programs.has_key(argkey):
                    programs[argkey] = []
                    progname = self.name + '_' + string.ascii_lowercase[i] + '_' + argkey
                    programs[argkey].append(progname)
                    out.write('  ShProgram ' + progname + ' = SH_BEGIN_PROGRAM("gpu:stream") {\n')
                    for j, (arg, type) in enumerate(src_arg_types):
                        out.write('    ' + make_attrib(len(arg), 'SH_INPUT', type) 
                                  + ' ' + string.ascii_lowercase[j] + ';\n')
                    out.write('    ' + make_attrib(len(test[0]), 'SH_OUTPUT', types[0]) +  ' out;\n')
                    out.write('    ' + str(call) + ';\n')
                    out.write('  } SH_END;\n\n')
                    out.write('  ' + progname + '.name("' + progname + '");\n')
            for p in programs[argkey]:
                out.write('  test.run(' + p + ', '
                          + ', '.join([make_attrib(len(arg), 'SH_CONST', types[j + 1]) + '('
                                       + ', '.join([str(a) for a in arg]) + ')' for arg, type in src_arg_types])
                          + ', ' + make_attrib(len(test[0]), 'SH_CONST', types[0]) + '('
                                       + ', '.join([str(a) for a in test[0]]) + ')'
                          + ');\n')
            out.write('\n')
        self.output_footer(out)

class ImmediateTest(Test):
    def __init__(self, name, arity):
        Test.__init__(self, name, arity)

    def output(self, out):
        self.output_header(out)
        for test, testcalls in self.tests:
            types = test[2]
            src_arg_types = zip(test[1], types[1:])
            for i, call in enumerate(testcalls):
                testname = enum_value_type[types[0]] + '_' + '_'.join([str(len(arg)) + enum_value_type[type] for arg, type in src_arg_types]) + '_' + call.key()
                out.write('  { // ' + testname + '\n')
                for j, (arg, type) in enumerate(src_arg_types):
                    out.write('    ' + make_attrib(len(arg), 'SH_CONST', type) 
                              + ' ' + string.ascii_lowercase[j] + '(' + ','.join([str(a) for a in arg]) + ');\n')
                out.write('    ' + make_attrib(len(test[0]), 'SH_TEMP', types[0]) +  ' out;\n')
                out.write('    ' + str(call) + ';\n')
                out.write('    test.check("' + testname + '", out, ' +
																			make_attrib(len(test[0]), 'SH_CONST', types[0]) + '('
                                       + ', '.join([str(a) for a in test[0]]) + ')'
                          + ');\n')
                out.write('  }\n\n')
        self.output_footer(out)

if __name__ == "__main__":
    foo = StreamTest("add", 2)
    foo.add_texture(ImageTexture("ShTexture2D<ShColor3f>", "mytex", "mytex.png")) 
    foo.add_texture(GenTexture("ShTexture2D<ShColor3f>", "float", 3, (128, 64), "mytex2", "i / 128.0 + j / 64.0"))
    foo.add_call(Call(Call.infix, '+', 2))
    foo.add_call(Call(Call.call, 'add', 2))
    foo.add_make_test((3, 5, 7), [(0, 1, 2), (3, 4, 5)])
    foo.add_make_test((4, 5, 6), [(1,), (3, 4, 5)], ['i', 'SH_INT'])
    foo.add_make_test((8, 9, 10), [(1, 2, 3), (7,)])
    foo.add_make_test((8,), [(1,), (7,)])
    foo.add_make_test((0, 0, 0), [(4, 5, 6), (-4, -5, -6)])
    foo.output(sys.stdout)

    foo = ImmediateTest("add", 2)
    foo.add_call(Call(Call.infix, '+', 2))
    foo.add_call(Call(Call.call, 'add', 2))
    foo.add_make_test((3, 5, 7), [(0, 1, 2), (3, 4, 5)])
    foo.add_make_test((4, 5, 6), [(1,), (3, 4, 5)], ['i', 'SH_INT'])
    foo.add_make_test((8, 9, 10), [(1, 2, 3), (7,)])
    foo.add_make_test((8,), [(1,), (7,)])
    foo.add_make_test((0, 0, 0), [(4, 5, 6), (-4, -5, -6)])
    foo.output(sys.stdout)
