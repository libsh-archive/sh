import common, semantic

common.header()

common.guard("SHTEXCOORD_HPP")
common.inprint('#include "Attrib.hpp"')
common.namespace()

decl = semantic.Class("TexCoord", "texture coordinate", "SH_TEXCOORD",
                      """A texture coordinate.
""")

decl.declare_parametrization()

common.endnamespace()
common.endguard("SHTEXCOORD_HPP")
