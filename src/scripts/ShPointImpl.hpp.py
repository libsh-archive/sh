import common, semantic

common.header()

common.guard("SHPOINTIMPL_HPP")
common.inprint('#include "ShPoint.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("Point", "point", "SH_POINT")

impl.implement_all()

common.endnamespace()
common.endguard("SHPOINTIMPL_HPP")
