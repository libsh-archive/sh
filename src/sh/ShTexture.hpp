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
#ifndef SHTEXTURE_HPP
#define SHTEXTURE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

/** Trait class to represent texture filtering modes.
 */
struct 
ShFilteredTextureTraits : public ShTextureTraits {
  ShFilteredTextureTraits()
    : ShTextureTraits(1, SH_FILTER_MIPMAP_LINEAR, SH_WRAP_CLAMP_TO_EDGE)
  {}
};

template<typename T> class ShTextureRect;

/** One-dimensional power-of-two texture type.
 */
template<typename T>
class ShTexture1D
  : public ShBaseTexture1D<T> {
public:
  ShTexture1D()
    : ShBaseTexture1D<T>(ShFilteredTextureTraits())
  {}
  ShTexture1D(int width)
    : ShBaseTexture1D<T>(width, ShFilteredTextureTraits())
  {}
  typedef ShTextureRect<T> rectangular_type;
  typedef ShBaseTexture1D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional square power-of-two texture type.
 */
template<typename T>
class ShTexture2D
  : public ShBaseTexture2D<T> {
public:
  ShTexture2D()
    : ShBaseTexture2D<T>(ShFilteredTextureTraits())
  {}
  ShTexture2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShFilteredTextureTraits())
  {}
  //    ShTexture2D(int width, int height, ShTextureTraits& traits)
  //     : ShBaseTexture2D<T>(width, height, traits)
  //   {}
  typedef ShTextureRect<T> rectangular_type;
  typedef ShBaseTexture2D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional rectangular texture type.
 */
template<typename T>
class ShTextureRect
  : public ShBaseTextureRect<T> {
public:
  ShTextureRect()
    : ShBaseTextureRect<T>(ShFilteredTextureTraits())
  {}
  ShTextureRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShFilteredTextureTraits())
  {}
  //    ShTextureRect(int width, int height, ShTextureTraits& traits)
  //     : ShBaseTextureRect<T>(width, height, traits)
  //   {}
  typedef ShTextureRect<T> rectangular_type;
  typedef ShBaseTextureRect<T> base_type;
  typedef T return_type;
};

/** Three-dimensional texture type.
 */
template<typename T>
class ShTexture3D
  : public ShBaseTexture3D<T> {
public:
  ShTexture3D()
    : ShBaseTexture3D<T>(ShFilteredTextureTraits())
  {}
  ShTexture3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShFilteredTextureTraits())
  {}
  typedef ShTextureRect<T> rectangular_type;
  typedef ShBaseTexture3D<T> base_type;
  typedef T return_type;
};

/** Cubic texture type.
 * Cubic textures have six faces and are indexed by a three-dimensional
 * vector.   This vector does NOT have to be unit length.   The texel
 * indexed will depend only on the direction of the vector.
 */
template<typename T>
class ShTextureCube
  : public ShBaseTextureCube<T> {
public:
  ShTextureCube()
    : ShBaseTextureCube<T>(ShFilteredTextureTraits())
  {}
  ShTextureCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShFilteredTextureTraits())
  {}
  typedef ShTextureRect<T> rectangular_type;
  typedef ShBaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
