import common, re

# TODO: SH_CONST

types = {"ShHalf": "h", 
         "float": "f", 
         "double": "d", 

         "char": "b",
         "short": "s",
         "int": "i",
         "unsigned char": "ub",
         "unsigned short": "us",
         "unsigned int": "ui",

         "ShFracByte": "fb",
         "ShFracShort": "fs",
         "ShFracInt": "fi",
         "ShFracUByte": "fub",
         "ShFracUShort": "fus",
         "ShFracUInt": "fui" } 



class Class:
    def __init__(self, name, comment_name, enum, comment, parent = "ShAttrib",
                 parentargs = "<N, Binding, T, Swizzled>"):
        self.name = "Sh" + name
        self.comment_name = comment_name
        self.enum = enum
        self.comment = comment
        self.parent = parent
        self.parentargs = parentargs

    def sizevar(self, size):
        if size <= 0:
            return "N"
        else:
            return str(size)

    def declare(self):
        self.maincomment()
        self.open()
        self.constants(0)
        self.constructors(0)
        self.destructor()
        self.assignments(0)
        self.modifying(0)
        self.swizzles()
        self.private_constants(0)
        self.close()

    def declare_sized(self, size):
        #self.maincomment()
        self.open_sized(size)
        self.constants(size)
        self.constructors(size)
        self.constructors_sized(size)
        self.destructor()
        self.assignments(size)
        self.modifying(size)
        self.swizzles()
        self.private_constants(size)
        self.close()

    def declare_all(self):
        self.declare()
        common.inprint('')

        for i in range(1, 5):
            self.declare_sized(i)
            common.inprint('')

        self.typedefs()

    def maincomment(self):
        common.doxygen(self.comment)

    def open(self):
        common.inprint("""template<int N, ShBindingType Binding, typename T=float, bool Swizzled=false>
class """ + self.name + " : public " + self.parent + self.parentargs + """ {
public:""")
        common.indent()

    def open_sized(self, size):
        pa = self.parentargs.replace("N", self.sizevar(size))
        common.inprint("template<ShBindingType Binding, typename T, bool Swizzled>\n" +
                       "class " + self.name + "<" + str(size) + ", Binding, T, Swizzled>" +
                       " : public " + self.parent + pa + " {\n" +
                       "public:")
        common.indent()
        #common.inprint("static const int N = " + str(size) + ";\n")

    def close(self):
        common.deindent()
        common.inprint("};")

    def constructors(self, size):
        common.inprint(self.name + "();")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "(const ShGeneric<" + self.sizevar(size) + ", T2>& other);")

        # note: need to put the default copy constructor, otherwise it is
        # implicitly defined and not what we want 
        common.inprint(self.name + "(const " + self.name + "<" +
          self.sizevar(size) + ", Binding, T, Swizzled>& other);")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "(const " + self.name + "<" + self.sizevar(size) + ", Binding, T2, Swizzled>& other);")
        common.inprint(self.name + "(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);")
        # common.inprint(self.name + "(const ShProgram&);")
        common.inprint("explicit " + self.name + "(host_type data[" + self.sizevar(size) + "]);")
        common.inprint("")

    def destructor(self):
        common.inprint("~" + self.name + "();\n")

    def constructors_sized(self, size):
        common.inprint(self.name + "(" + ', '.join(["host_type"] * size) + ");")
        if size != 1:
            common.inprint("template<" + ", ".join(["typename T" + str(x) for x in range(2, size + 2)]) + ">")
            common.inprint(self.name + "(" + ', '.join(["const ShGeneric<1, T" + str(x) + ">&" for x in range(2, size + 2)]) + ");")
        common.inprint('')

    def assignments(self, size):
        # note: need to put the default assignment, otherwise it is
        # implicitly defined and not what we want 
        common.inprint("\ntemplate<typename T2>\n" +
                       self.name + "& operator=(const ShGeneric<" + self.sizevar(size) + ", T2>& other);\n" +
                       "\ntemplate<typename T2>\n" +
                       self.name + "& operator=(const " + self.name + "<" +
                       self.sizevar(size) + ", Binding, T2, Swizzled>& other);\n" +
                       self.name + "& operator=(const " + self.name + "<" +
                       self.sizevar(size) + ", Binding, T, Swizzled>& other);\n")
        if size == 1:
            common.inprint(self.name + "& operator=(host_type other);\n")
        common.inprint(self.name + "& operator=(const ShProgram& prg);\n")

    def modifying(self, size):
        common.inprint("\n" + self.name + "& operator++();")
        common.inprint("\n" + self.name + "& operator--();")
        
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "& operator+=(const ShGeneric<" + self.sizevar(size) + ", T2>& right);")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "& operator-=(const ShGeneric<" + self.sizevar(size) + ", T2>& right);")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "& operator*=(const ShGeneric<" + self.sizevar(size) + ", T2>& right);")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "& operator/=(const ShGeneric<" + self.sizevar(size) + ", T2>& right);")
        common.inprint("\ntemplate<typename T2>")
        common.inprint(self.name + "& operator%=(const ShGeneric<" + self.sizevar(size) + ", T2>& right);")
        
        common.inprint(self.name + "& operator*=(host_type);")
        common.inprint(self.name + "& operator/=(host_type);")
        common.inprint(self.name + "& operator%=(host_type);")
        common.inprint(self.name + "& operator+=(host_type);")
        common.inprint(self.name + "& operator-=(host_type);")
        if size != 1:
            common.inprint("\ntemplate<typename T2>")
            common.inprint(self.name + "& operator+=(const ShGeneric<1, T2>&);")
            common.inprint("\ntemplate<typename T2>")
            common.inprint(self.name + "& operator-=(const ShGeneric<1, T2>&);")
            common.inprint("\ntemplate<typename T2>")
            common.inprint(self.name + "& operator*=(const ShGeneric<1, T2>&);")
            common.inprint("\ntemplate<typename T2>")
            common.inprint(self.name + "& operator/=(const ShGeneric<1, T2>&);")
            common.inprint("\ntemplate<typename T2>")
            common.inprint(self.name + "& operator%=(const ShGeneric<1, T2>&);")

    def swizzles(self):
        for num in range(1, 5):
            args = ', '.join(["int"] * num)
            common.inprint(self.name + "<" + str(num) + ", Binding, T, true> operator()(" + args + ") const;")
        common.inprint(self.name + "<1, Binding, T, true> operator[](int) const;")
        common.inprint('')
        common.inprint("template<int N2>\n" +
                       self.name + "<N2, Binding, T, true> swiz(int indices[]) const;")
        common.inprint('')
        common.inprint(self.name + " operator-() const;")

    def constants(self, size):
        common.inprint("""typedef T storage_type;
typedef typename ShHostType<T>::type host_type; 
typedef typename ShMemType<T>::type mem_type; 
static const ShBindingType binding_type = Binding;
static const ShSemanticType semantic_type = """ + self.enum + """;\n""")
        common.inprint("typedef " + self.name + "<" + self.sizevar(size) + ", SH_INPUT, T> InputType;")
        common.inprint("typedef " + self.name + "<" + self.sizevar(size) + ", SH_OUTPUT, T> OutputType;")
        common.inprint("typedef " + self.name + "<" + self.sizevar(size) + ", SH_INOUT, T> InOutType;")
        common.inprint("typedef " + self.name + "<" + self.sizevar(size) + ", SH_TEMP, T> TempType;")
        common.inprint("typedef " + self.name + "<" + self.sizevar(size) + ", SH_CONST, T> ConstType;")

    def private_constants(self, size):
        common.inprint("private:")
        common.indent()
        pa = self.parentargs.replace("N", self.sizevar(size))
        common.inprint("typedef " + self.parent + pa + " ParentType;")
        common.deindent()

    def typedefs(self):
        name = self.name.replace('Sh', '', 1)
        for t in types:
            for i in range(1, 5):
                common.inprint("typedef Sh" + name + "<" + str(i) + ", SH_INPUT, " + t + "> ShInput" + name + str(i) + types[t] + ";")
                common.inprint("typedef Sh" + name + "<" + str(i) + ", SH_OUTPUT, " + t + "> ShOutput" + name + str(i) + types[t] + ";")
                common.inprint("typedef Sh" + name + "<" + str(i) + ", SH_INOUT, " + t + "> ShInOut" + name + str(i) + types[t] + ";")
                common.inprint("typedef Sh" + name + "<" + str(i) + ", SH_TEMP, " + t + "> Sh" + name + str(i) + types[t] + ";")
                common.inprint("typedef Sh" + name + "<" + str(i) + ", SH_CONST, " + t + "> ShConst" + name + str(i) + types[t] + ";")
            common.inprint("\n")

