import common, semantic

common.header()

common.guard("SHPOSITIONIMPL_HPP")
common.inprint('#include "ShPosition.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("Position", "position", "SH_POSITION", "ShPoint")

impl.implement_all()

common.endnamespace()
common.endguard("SHPOSITIONIMPL_HPP")
