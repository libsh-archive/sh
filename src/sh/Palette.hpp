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
#ifndef SHPALETTE_HPP
#define SHPALETTE_HPP

#include <cstddef>

#include "PaletteNode.hpp"
#include "Generic.hpp"

namespace SH {

template<typename T>
class Palette {
public:
  Palette(std::size_t size);
  ~Palette();
  
  // C++-time lookup
  const T& operator[](std::size_t index) const;
  T& operator[](std::size_t index);

  // Sh program runtime lookup
  template<typename T2>
  T operator[](const Generic<1, T2>& index) const;

  
private:
  PaletteNodePtr m_node;
  T* m_data;

  /* not implemented */
  Palette(const Palette& other) {}
  Palette& operator=(const Palette& other);
};

}

#include "PaletteImpl.hpp"

#endif
