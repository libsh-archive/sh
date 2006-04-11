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
#ifndef SHTRACKBALL_HPP
#define SHTRACKBALL_HPP

#include <sh/sh.hpp>

namespace SH {

class Trackball {
public:
  Trackball() : m_width(0.0), m_height(0.0) {}
  
  ~Trackball() {}

  void resize(float width, float height);
  Matrix4x4f rotate(float sx, float sy, float ex, float ey) const;

private:
  float m_width, m_height;
};

}

#endif
