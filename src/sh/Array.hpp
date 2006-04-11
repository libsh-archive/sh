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
#ifndef SHARRAY_HPP
#define SHARRAY_HPP

namespace SH {

template<typename T> class BaseTexture1D;
template<typename T> class BaseTexture2D;
template<typename T> class BaseTexture3D;
template<typename T> class BaseTextureRect;
template<typename T> class BaseTextureCube;
  
/** Default traits for Array.
 * An array is a texture that does not support filtering or interpolation.
 */
struct
ArrayTraits : public TextureTraits {
  ArrayTraits()
    : TextureTraits(0, FILTER_NONE, WRAP_CLAMP_TO_EDGE)
  {}
};

template<typename T> class ArrayRect;
/** One-dimensional array.
 */
template<typename T>
class Array1D
  : public BaseTexture1D<T> {
public:
  Array1D()
    : BaseTexture1D<T>(ArrayTraits())
  {}
  Array1D(int width)
    : BaseTexture1D<T>(width, ArrayTraits())
  {}
  typedef ArrayRect<T> rectangular_type;
  typedef BaseTexture1D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional square power-of-two array.
 */
template<typename T>
class Array2D
  : public BaseTexture2D<T> {
public:
  Array2D()
    : BaseTexture2D<T>(ArrayTraits())
  {}
  Array2D(int width, int height)
    : BaseTexture2D<T>(width, height, ArrayTraits())
  {}
  typedef ArrayRect<T> rectangular_type;
  typedef BaseTexture2D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional non-square array.
 */
template<typename T>
class ArrayRect
  : public BaseTextureRect<T> {
public:
  ArrayRect()
    : BaseTextureRect<T>(ArrayTraits())
  {}
  ArrayRect(int width, int height)
    : BaseTextureRect<T>(width, height, ArrayTraits())
  {}
  typedef ArrayRect<T> rectangular_type;
  typedef BaseTextureRect<T> base_type;
  typedef T return_type;
};

/** Three-dimensional array.
 */
template<typename T>
class Array3D
  : public BaseTexture3D<T> {
public:
  Array3D()
    : BaseTexture3D<T>(ArrayTraits())
  {}
  Array3D(int width, int height, int depth)
    : BaseTexture3D<T>(width, height, depth, ArrayTraits())
  {}
  typedef ArrayRect<T> rectangular_type;
  typedef BaseTexture3D<T> base_type;
  typedef T return_type;
};

/** Cube array.
 * A cube array is indexed by a 3D vector, and has six square power-of-two
 * faces.   The texel indexed depends only on the direction of the vector.
 */
template<typename T>
class ArrayCube
  : public BaseTextureCube<T> {
public:
  ArrayCube()
    : BaseTextureCube<T>(ArrayTraits())
  {}
  ArrayCube(int width, int height)
    : BaseTextureCube<T>(width, height, ArrayTraits())
  {}
  typedef ArrayRect<T> rectangular_type;
  typedef BaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
