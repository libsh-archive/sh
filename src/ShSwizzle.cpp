// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <sstream>
#include "ShSwizzle.hpp"
#include "ShError.hpp"

namespace SH {

ShSwizzle::ShSwizzle(const ShSwizzle& other, int n)
  : m_srcSize(other.m_srcSize),
    m_size(other.m_size * n)
{
  SH_DEBUG_ASSERT(n >= 1);
  int i, j;
  if(alloc()) {
    for(j = 0; j < other.m_size; ++j) {
      m_index.ptr[j] = other[j];
    }
    for(i = 1; i < n; ++i) for(j = 0; j < other.m_size; ++j) {
      m_index.ptr[i * n + j] = m_index.ptr[j];
    }
  } else {
    SH_DEBUG_ASSERT(other.local());
    for(j = 0; j < other.m_size; ++j) {
      m_index.local[j] = other.m_index.local[j];
    }
    if(other.m_size == 1) {
      for(j = 1; j < n; ++j) m_index.local[j] = m_index.local[0];
    } else if(other.m_size == 2) {
      for(j = 2; j < 4; ++j) m_index.local[j] = m_index.local[j-2];
    } else {
      SH_DEBUG_ASSERT(n == 1);
    }
  }
}

ShSwizzleException::ShSwizzleException(const ShSwizzle& s, int index, int size)
  : ShException("")
{
  std::ostringstream out;
  out << "Swizzle error: " << index << " out of range [0, " << size << ") in " << s;

  m_message = out.str();
}

std::ostream& operator<<(std::ostream& out, const ShSwizzle& swizzle)
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
