import common, semantic

common.header()

common.guard("SHTEXCOORDIMPL_HPP")
common.inprint('#include "ShTexCoord.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("TexCoord", "texture coordinate", "SH_TEXCOORD")

impl.implement_all()

common.endnamespace()
common.endguard("SHTEXCOORDIMPL_HPP")
