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
#ifndef SHMIPFILTER_HPP
#define SHMIPFILTER_HPP

namespace SH {

/** Enable mipmapping in Texture type.
 * Use this template to indicate that a texture should be mipmapped
 * and whether it should choose the nearest mipmap or interpolate
 * between the two closest one.
 */
template<typename T, bool interpolated=true>
class ShMIPFilter : public T {
public:
  ShMIPFilter()
    : T()
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width)
    : T(width)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width, int height)
    : T(width, height)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }
  ShMIPFilter(int width, int height, int depth)
    : T(width, height, depth)
  {
    if (interpolated) {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_LINEAR);
    } else {
      this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_MIPMAP_NEAREST);
    }
  }

  typedef typename T::return_type return_type;
  
};

/** Disable mipmapping in Texture type.
 * Use this template to indicate that a texture should not be mipmapped.
 */
template<typename T>
class ShNoMIPFilter : public T {
public:
  ShNoMIPFilter()
    : T()
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width)
    : T(width)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }
  ShNoMIPFilter(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().filtering(ShTextureTraits::SH_FILTER_NONE);
  }

  typedef typename T::return_type return_type;
  
};

}

#endif
