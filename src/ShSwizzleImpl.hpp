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
#ifndef SH_SWIZZLEIMPL_HPP
#define SH_SWIZZLEIMPL_HPP
#include <iostream>
#include "ShSwizzle.hpp"
#include "ShError.hpp"

namespace SH {

inline ShSwizzle::ShSwizzle()
  : m_srcSize(0),
    m_indices(0),
    m_size(0)
{
}

inline ShSwizzle::ShSwizzle(int srcSize)
  : m_srcSize(srcSize),
    m_indices(new int[srcSize]),
    m_size(srcSize)
{
  for (int i = 0; i < srcSize; i++) m_indices[i] = i;
}

inline ShSwizzle::ShSwizzle(int srcSize, int i0)
  : m_srcSize(srcSize),
    m_indices(new int[1]),
    m_size(1)
{
  checkSrcSize(i0);
  m_indices[0] = i0;
}

inline ShSwizzle::ShSwizzle(const ShSwizzle& other)
  : m_srcSize(other.m_srcSize),
    m_indices(new int[other.m_size]),
    m_size(other.m_size)
{
  memcpy(m_indices, other.m_indices, sizeof(int)*m_size);
}

inline ShSwizzle::~ShSwizzle()
{
  delete [] m_indices;
}

inline ShSwizzle& ShSwizzle::operator=(const ShSwizzle& other)
{
  realloc(other.m_size);
  memcpy(m_indices, other.m_indices, sizeof(int)*m_size);
  m_srcSize = other.m_srcSize;
  return *this;
}

inline ShSwizzle ShSwizzle::operator*(const ShSwizzle& other) const
{
  ShSwizzle swizzle(*this);
  swizzle *= other;
  return swizzle;
}

inline int ShSwizzle::operator[](int index) const
{
  checkSize(index);
  return m_indices[index];
}

inline void ShSwizzle::checkSrcSize(int index) const
{
  if (index < 0 || index >= m_srcSize) {
    delete [] m_indices;
    shError( ShSwizzleException(*this, index, m_srcSize) );
  }
}

inline void ShSwizzle::checkSize(int index) const
{
  if (index < 0 || index >= m_size) {
    delete [] m_indices;
    shError( ShSwizzleException(*this, index, m_size) );
  }
}

inline void ShSwizzle::realloc(int newsize) 
{
  if(m_size != newsize) {
    delete [] m_indices;
    m_size = newsize;
    m_indices = new int[m_size];
  }
}

inline bool ShSwizzle::identity() const
{
  if (m_size != m_srcSize) return false;
  for (int i = 0; i < m_size; i++) {
    if (m_indices[i] != i) return false;
  }
  return true;
}

inline bool ShSwizzle::operator==(const ShSwizzle& other) const
{
  if (m_srcSize != other.m_srcSize) return false;
  if (m_size != other.m_size) return false;
  return memcmp(m_indices, other.m_indices, sizeof(int)*m_size) == 0;
}

} // namespace SH

#endif
