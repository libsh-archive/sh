import common, semantic

common.header()

common.guard("SHPLANE_HPP")
common.inprint('#include "Attrib.hpp"')
common.namespace()

decl = semantic.Class("Plane", "plane", "PLANE",
                      """An n-plane.
""")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHPLANE_HPP")
