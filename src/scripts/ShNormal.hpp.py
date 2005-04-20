import common, semantic

common.header()

common.guard("SHNORMAL_HPP")
common.inprint('#include "ShVector.hpp"')
common.namespace()

decl = semantic.Class("Normal", "normal", "SH_NORMAL",
                      """An n-normal.
""", "ShVector")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHNORMAL_HPP")
