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

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1)
  : m_srcSize(srcSize),
    m_indices(new int[2]),
    m_size(2)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  m_indices[0] = i0;
  m_indices[1] = i1;
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2)
  : m_srcSize(srcSize),
    m_indices(new int[3]),
    m_size(3)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  checkSrcSize(i2);
  m_indices[0] = i0;
  m_indices[1] = i1;
  m_indices[2] = i2;
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2, int i3)
  : m_srcSize(srcSize),
    m_indices(new int[4]),
    m_size(4)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  checkSrcSize(i2);
  checkSrcSize(i3);
  m_indices[0] = i0;
  m_indices[1] = i1;
  m_indices[2] = i2;
  m_indices[3] = i3;
}

ShSwizzle::ShSwizzle(int srcSize, int size, int* indices)
  : m_srcSize(srcSize),
    m_indices(new int[size]),
    m_size(size)
{
  for (int i = 0; i < size; i++) {
    checkSrcSize(indices[i]);
    m_indices[i] = indices[i];
  }
}


ShSwizzle& ShSwizzle::operator*=(const ShSwizzle& other)
{
  int* indices = new int[other.m_size];

  for (int i = 0; i < other.m_size; i++) {
    if (other[i] < 0 || other[i] >= m_size) {
      delete indices;
      shError( ShSwizzleException(*this, other[i], size()) );
    }
    indices[i] = m_indices[other[i]];
  }
  m_srcSize = other.m_srcSize;
  m_size = other.m_size;
  delete [] m_indices;
  m_indices = indices;
  return *this;
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
