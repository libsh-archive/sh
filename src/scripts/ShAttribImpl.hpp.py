import common, semantic

common.header()

common.guard("SHATTRIBIMPL_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.inprint('#include "ShContext.hpp"')
common.inprint('#include "ShStatement.hpp"')
common.inprint('#include "ShEnvironment.hpp"')
common.inprint('#include "ShDebug.hpp"')
common.inprint('')
common.namespace()

# TODO
#  - array constructor
# Special cases for SH_CONST:
#  - operator=()

class Impl(semantic.Impl):
    def __init__(self):
        semantic.Impl.__init__(self, "Attrib", "attribute", "SH_ATTRIB", "ShGeneric")

    def copycons(self, args, size, extraTplArg=[]):
        other = args[0][1]

        common.inprint(self.tpl(size))
        if len(extraTplArg) > 0: common.inprint("template<" + ", ".join(extraTplArg) + ">")
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ", ShStorageTypeInfo<T>::value_type, Semantic))")
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
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ", ShStorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.indent()
        
        common.inprint("if (Binding == SH_CONST) {")
        common.indent()
        for i in range(0, size):
            if args[0][0] == "host_type":
                common.inprint("setValue(" + str(i) + ", s" + str(i) + ");")
            else:
                common.inprint("SH_DEBUG_ASSERT(s" + str(i) + ".hasValues());")
                common.inprint("setValue(" + str(i) + ", s" + str(i) + ".getValue(0));")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        if args[0][0] != "host_type":
            for i in range(0, size):
                common.inprint("(*this)[" + str(i) + "] = s" + str(i) + ";")
        else:
            data = ""
            for i in range(0, size):
                if data != "":
                    data += ", "
                data += "s" + str(i)
            common.inprint("(*this) = ShAttrib<" + self.sizevar(size) + ", SH_CONST, T, Semantic>(" + data + ");")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def constructors(self, size = 0):
        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "()")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ", ShStorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.inprint("}")
        common.inprint("")

        self.copycons([["const ShGeneric<" + self.sizevar(size) + ", T2>&", "other"]], size, ["typename T2"])
        self.copycons([["const " + self.tplcls(size) + "&","other"]], size)
        self.copycons([["const " + self.tplcls(size, "Swizzled", "T2") + "&","other"]], size, ["typename T2"])

        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(const ShVariableNodePtr& node,")
        common.inprint("  const ShSwizzle& swizzle, bool neg)")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" + "(node, swizzle, neg)")
        common.inprint("{")
        common.indent()
        common.deindent()
        common.inprint("}")
        common.inprint("")

        common.inprint(self.tpl(size))
        #common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(host_type data[" + self.sizevar(size) + "])")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ", ShStorageTypeInfo<T>::value_type, Semantic))")
        common.inprint("{")
        common.indent()
        common.inprint("if (Binding == SH_CONST) {")
        common.indent()
        common.inprint("for (int i = 0; i < " + self.sizevar(size) + "; i++) setValue(i, data[i]);")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        common.inprint("(*this) = ShAttrib<" + self.sizevar(size) + ", SH_CONST, T, Semantic>(data);")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

        if size > 0:
            self.scalarcons([["host_type", "s" + str(x)] for x in range(0, size)], size)
        if size > 1:
            self.scalarcons([["const ShGeneric<1, T" + str(x + 2) + ">&", "s" + str(x)] for x in range(0, size)], size, ["typename T" + str(x + 2) for x in range(0, size)])

impl = Impl()

impl.implement_all()

common.endnamespace()
common.endguard("SHATTRIBIMPL_HPP")
