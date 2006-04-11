import common, semantic

common.header()

common.guard("SHNORMAL_HPP")
common.inprint('#include "Vector.hpp"')
common.namespace()

decl = semantic.Class("Normal", "normal", "SH_VECTOR",
                      """An n-normal.
""", "Vector")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHNORMAL_HPP")
