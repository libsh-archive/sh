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

ShSwizzle::ShSwizzle(int srcSize)
  : m_srcSize(srcSize)
{
  for (int i = 0; i < srcSize; i++) m_indices.push_back(i);
}

ShSwizzle::ShSwizzle(int srcSize, int i0)
  : m_srcSize(srcSize)
{
  if (i0 < 0 || i0 >= srcSize) shError( ShSwizzleException(*this, i0, srcSize) );
  m_indices.push_back(i0);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1)
  : m_srcSize(srcSize)
{
  if (i0 < 0 || i0 >= srcSize) shError( ShSwizzleException(*this, i0, srcSize) );
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) shError( ShSwizzleException(*this, i1, srcSize) );
  m_indices.push_back(i1);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2)
  : m_srcSize(srcSize)
{
  if (i0 < 0 || i0 >= srcSize) shError( ShSwizzleException(*this, i0, srcSize) );
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) shError( ShSwizzleException(*this, i1, srcSize) );
  m_indices.push_back(i1);
  if (i2 < 0 || i2 >= srcSize) shError( ShSwizzleException(*this, i2, srcSize) );
  m_indices.push_back(i2);
}

ShSwizzle::ShSwizzle(int srcSize, int i0, int i1, int i2, int i3)
  : m_srcSize(srcSize)
{
  if (i0 < 0 || i0 >= srcSize) shError( ShSwizzleException(*this, i0, srcSize) );
  m_indices.push_back(i0);
  if (i1 < 0 || i1 >= srcSize) shError( ShSwizzleException(*this, i1, srcSize) );
  m_indices.push_back(i1);
  if (i2 < 0 || i2 >= srcSize) shError( ShSwizzleException(*this, i2, srcSize) );
  m_indices.push_back(i2);
  if (i3 < 0 || i3 >= srcSize) shError( ShSwizzleException(*this, i3, srcSize) );
  m_indices.push_back(i3);
}

ShSwizzle::ShSwizzle(int srcSize, int size, int* indices)
  : m_srcSize(srcSize)
{
  for (int i = 0; i < size; i++) {
    if (indices[i] < 0 || indices[i] >= srcSize) shError( ShSwizzleException(*this, indices[i], srcSize) );
    m_indices.push_back(indices[i]);
  }
}

ShSwizzle::ShSwizzle(const ShSwizzle& other)
  : m_srcSize(other.m_srcSize)
{
  *this = other;
}

ShSwizzle::~ShSwizzle()
{
}

ShSwizzle& ShSwizzle::operator=(const ShSwizzle& other)
{
  m_srcSize = other.m_srcSize;
  m_indices = other.m_indices;
  //  m_indices.clear();
  //  for (int i = 0; i < other.size(); i++) {
  //    m_indices.push_back(other[i]);
  //  }
  return *this;
}

ShSwizzle& ShSwizzle::operator*=(const ShSwizzle& other)
{
  std::vector<int> indices;

  for (int i = 0; i < other.size(); i++) {
    if (other[i] < 0 || other[i] >= size()) {
      shError( ShSwizzleException(*this, other[i], size()) );
    }
    indices.push_back(m_indices[other[i]]);
  }
  m_srcSize = other.m_srcSize;
  m_indices = indices;
  return *this;
}

ShSwizzle ShSwizzle::operator*(const ShSwizzle& other) const
{
  ShSwizzle swizzle(*this);
  swizzle *= other;
  return swizzle;
}

int ShSwizzle::size() const
{
  return (int)m_indices.size();
}

int ShSwizzle::operator[](int index) const
{
  if (index >= size() || index < 0) shError( ShSwizzleException(*this, index, size()) );
  return m_indices[index];
}

ShSwizzleException::ShSwizzleException(const ShSwizzle& s, int index, int size)
  : ShException("")
{
  std::ostringstream out;
  out << "Swizzle error: " << index << " out of range [0, " << size << ") in " << s;

  m_message = out.str();
}

bool ShSwizzle::identity() const
{
  if (m_indices.size() != m_srcSize) return false;
  for (std::size_t i = 0; i < m_indices.size(); i++) {
    if (m_indices[i] != i) return false;
  }
  return true;
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
