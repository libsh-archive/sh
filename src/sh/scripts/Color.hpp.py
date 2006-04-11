import common, semantic

common.header()

common.guard("SHCOLOR_HPP")
common.inprint('#include "Attrib.hpp"')
common.namespace()

decl = semantic.Class("Color", "color", "COLOR",
                      """An n-color.
""")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHCOLOR_HPP")
