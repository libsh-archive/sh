import common, semantic

common.header()

common.guard("SHTEXCOORD_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.namespace()

decl = semantic.Class("TexCoord", "texture coordinate", "SH_TEXCOORD",
                      """A texture coordinate.
""")

decl.declare_all()

common.endnamespace()
common.inprint('#include "ShTexCoordImpl.hpp"')
common.endguard("SHTEXCOORD_HPP")
