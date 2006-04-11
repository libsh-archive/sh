import common, semantic

common.header()

common.guard("SHPOINT_HPP")
common.inprint('#include "Attrib.hpp"')
common.namespace()

decl = semantic.Class("Point", "point", "POINT",
                      """An n-point.
""")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHPOINT_HPP")