class Impl:
    def __init__(self, name, comment_name, enum, parent = "ShAttrib"):
        self.name = "Sh" + name
        self.comment_name = comment_name
        self.enum = enum
        self.parent = parent

    def sizevar(self, size):
        if size <= 0:
            return "N"
        else:
            return str(size)

    def implement(self, size = 0):
        self.constructors(size)
        self.destructor(size)
        self.assignments(size)
        self.modifying(size)
        self.swizzles(size)


    def implement_all(self):
        self.implement()
        common.inprint('')

        for i in range(1, 5):
            self.implement(i)
            common.inprint('')

    def tpl(self, size):
        s = "template<"
        if size <= 0:
            s += "int N, "
        s += "ShBindingType Binding, typename T, bool Swizzled>"
        return s

    def tplcls(self, size, swiz = "Swizzled", type="T"):
        s = self.name + "<"
        try:
            s += size
        except:
            if size <= 0:
                s += "N"
            else:
                s += str(size)
        s += ", Binding, " + type + ", " + swiz + ">"
        return s

    def constructor(self, args, size, extraTplArg=[]):
        extraTplStr = ""
        if len(extraTplArg) > 0: extraTplStr = "template<" + ",".join(extraTplArg) + ">\n"
        common.inprint(self.tpl(size) + "\n" +
                       extraTplStr +
                       "inline\n" +
                       self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        if len(args) > 0:
            common.inprint("  : ParentType(" + ', '.join([re.sub(r'\[.*\]', r'', x[-1]) for x in args]) + ")")
        common.inprint("{")
        common.indent()
        common.inprint("this->m_node->specialType(" + self.enum + ");")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def constructors(self, size = 0):
        self.constructor([], size)
        if size <= 0:
            s = "N"
        else:
            s = str(size)
        self.constructor([["const ShGeneric<" + s + ", T2>&", "other"]], size, ["typename T2"])
        self.constructor([["const " + self.tplcls(size) + "&", "other"]], size)
        self.constructor([["const " + self.tplcls(size, "Swizzled", "T2") + "&", "other"]], size, ["typename T2"])
        self.constructor([["const ShVariableNodePtr&", "node"],
                          ["const ShSwizzle&", "swizzle"],
                          ["bool", "neg"]], size)
        self.constructor([["host_type", "data[" + self.sizevar(size) + "]"]], size)
        # self.constructor([["const ShProgram&", "prg"]], size)
        if size > 0:
            self.constructor([["host_type", "s" + str(x)] for x in range(0, size)], size)
        if size > 1:
            self.constructor([["const ShGeneric<1, T" + str(x + 2) + ">&", "s" + str(x)] for x in range(0, size)], size, 
                ["typename T" + str(x) for x in range(2, size + 2)])
        
    def destructor(self, size):
        common.inprint(self.tpl(size) + "\n" +
                       "inline\n" +
                       self.tplcls(size) + "::~" + self.name + "()")
        common.inprint("{")
        common.inprint("}")
        common.inprint("")

    def assign(self, fun, args, size, extraTplArg=[]):
        extraTplStr = ""
        if len(extraTplArg) > 0: extraTplStr = "template<" + ", ".join(extraTplArg) + ">\n"
        common.inprint(self.tpl(size) + "\n" +
                       extraTplStr +
                       "inline\n" +
                       self.tplcls(size) + "&\n" +
                       self.tplcls(size) + "::" + fun +
                       "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("{")
        common.indent()
        common.inprint("ParentType::" + fun + "(" + ', '.join([x[-1] for x in args]) + ");")
        common.inprint("return *this;")
        common.deindent()
        common.inprint("}")
        common.inprint("")
        
    def assignments(self, size = 0):
        if size <= 0:
            s = "N"
        else:
            s = str(size)
        self.assign("operator=", [["const ShGeneric<" + s + ", T2>&", "other"]], size, ["typename T2"])
        self.assign("operator=", [["const " + self.tplcls(size) + "&", "other"]], size)
        self.assign("operator=", [["const " + self.tplcls(size, "Swizzled", "T2") + "&", "other"]], size, ["typename T2"])
        if size == 1:
            self.assign("operator=", [["host_type", "other"]], size)
        self.assign("operator=", [["const ShProgram&", "prg"]], size)

    def modifying(self, size = 0):
        if size <= 0:
            s = "N"
        else:
            s = str(size)
        self.assign("operator++", [], size, [])
        self.assign("operator--", [], size, [])

        self.assign("operator+=", [["const ShGeneric<" + s + ", T2>&", "right"]], size, ["typename T2"])
        self.assign("operator-=", [["const ShGeneric<" + s + ", T2>&", "right"]], size, ["typename T2"])
        self.assign("operator*=", [["const ShGeneric<" + s + ", T2>&", "right"]], size, ["typename T2"])
        self.assign("operator/=", [["const ShGeneric<" + s + ", T2>&", "right"]], size, ["typename T2"])
        self.assign("operator%=", [["const ShGeneric<" + s + ", T2>&", "right"]], size, ["typename T2"])

        self.assign("operator+=", [["host_type", "right"]], size)
        self.assign("operator-=", [["host_type", "right"]], size)
        self.assign("operator*=", [["host_type", "right"]], size)
        self.assign("operator/=", [["host_type", "right"]], size)
        self.assign("operator%=", [["host_type", "right"]], size)
        if size != 1:
            self.assign("operator+=", [["const ShGeneric<1, T2>&", "right"]], size, ["typename T2"])
            self.assign("operator-=", [["const ShGeneric<1, T2>&", "right"]], size, ["typename T2"])
            self.assign("operator*=", [["const ShGeneric<1, T2>&", "right"]], size, ["typename T2"])
            self.assign("operator/=", [["const ShGeneric<1, T2>&", "right"]], size, ["typename T2"])
            self.assign("operator%=", [["const ShGeneric<1, T2>&", "right"]], size, ["typename T2"])
            
    def swizzle(self, num, size, op = "()"):
        args = ["s" + str(i) for i in range(0, num)]
        common.inprint(self.tpl(size) + "\n" +
                       "inline\n" +
                       self.tplcls(num, "true") + "\n" +
                       self.tplcls(size) + "::operator" + op + "(" + ', '.join(["int " + x for x in args]) + ")" +
                       " const")
        common.inprint("{")
        common.indent()
        if size <= 0:
            args = ["N"] + args
        else:
            args = [str(size)] + args
        common.inprint("return " + self.tplcls(num, "true") + "(this->m_node, " +
                       "this->m_swizzle * ShSwizzle(" + ', '.join(args) + "), " +
                       "this->m_neg);")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def dynswiz(self, size):
        common.inprint(self.tpl(size) + "\n" +
                       "template<int N2>\n" +
                       self.tplcls("N2", "true") + "\n" +
                       self.tplcls(size) + "::swiz(int indices[]) const")
        common.inprint("{")
        common.indent()
        args = ["N2", "indices"]
        if size <= 0:
            args = ["N"] + args
        else:
            args = [str(size)] + args
        common.inprint("return " + self.tplcls("N2", "true") + "(this->m_node, " +
                       "this->m_swizzle * ShSwizzle(" + ', '.join(args) + "), " +
                       "this->m_neg);")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def swizzles(self, size = 0):
        for num in range(1, 5):
            self.swizzle(num, size)
        self.dynswiz(size)
        self.swizzle(1, size, '[]')
        
        common.inprint(self.tpl(size) + "\n" +
                       self.tplcls(size) + "\n" +
                       self.tplcls(size) + "::operator-() const")
        common.inprint("{")
        common.indent()
        common.inprint("return " + self.tplcls(size) + "(this->m_node, this->m_swizzle, !this->m_neg);");
        common.deindent()
        common.inprint("}")


def instantiate(name):
    # @todo check that this works with types properly 
    for i in range(1, 5):
        for b in ["INPUT", "OUTPUT", "INOUT", "TEMP", "CONST"]:
            for t in types:  
                for s in ["false", "true"]:
                    common.inprint("template class Sh" + name + "<" + str(i) +
                    ", SH_" + b + ", " + t + ", " + s + ">;")
