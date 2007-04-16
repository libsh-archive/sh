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
#include "Generic.hpp"
#undef SH_DO_NOT_INCLUDE_GENERIC_IMPL
#else
#include "Generic.hpp"
#endif
#include "TextureNode.hpp"

namespace SH {

template<typename T, int N, typename T2>
class TexData : public T {
public:
  TexData(const TextureNodePtr& node, const Generic<N, T2>& coords, bool indexed);
  TexData(const TextureNodePtr& node, const Generic<N, T2>& coords,
            const Generic<N, T2>& dx, const Generic<N, T2>& dy);

  TexData& operator=(const T& a);
};

} // namespace

#include "TexDataImpl.hpp"

#endif // SHTEXDATA_HPP
