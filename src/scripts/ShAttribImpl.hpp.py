import common, semantic

common.header()

common.guard("SHATTRIBIMPL_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.inprint('#include "ShContext.hpp"')
common.inprint('#include "ShTypeInfo.hpp"')
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
        if len(extraTplArg) > 0: common.inprint("template<" + ", ".join(["typename " + x for x in extraTplArg]) + ">")
        common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ",shTypeIndex<T>()))")
        common.inprint("{")
        common.indent()
        common.inprint("shASN(*this, " + other + ");")
#         common.inprint("if (Binding == SH_CONST || uniform()) {")
#         common.indent()
#         common.inprint("SH_DEBUG_ASSERT(" + other + ".hasValues());")
#         common.inprint("H data[" + self.sizevar(size) + "];")
#         common.inprint(other + ".getValues(data);")
#         common.inprint("setValues(data);")
#         common.deindent()
#         common.inprint("} else {")
#         common.indent()
#         common.deindent()
#         common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def scalarcons(self, args, size, extraTplArg=[]):
        common.inprint(self.tpl(size))
        if len(extraTplArg) > 0: common.inprint("template<" + ", ".join(["typename " + x for x in extraTplArg]) + ">")
        common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(" + ', '.join([' '.join(x) for x in args]) + ")")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ",shTypeIndex<T>()))")
        common.inprint("{")
        common.indent()
        
        common.inprint("if (Binding == SH_CONST) {")
        common.indent()
        for i in range(0, size):
            if args[0][0] == "H":
                common.inprint("setValue(" + str(i) + ", s" + str(i) + ");")
            else:
                common.inprint("SH_DEBUG_ASSERT(s" + str(i) + ".hasValues());")
                common.inprint("setValue(" + str(i) + ", s" + str(i) + ".getValue(0));")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        for i in range(0, size):
            if args[0][0] == "T":
                val = "ShAttrib<1, SH_CONST, T>(s" + str(i) + ")"
            else:
                val = "s" + str(i)
            common.inprint("(*this)[" + str(i) + "] = " + val + ";")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

    def constructors(self, size = 0):
        common.inprint(self.tpl(size))
        common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "()")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ",shTypeIndex<T>()))")
        common.inprint("{")
        common.inprint("}")
        common.inprint("")

        self.copycons([["const ShGeneric<" + self.sizevar(size) + ", T2>&", "other"]], size, ["T2"])
        self.copycons([["const " + self.tplcls(size) + "&","other"]], size)
        self.copycons([["const " + self.tplcls(size, "Swizzled", "T2") + "&","other"]], size, ["T2"])
#         common.inprint(self.tpl(size))
#         common.inprint(self.tplcls(size) + "::" + self.name + "(const ShProgram& prg)")
#         common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
#                        "(new ShVariableNode(Binding, " + self.sizevar(size) + "))")
#         common.inprint("{")
#         common.inprint("  *this = prg;")
#         common.inprint("}")
#         common.inprint("")

        common.inprint(self.tpl(size))
        common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(const ShVariableNodePtr& node,")
        common.inprint("  const ShSwizzle& swizzle, bool neg)")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" + "(node, swizzle, neg)")
        common.inprint("{")
        common.indent()
        common.deindent()
        common.inprint("}")
        common.inprint("")

        common.inprint(self.tpl(size))
        common.inprint("inline")
        common.inprint(self.tplcls(size) + "::" + self.name + "(H data[" + self.sizevar(size) + "])")
        common.inprint("  : ShGeneric<" + self.sizevar(size) + ", T>" +
                       "(new ShVariableNode(Binding, " + self.sizevar(size) + ",shTypeIndex<T>()))")
        common.inprint("{")
        common.indent()
        common.inprint("if (Binding == SH_CONST) {")
        common.indent()
        common.inprint("for (int i = 0; i < " + self.sizevar(size) + "; i++) setValue(i, data[i]);")
        common.deindent()
        common.inprint("} else {")
        common.indent()
        common.inprint("(*this) = ShAttrib<" + self.sizevar(size) + ", SH_CONST, T>(data);")
        common.deindent()
        common.inprint("}")
        common.deindent()
        common.inprint("}")
        common.inprint("")

        if size > 0:
            self.scalarcons([["H", "s" + str(x)] for x in range(0, size)], size)
        if size > 1:
            self.scalarcons([["const ShGeneric<1, T" + str(x + 2) + ">&", "s" + str(x)] for x in range(0, size)], size, ["T" + str(x + 2) for x in range(0, size)])

#     def assign(self, fun, args, size):
#         common.inprint(self.tpl(size) + "\n" +
#                        self.tplcls(size) + "&\n" +
#                        self.tplcls(size) + "::" + fun +
#                        "(" + ', '.join([' '.join(x) for x in args]) + ")")
#         common.inprint("{")
#         common.indent()
#         other = args[0][1]
#         if fun[-1] == "=" and fun[-2] in ["+", "-", "/", "*"]:
#             common.inprint("*this = *this " + fun[-2] + " " + other + ";")
#         else:
#             common.inprint("if (Binding == SH_CONST || uniform()) {")
#             common.indent()
#             common.inprint("SH_DEBUG_ASSERT(!ShEnvironment::insideShader);")
#             common.inprint("SH_DEBUG_ASSERT(" + other + ".hasValues());")
#             common.inprint("H data[" + self.sizevar(size) + "];")
#             common.inprint(other + ".getValues(data);")
#             common.inprint("setValues(data);")
#             common.deindent()
#             common.inprint("} else {")
#             common.indent()
#             common.inprint("ShStatement asn(*this, SH_OP_ASN, " + other + ");")
#             common.inprint("ShEnvironment::shader->tokenizer.blockList()->addStatement(asn);")
#             common.deindent()
#             common.inprint("}")

#         common.inprint("return *this;")
#         common.deindent()
#         common.inprint("}")
#         common.inprint("")
        

impl = Impl()

impl.implement_all()

common.endnamespace()
common.endguard("SHATTRIBIMPL_HPP")
