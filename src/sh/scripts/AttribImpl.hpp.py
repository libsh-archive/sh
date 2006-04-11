import common, semantic

common.header()

common.guard("ATTRIBIMPL_HPP")
common.inprint('#include "Attrib.hpp"')
common.inprint('#include "Context.hpp"')
common.inprint('#include "Statement.hpp"')
common.inprint('#include "Debug.hpp"')
common.inprint('')
common.namespace()

# TODO
#  - array constructor
# Special cases for CONST:
#  - operator=()

class Impl(semantic.Impl):
    def __init__(self):
        semantic.Impl.__init__(self, "Attrib", "attribute", "ATTRIB", "Generic")

    def copycons(self, args, size, extraTplArg=[]):
        other = args[0][1]

        common.inprint(self.tpl(size))
        if len(extraTplArg) > 0: common.inprint("template<" + ", ".join(extraTplArg) + ">")
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("  : Generic<" + self.sizevar(size) + ", T>" +
                       "(new VariableNode(Binding, " + self.sizevar(size) + ", StorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.indent()
        common.inprint("shASN(*this, " + other + ");")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def scalarcons(self, args, size, extraTplArg=[]):
        common.inprint(self.tpl(size))
        if len(extraTplArg) > 0: common.inprint("template<" + ", ".join(extraTplArg) + ">")
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("  : Generic<" + self.sizevar(size) + ", T>" +
                       "(new VariableNode(Binding, " + self.sizevar(size) + ", StorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.indent()
        
        common.inprint("if (Binding == CONST) {")
        common.indent()
        if size > 1:
            values = ""
            for i in range(0, size):
                if i > 0: values += ", "
                if args[0][0] == "const host_type":
                    values += "s" + str(i)
                else:
                    common.inprint("DEBUG_ASSERT(s" + str(i) + ".hasValues());")
                    values += "s" + str(i) + ".getValue(0)"
            common.inprint("host_type data[" + str(size) + "] = {" + values + "};")
            common.inprint("setValues(data);")
        else:
            if args[0][0] == "const host_type":
                common.inprint("setValue(0, s0);")
            else:
                common.inprint("DEBUG_ASSERT(s0.hasValues());")
                common.inprint("setValue(0, s0.getValue(0));")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        if args[0][0] != "const host_type":
            for i in range(0, size):
                common.inprint("(*this)[" + str(i) + "] = s" + str(i) + ";")
        else:
            data = ""
            for i in range(0, size):
                if data != "":
                    data += ", "
                data += "s" + str(i)
            common.inprint("(*this) = Attrib<" + self.sizevar(size) + ", CONST, T, Semantic>(" + data + ");")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def constructors(self, size = 0):
        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "()")
        common.inprint("  : Generic<" + self.sizevar(size) + ", T>" +
                       "(new VariableNode(Binding, " + self.sizevar(size) + ", StorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.inprint("}")
        common.inprint("")

        self.copycons([["const Generic<" + self.sizevar(size) + ", T2>&", "other"]], size, ["typename T2"])
        self.copycons([["const " + self.tplcls(size) + "&","other"]], size)
        self.copycons([["const " + self.tplcls(size, "Swizzled", "T2") + "&","other"]], size, ["typename T2"])

        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(const VariableNodePtr& node,")
        common.inprint("  const Swizzle& swizzle, bool neg)")
        common.inprint("  : Generic<" + self.sizevar(size) + ", T>" + "(node, swizzle, neg)")
        common.inprint("{")
        common.indent()
        common.deindent()
        common.inprint("}")
        common.inprint("")

        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(const host_type data[" + self.sizevar(size) + "])")
        common.inprint("  : Generic<" + self.sizevar(size) + ", T>" +
                       "(new VariableNode(Binding, " + self.sizevar(size) + ", StorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.indent()
        common.inprint("if (Binding == CONST) {")
        common.indent()
        common.inprint("for (int i = 0; i < " + self.sizevar(size) + "; i++) setValue(i, data[i]);")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        common.inprint("(*this) = Attrib<" + self.sizevar(size) + ", CONST, T, Semantic>(data);")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

        if size > 0:
            self.scalarcons([["const host_type", "s" + str(x)] for x in range(0, size)], size)
        if size > 1:
            self.scalarcons([["const Generic<1, T" + str(x + 2) + ">&", "s" + str(x)] for x in range(0, size)], size, ["typename T" + str(x + 2) for x in range(0, size)])

impl = Impl()

impl.implement_all()

common.endnamespace()
common.endguard("ATTRIBIMPL_HPP")
