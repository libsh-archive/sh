import common, semantic

common.header()

common.guard("SHNORMAL_HPP")
common.inprint('#include "ShVector.hpp"')
common.namespace()

decl = semantic.Class("Normal", "normal", "SH_NORMAL",
                      """An n-normal.
""", "ShVector")

decl.declare_all()

common.endnamespace()
common.inprint('#include "ShNormalImpl.hpp"')
common.endguard("SHNORMAL_HPP")
