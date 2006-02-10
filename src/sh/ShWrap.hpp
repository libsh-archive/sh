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
#ifndef SHWRAP_HPP
#define SHWRAP_HPP

namespace SH {

/** Set WrapClamp trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap/clamp boundary treatment enabled.   This version can be
 * used with any dimension of texture.
 */
template<typename T>
class ShWrapClamp : public T {
public:
  ShWrapClamp()
    : T()
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }
  ShWrapClamp(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP);
  }

  typedef ShWrapClamp<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

/** Set WrapClampToEdge trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap/clamp-to-boundary trait enabled.   This version can be
 * used with any dimension of texture.
 */
template<typename T>
class ShWrapClampToEdge : public T {
public:
  ShWrapClampToEdge()
    : T()
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
  ShWrapClampToEdge(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
  ShWrapClampToEdge(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }
  ShWrapClampToEdge(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE);
  }

  typedef ShWrapClampToEdge<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

/** Set WrapRepeat trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can be
 * used with any dimension of texture.
 */
template<typename T>
class ShWrapRepeat : public T {
public:
  ShWrapRepeat()
    : T()
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }
  ShWrapRepeat(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(ShTextureTraits::SH_WRAP_REPEAT);
  }

  typedef ShWrapRepeat<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

}

#endif
