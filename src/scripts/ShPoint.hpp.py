import common, semantic

common.header()

common.guard("SHPOINT_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.namespace()

decl = semantic.Class("Point", "point", "SH_POINT",
                      """An n-point.
""")

decl.declare_all()

common.endnamespace()
common.endguard("SHPOINT_HPP")
