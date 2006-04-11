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

#include "BaseTexture.hpp"

namespace SH {

/** Trait class to represent texture filtering modes.
 */
struct 
FilteredTextureTraits : public TextureTraits {
  FilteredTextureTraits()
    : TextureTraits(1, FILTER_MIPMAP_LINEAR, WRAP_CLAMP_TO_EDGE)
  {}
};

template<typename T> class TextureRect;

/** One-dimensional power-of-two texture type.
 */
template<typename T>
class Texture1D
  : public BaseTexture1D<T> {
public:
  Texture1D()
    : BaseTexture1D<T>(FilteredTextureTraits())
  {}
  Texture1D(int width)
    : BaseTexture1D<T>(width, FilteredTextureTraits())
  {}
  typedef TextureRect<T> rectangular_type;
  typedef BaseTexture1D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional square power-of-two texture type.
 */
template<typename T>
class Texture2D
  : public BaseTexture2D<T> {
public:
  Texture2D()
    : BaseTexture2D<T>(FilteredTextureTraits())
  {}
  Texture2D(int width, int height)
    : BaseTexture2D<T>(width, height, FilteredTextureTraits())
  {}
  //    Texture2D(int width, int height, TextureTraits& traits)
  //     : BaseTexture2D<T>(width, height, traits)
  //   {}
  typedef TextureRect<T> rectangular_type;
  typedef BaseTexture2D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional rectangular texture type.
 */
template<typename T>
class TextureRect
  : public BaseTextureRect<T> {
public:
  TextureRect()
    : BaseTextureRect<T>(FilteredTextureTraits())
  {}
  TextureRect(int width, int height)
    : BaseTextureRect<T>(width, height, FilteredTextureTraits())
  {}
  //    TextureRect(int width, int height, TextureTraits& traits)
  //     : BaseTextureRect<T>(width, height, traits)
  //   {}
  typedef TextureRect<T> rectangular_type;
  typedef BaseTextureRect<T> base_type;
  typedef T return_type;
};

/** Three-dimensional texture type.
 */
template<typename T>
class Texture3D
  : public BaseTexture3D<T> {
public:
  Texture3D()
    : BaseTexture3D<T>(FilteredTextureTraits())
  {}
  Texture3D(int width, int height, int depth)
    : BaseTexture3D<T>(width, height, depth, FilteredTextureTraits())
  {}
  typedef TextureRect<T> rectangular_type;
  typedef BaseTexture3D<T> base_type;
  typedef T return_type;
};

/** Cubic texture type.
 * Cubic textures have six faces and are indexed by a three-dimensional
 * vector.   This vector does NOT have to be unit length.   The texel
 * indexed will depend only on the direction of the vector.
 */
template<typename T>
class TextureCube
  : public BaseTextureCube<T> {
public:
  TextureCube()
    : BaseTextureCube<T>(FilteredTextureTraits())
  {}
  TextureCube(int width, int height)
    : BaseTextureCube<T>(width, height, FilteredTextureTraits())
  {}
  typedef TextureRect<T> rectangular_type;
  typedef BaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
