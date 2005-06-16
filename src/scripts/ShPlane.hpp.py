import common, semantic

common.header()

common.guard("SHPLANE_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.namespace()

decl = semantic.Class("Plane", "plane", "SH_PLANE",
                      """An n-plane.
""")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHPLANE_HPP")
