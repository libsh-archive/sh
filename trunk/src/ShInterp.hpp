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
#ifndef SHINTERP_HPP
#define SHINTERP_HPP

namespace SH {

/** Set Interpolation level in Texture type.
 * Use this template to indicate that a texture should be interpolated
 * to a particular level L.
 * For example, ShInterp<0, T> implies a nearest-neighbour lookup,
 * whereas ShInterp<1, T> implies linear interpolation.
 */
template<int L, typename T>
class ShInterp : public T {
public:
  static int level() {
    if (L >= 2) return 3; else return L;
  }
  
  ShInterp()
    : T()
  {
    this->m_node->traits().interpolation(level());
  }
  ShInterp(int width)
    : T(width)
  {
    this->m_node->traits().interpolation(level());
  }
  ShInterp(int width, int height)
    : T(width, height)
  {
    this->m_node->traits().interpolation(level());
  }
  ShInterp(int width, int height, int depth)
    : T(width, height, depth)
  {
    this->m_node->traits().interpolation(level());
  }

  typedef typename T::return_type return_type;
  
};

}

#endif
