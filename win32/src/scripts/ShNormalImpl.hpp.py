import common, semantic

common.header()

common.guard("SHNORMALIMPL_HPP")
common.inprint('#include "ShNormal.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("Normal", "normal", "SH_NORMAL", "ShVector")

impl.implement_all()

common.endnamespace()
common.endguard("SHNORMALIMPL_HPP")
