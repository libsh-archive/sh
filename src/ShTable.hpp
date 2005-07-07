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
#ifndef SHTABLE_HPP
#define SHTABLE_HPP

#include "ShBaseTexture.hpp"

namespace SH {

struct
ShTableTraits : public ShTextureTraits {
  ShTableTraits()
    : ShTextureTraits(1, SH_FILTER_NONE, SH_WRAP_CLAMP_TO_EDGE)
  {
  }
};

template<typename T> class ShTableRect;

template<typename T>
class ShTable1D
  : public ShBaseTexture1D<T> {
public:
  ShTable1D()
    : ShBaseTexture1D<T>(ShTableTraits())
  {}
  ShTable1D(int width)
    : ShBaseTexture1D<T>(width, ShTableTraits())
  {}
  typedef ShTableRect<T> rectangular_type;
  typedef ShBaseTexture1D<T> base_type;
  typedef T return_type;
};

template<typename T>
class ShTable2D
  : public ShBaseTexture2D<T> {
public:
  ShTable2D()
    : ShBaseTexture2D<T>(ShTableTraits())
  {}
  ShTable2D(int width, int height)
    : ShBaseTexture2D<T>(width, height, ShTableTraits())
  {}
  typedef ShTableRect<T> rectangular_type;
  typedef ShBaseTexture2D<T> base_type;
  typedef T return_type;
};

template<typename T>
class ShTableRect
  : public ShBaseTextureRect<T> {
public:
  ShTableRect()
    : ShBaseTextureRect<T>(ShTableTraits())
  {}
  ShTableRect(int width, int height)
    : ShBaseTextureRect<T>(width, height, ShTableTraits())
  {}
  typedef ShTableRect<T> rectangular_type;
  typedef ShBaseTextureRect<T> base_type;
  typedef T return_type;
};

template<typename T>
class ShTable3D
  : public ShBaseTexture3D<T> {
public:
  ShTable3D()
    : ShBaseTexture3D<T>(ShTableTraits())
  {}
  ShTable3D(int width, int height, int depth)
    : ShBaseTexture3D<T>(width, height, depth, ShTableTraits())
  {}
  typedef ShTableRect<T> rectangular_type;
  typedef ShBaseTexture3D<T> base_type;
  typedef T return_type;
};

template<typename T>
class ShTableCube
  : public ShBaseTextureCube<T> {
public:
  ShTableCube()
    : ShBaseTextureCube<T>(ShTableTraits())
  {}
  ShTableCube(int width, int height)
    : ShBaseTextureCube<T>(width, height, ShTableTraits())
  {}
  typedef ShTableRect<T> rectangular_type;
  typedef ShBaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
