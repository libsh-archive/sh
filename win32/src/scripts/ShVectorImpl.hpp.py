import common, semantic

common.header()

common.guard("SHVECTORIMPL_HPP")
common.inprint('#include "ShVector.hpp"')
common.inprint('')
common.namespace()

impl = semantic.Impl("Vector", "vector", "SH_VECTOR")

impl.implement_all()

common.endnamespace()
common.endguard("SHVECTORIMPL_HPP")
