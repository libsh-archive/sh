// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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

#include "TexData.hpp"
#include "Error.hpp"
#include "Context.hpp"

namespace SH {

template<typename T, int N, typename T2>
TexData<T, N, T2>::TexData(const TextureNodePtr& node, const Generic<N, T2>& coords, bool indexed)
{
  if (Context::current()->parsing()) {
    Variable tex_var(node);
    Statement stmt(*this, tex_var, indexed ? OP_TEXI : OP_TEX, coords);
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } else {
    DEBUG_WARN("Immediate mode texture lookup is not yet implemented.");
    // TODO
  }
}

template<typename T, int N, typename T2>
TexData<T, N, T2>::TexData(const TextureNodePtr& node, const Generic<N, T2>& coords,
                               const Generic<N, T2>& dx, const Generic<N, T2>& dy)
{
  if (Context::current()->parsing()) {
    Variable tex_var(node);
    Statement stmt(*this, OP_TEXD, tex_var, coords, join(dx, dy));
    Context::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
  } else {
    error(ScopeException("Cannot do derivative texture lookup in immediate mode"));
  }
}

template<typename T, int N, typename T2>
TexData<T, N, T2>& TexData<T, N, T2>::operator=(const T& a)
{
  if (Context::current()->parsing()) {
    error(ScopeException("Cannot assign to a texture in retained mode"));
  } else {
    DEBUG_WARN("Immediate mode texture assignment is not yet implemented.");
    // TODO
  }
  return *this;
}

} // namespace

#endif // SHTEXDATAIMPL_HPP
