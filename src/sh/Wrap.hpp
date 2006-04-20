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
class WrapClamp : public T {
public:
  WrapClamp()
    : T()
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP);
  }
  WrapClamp(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP);
  }
  WrapClamp(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP);
  }
  WrapClamp(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP);
  }

  typedef WrapClamp<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

/** Set WrapClampToEdge trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap/clamp-to-boundary trait enabled.   This version can be
 * used with any dimension of texture.
 */
template<typename T>
class WrapClampToEdge : public T {
public:
  WrapClampToEdge()
    : T()
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP_TO_EDGE);
  }
  WrapClampToEdge(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP_TO_EDGE);
  }
  WrapClampToEdge(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP_TO_EDGE);
  }
  WrapClampToEdge(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_CLAMP_TO_EDGE);
  }

  typedef WrapClampToEdge<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

/** Set WrapRepeat trait in Texture type.
 * Use this template to indicate that a texture should be set up with 
 * the wrap-repeat boundary trait enabled.   This version can be
 * used with any dimension of texture.
 */
template<typename T>
class WrapRepeat : public T {
public:
  WrapRepeat()
    : T()
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_REPEAT);
  }
  WrapRepeat(int width)
    : T(width)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_REPEAT);
  }
  WrapRepeat(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_REPEAT);
  }
  WrapRepeat(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().wrapping(TextureTraits::WRAP_REPEAT);
  }

  typedef WrapRepeat<typename T::rectangular_type> rectangular_type;
  typedef typename T::base_type base_type;
  typedef typename T::return_type return_type;
};

}

#endif
