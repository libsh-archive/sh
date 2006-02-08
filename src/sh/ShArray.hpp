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
#ifndef SHARRAY_HPP
#define SHARRAY_HPP

namespace SH {

template<typename T> class ShBaseTexture1D;
template<typename T> class ShBaseTexture2D;
template<typename T> class ShBaseTexture3D;
template<typename T> class ShBaseTextureRect;
template<typename T> class ShBaseTextureCube;
  
/** Default traits for ShArray.
 * An array is a texture that does not support filtering or interpolation.
 */
struct
ShArrayTraits : public ShTextureTraits {
  ShArrayTraits()
    : ShTextureTraits(0, SH_FILTER_NONE, SH_WRAP_CLAMP_TO_EDGE)
  {}
};

template<typename T> class ShArrayRect;
/** One-dimensional array.
 */
template<typename T>
class ShArray1D
  : public ShBaseTexture1D<T> {
public:
  ShArray1D()
    : ShBaseTexture1D<T>(ShArrayTraits())
  {}
  ShArray1D(int width)
    : ShBaseTexture1D<T>(width, ShArrayTraits())
  {}
  typedef ShArrayRect<T> rectangular_type;
  typedef ShBaseTexture1D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional square power-of-two array.
 */
template<typename T>
class ShArray2D
  : public ShBaseTexture2D<T> {
public:
  ShArray2D()
    : ShBaseTexture2D<T>(ShArrayTraits())
  {}
  ShArray2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShArrayTraits())
  {}
  typedef ShArrayRect<T> rectangular_type;
  typedef ShBaseTexture2D<T> base_type;
  typedef T return_type;
};

/** Two-dimensional non-square array.
 */
template<typename T>
class ShArrayRect
  : public ShBaseTextureRect<T> {
public:
  ShArrayRect()
    : ShBaseTextureRect<T>(ShArrayTraits())
  {}
  ShArrayRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShArrayTraits())
  {}
  typedef ShArrayRect<T> rectangular_type;
  typedef ShBaseTextureRect<T> base_type;
  typedef T return_type;
};

/** Three-dimensional array.
 */
template<typename T>
class ShArray3D
  : public ShBaseTexture3D<T> {
public:
  ShArray3D()
    : ShBaseTexture3D<T>(ShArrayTraits())
  {}
  ShArray3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShArrayTraits())
  {}
  typedef ShArrayRect<T> rectangular_type;
  typedef ShBaseTexture3D<T> base_type;
  typedef T return_type;
};

/** Cube array.
 * A cube array is indexed by a 3D vector, and has six square power-of-two
 * faces.   The texel indexed depends only on the direction of the vector.
 */
template<typename T>
class ShArrayCube
  : public ShBaseTextureCube<T> {
public:
  ShArrayCube()
    : ShBaseTextureCube<T>(ShArrayTraits())
  {}
  ShArrayCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShArrayTraits())
  {}
  typedef ShArrayRect<T> rectangular_type;
  typedef ShBaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
