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
#include <iostream>
#include <sstream>
#include "Swizzle.hpp"
#include "Error.hpp"

namespace SH {

Swizzle::Swizzle(const Swizzle& other, int n)
  : m_srcSize(other.m_srcSize),
    m_size(other.m_size * n)
{
  DEBUG_ASSERT(n >= 1);
  int i, j;
  if(alloc()) {
    for(j = 0; j < other.m_size; ++j) {
      m_index.ptr[j] = other[j];
    }
    for(i = 1; i < n; ++i) for(j = 0; j < other.m_size; ++j) {
      m_index.ptr[i * other.m_size + j] = m_index.ptr[j];
    }
  } else {
    DEBUG_ASSERT(other.local());
    for(j = 0; j < other.m_size; ++j) {
      m_index.local[j] = other.m_index.local[j];
    }
    if(other.m_size == 1) {
      for(j = 1; j < n; ++j) m_index.local[j] = m_index.local[0];
    } else if(other.m_size == 2) {
      for(j = 2; j < 4; ++j) m_index.local[j] = m_index.local[j-2];
    } else {
      DEBUG_ASSERT(n == 1);
    }
  }
}

SwizzleException::SwizzleException(const Swizzle& s, int index, int size)
  : Exception("")
{
  std::ostringstream out;
  out << "Swizzle error: " << index << " out of range [0, " << size << ") in " << s;

  m_message = out.str();
}

std::ostream& operator<<(std::ostream& out, const Swizzle& swizzle)
{
  if (swizzle.identity()) return out;
  out << "(";
  for (int i = 0; i < swizzle.size(); i++) {
    if (i > 0) out << ", ";
    out << swizzle[i];
  }
  out << ")";
  return out;
}

}
