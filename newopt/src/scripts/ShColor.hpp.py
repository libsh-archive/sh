import common, semantic

common.header()

common.guard("SHCOLOR_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.namespace()

decl = semantic.Class("Color", "color", "SH_COLOR",
                      """An n-color.
""")

decl.declare_all()

common.endnamespace()
common.inprint('#include "ShColorImpl.hpp"')
common.endguard("SHCOLOR_HPP")
