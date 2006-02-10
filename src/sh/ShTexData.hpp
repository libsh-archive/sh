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
#ifndef SHTEXDATA_HPP
#define SHTEXDATA_HPP

#ifndef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#define SH_DO_NOT_INCLUDE_GENERIC_IMPL
#include "ShGeneric.hpp"
#undef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#else
#include "ShGeneric.hpp"
#endif
#include "ShTextureNode.hpp"

namespace SH {

template<typename T, int N, typename T2>
class ShTexData : public T {
public:
  ShTexData(const ShTextureNodePtr& node, const ShGeneric<N, T2>& coords, bool indexed);
  ShTexData(const ShTextureNodePtr& node, const ShGeneric<N, T2>& coords,
            const ShGeneric<N, T2>& dx, const ShGeneric<N, T2>& dy);

  ShTexData& operator=(const T& a);
};

} // namespace

#include "ShTexDataImpl.hpp"

#endif // SHTEXDATA_HPP
