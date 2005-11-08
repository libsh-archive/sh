// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHTEXDATAIMPL_HPP
#define SHTEXDATAIMPL_HPP

#include "ShTexData.hpp"
#include "ShError.hpp"

namespace SH {

template<typename T, int N, typename T2>
ShTexData<T, N, T2>::ShTexData(const ShTextureNodePtr& node, const ShGeneric<N, T2>& coords, bool indexed)
{
  if (ShContext::current()->parsing()) {
    ShVariable tex_var(node);
    ShStatement stmt(*this, tex_var, indexed ? SH_OP_TEXI : SH_OP_TEX, coords);
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } else {
    SH_DEBUG_WARN("Immediate mode texture lookup is not yet implemented.");
    // TODO
  }
}

template<typename T, int N, typename T2>
ShTexData<T, N, T2>::ShTexData(const ShTextureNodePtr& node, const ShGeneric<N, T2>& coords,
                               const ShGeneric<N, T2>& dx, const ShGeneric<N, T2>& dy)
{
  if (ShContext::current()->parsing()) {
    ShVariable tex_var(node);
    ShStatement stmt(*this, SH_OP_TEXD, tex_var, coords, join(dx, dy));
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } else {
    shError(ShScopeException("Cannot do derivative texture lookup in immediate mode"));
  }
}

template<typename T, int N, typename T2>
ShTexData<T, N, T2>& ShTexData<T, N, T2>::operator=(const T& a)
{
  if (ShContext::current()->parsing()) {
    shError(ShScopeException("Cannot assign to a texture in retained mode"));
  } else {
    SH_DEBUG_WARN("Immediate mode texture assignment is not yet implemented.");
    // TODO
  }
  return *this;
}

} // namespace

#endif // SHTEXDATAIMPL_HPP
