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
#ifndef SHTABLE_HPP
#define SHTABLE_HPP

#include "BaseTexture.hpp"

namespace SH {

struct
TableTraits : public TextureTraits {
  TableTraits()
    : TextureTraits(1, FILTER_NONE, WRAP_CLAMP_TO_EDGE)
  {
  }
};

template<typename T> class TableRect;

template<typename T>
class Table1D
  : public BaseTexture1D<T> {
public:
  Table1D()
    : BaseTexture1D<T>(TableTraits())
  {}
  Table1D(int width)
    : BaseTexture1D<T>(width, TableTraits())
  {}
  typedef TableRect<T> rectangular_type;
  typedef BaseTexture1D<T> base_type;
  typedef T return_type;
};

template<typename T>
class Table2D
  : public BaseTexture2D<T> {
public:
  Table2D()
    : BaseTexture2D<T>(TableTraits())
  {}
  Table2D(int width, int height)
    : BaseTexture2D<T>(width, height, TableTraits())
  {}
  typedef TableRect<T> rectangular_type;
  typedef BaseTexture2D<T> base_type;
  typedef T return_type;
};

template<typename T>
class TableRect
  : public BaseTextureRect<T> {
public:
  TableRect()
    : BaseTextureRect<T>(TableTraits())
  {}
  TableRect(int width, int height)
    : BaseTextureRect<T>(width, height, TableTraits())
  {}
  typedef TableRect<T> rectangular_type;
  typedef BaseTextureRect<T> base_type;
  typedef T return_type;
};

template<typename T>
class Table3D
  : public BaseTexture3D<T> {
public:
  Table3D()
    : BaseTexture3D<T>(TableTraits())
  {}
  Table3D(int width, int height, int depth)
    : BaseTexture3D<T>(width, height, depth, TableTraits())
  {}
  typedef TableRect<T> rectangular_type;
  typedef BaseTexture3D<T> base_type;
  typedef T return_type;
};

template<typename T>
class TableCube
  : public BaseTextureCube<T> {
public:
  TableCube()
    : BaseTextureCube<T>(TableTraits())
  {}
  TableCube(int width, int height)
    : BaseTextureCube<T>(width, height, TableTraits())
  {}
  typedef TableRect<T> rectangular_type;
  typedef BaseTextureCube<T> base_type;
  typedef T return_type;
};

}

#endif
