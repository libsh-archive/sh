import common, semantic

common.header()

common.guard("SHCOLORIMPL_HPP")
common.inprint('#include "ShColor.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("Color", "color", "SH_COLOR")

impl.implement_all()

common.endnamespace()
common.endguard("SHCOLORIMPL_HPP")
