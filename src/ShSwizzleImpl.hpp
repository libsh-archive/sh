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
    m_size(0)
{
#ifndef SH_SWIZZLE_CHAR
  m_indices = 0;
#endif
  // otherwise, we don't do dynamic memory allocation anyway, so nothing to wory
  // about
}

inline ShSwizzle::ShSwizzle(T srcSize)
  : m_srcSize(srcSize),
    m_size(srcSize)
{
  alloc();
  for (T i = 0; i < srcSize; i++) get(i) = i;
}

inline ShSwizzle::ShSwizzle(T srcSize, T i0)
  : m_srcSize(srcSize),
    m_size(1)
{
  alloc();
  checkSrcSize(i0);
  get(0) = i0;
}

inline ShSwizzle::ShSwizzle(const ShSwizzle& other)
  : m_srcSize(other.m_srcSize),
    m_size(other.m_size)
{
  alloc();
  memcpy(getptr(), other.getptr(), sizeof(T)*m_size);
}

inline ShSwizzle::~ShSwizzle()
{
  dealloc();
}

inline ShSwizzle& ShSwizzle::operator=(const ShSwizzle& other)
{
  realloc(other.m_size);
  memcpy(getptr(), other.getptr(), sizeof(T)*m_size);
  m_srcSize = other.m_srcSize;
  return *this;
}

inline ShSwizzle ShSwizzle::operator*(const ShSwizzle& other) const
{
  ShSwizzle result(other.m_size);
  for(T i = 0; i < other.m_size; ++i) result.get(i) = get(other[i]);
  return result;
}

inline ShSwizzle& ShSwizzle::operator*=(const ShSwizzle& other)
{
    (*this) = (*this) * other;
    return (*this);
}

inline ShSwizzle::T ShSwizzle::operator[](T index) const
{
  if (index >= m_size || index < 0) shError( ShSwizzleException(*this, index, m_size) );
  return get(index); 
}

inline void ShSwizzle::checkSrcSize(T index) 
{
  if (index < 0 || index >= m_srcSize) {
    dealloc();
    shError( ShSwizzleException(*this, index, m_srcSize) );
  }
}

inline void ShSwizzle::checkSize(T index) 
{
  if (index < 0 || index >= m_size) {
    dealloc();
    shError( ShSwizzleException(*this, index, m_size) );
  }
}

inline void ShSwizzle::alloc() 
{
#ifdef SH_SWIZZLE_CHAR
  if(m_size > 4) m_indices.ptr = new T[m_size];
#else
  m_indices = new T[m_size];
#endif
}

inline void ShSwizzle::realloc(T newsize) 
{
  if(m_size != newsize) {
    dealloc();
    m_size = newsize;
    alloc();
  }
}

inline void ShSwizzle::dealloc()
{
#ifdef SH_SWIZZLE_CHAR
  if(m_size > 4) delete m_indices.ptr;
#else
  delete [] m_indices;
#endif
}

inline ShSwizzle::T ShSwizzle::get(T index) const
{
  return getptr()[index];
}

inline ShSwizzle::T& ShSwizzle::get(T index) 
{
  return getptr()[index];
}

inline ShSwizzle::T* ShSwizzle::getptr() 
{

#ifdef SH_SWIZZLE_CHAR
  if(m_size <= 4) return m_indices.local;
  else return m_indices.ptr;
#else
  return m_indices;
#endif
}

inline const ShSwizzle::T* ShSwizzle::getptr() const
{
#ifdef SH_SWIZZLE_CHAR
  if(m_size <= 4) return m_indices.local;
  else return m_indices.ptr;
#else
  return m_indices;
#endif
}

inline bool ShSwizzle::identity() const
{
  if (m_size != m_srcSize) return false;
  for (T i = 0; i < m_size; i++) {
    if (get(i) != i) return false;
  }
  return true;
}

inline bool ShSwizzle::operator==(const ShSwizzle& other) const
{
  if (m_srcSize != other.m_srcSize) return false;
  if (m_size != other.m_size) return false;
  return memcmp(getptr(), other.getptr(), sizeof(T)*m_size) == 0;
}

} // namespace SH

#endif
