import common, semantic

common.header()

common.guard("SHVECTOR_HPP")
common.inprint('#include "ShAttrib.hpp"')
common.namespace()

decl = semantic.Class("Vector", "vector", "SH_VECTOR",
                      """A (geometric) n-vector.

By "vector" we mean a geometric vector, not just a tuple or an
array (unlike the unfortunate terminology in the standard
library).""")

decl.declare_all()

common.endnamespace()
common.endguard("SHVECTOR_HPP")
