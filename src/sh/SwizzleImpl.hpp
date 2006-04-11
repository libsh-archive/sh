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
#ifndef SHSWIZZLEIMPL_HPP
#define SHSWIZZLEIMPL_HPP
#include <iostream>
#include "Swizzle.hpp"
#include "Error.hpp"
#include "Debug.hpp"

namespace SH {

inline Swizzle::Swizzle()
  : m_srcSize(0),
    m_size(0)
{
  alloc();
}

inline Swizzle::Swizzle(int srcSize)
  : m_srcSize(srcSize),
    m_size(srcSize)
{
  if(alloc()) {
    for(int i = 0; i < m_size; ++i) m_index.ptr[i] = i;
  } 
}

inline Swizzle::Swizzle(int srcSize, int i0)
  : m_srcSize(srcSize),
    m_size(1)
{
  checkSrcSize(i0);
  if(alloc()) {
    m_index.ptr[0] = i0;
  } else {
    m_index.local[0] = (unsigned char)i0;
  }
}

inline Swizzle::Swizzle(int srcSize, int i0, int i1)
  : m_srcSize(srcSize),
    m_size(2)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  if(alloc()) {
    m_index.ptr[0] = i0;
    m_index.ptr[1] = i1;
  } else {
    m_index.local[0] = (unsigned char)i0;
    m_index.local[1] = (unsigned char)i1;
  }
}

inline Swizzle::Swizzle(int srcSize, int i0, int i1, int i2)
  : m_srcSize(srcSize),
    m_size(3)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  checkSrcSize(i2);
  if(alloc()) {
    m_index.ptr[0] = i0;
    m_index.ptr[1] = i1;
    m_index.ptr[2] = i2;
  } else {
    m_index.local[0] = (unsigned char)i0;
    m_index.local[1] = (unsigned char)i1;
    m_index.local[2] = (unsigned char)i2;
  }
}

inline Swizzle::Swizzle(int srcSize, int i0, int i1, int i2, int i3)
  : m_srcSize(srcSize),
    m_size(4)
{
  checkSrcSize(i0);
  checkSrcSize(i1);
  checkSrcSize(i2);
  checkSrcSize(i3);
  if(alloc()) {
    m_index.ptr[0] = i0;
    m_index.ptr[1] = i1;
    m_index.ptr[2] = i2;
    m_index.ptr[3] = i3;
  } else {
    m_index.local[0] = (unsigned char)i0;
    m_index.local[1] = (unsigned char)i1;
    m_index.local[2] = (unsigned char)i2;
    m_index.local[3] = (unsigned char)i3;
  }
}

inline Swizzle::Swizzle(int srcSize, int size, int* indices)
  : m_srcSize(srcSize),
    m_size(size)
{
  int i;
  for (i = 0; i < size; i++) checkSrcSize(indices[i]);
  if(alloc()) {
    for (i = 0; i < size; i++) m_index.ptr[i] = indices[i];
  } else {
    for (i = 0; i < size; i++) m_index.local[i] = (unsigned char)indices[i];
  }
}


inline Swizzle::Swizzle(const Swizzle& other)
  : m_srcSize(other.m_srcSize),
    m_size(other.m_size)
{
  copy(other, !alloc());
}

inline Swizzle::~Swizzle()
{
  dealloc();
}

inline Swizzle& Swizzle::operator=(const Swizzle& other)
{
  if(this == &other) return *this;

  if(m_size != other.m_size || m_srcSize != other.m_srcSize) {
    dealloc();
    m_size = other.m_size;
    m_srcSize = other.m_srcSize;
    alloc();
  } 
  copy(other, local());
  return *this;
}

inline Swizzle& Swizzle::operator*=(const Swizzle& other)  
{
  (*this) = (*this) * other;
  return (*this);
}

inline Swizzle Swizzle::operator*(const Swizzle& other) const
{
  Swizzle result;
  result.m_size = other.m_size;
  result.m_srcSize = m_srcSize;
  bool resultLocal = true; 
  if(m_srcSize >= 256 || other.m_size > 4) { // result must go ptr 
    result.alloc();
    resultLocal = false;
  } 

  const bool isLocal = local();
  for (int i = 0; i < other.m_size; i++) {
    int oi = other[i];
    if (oi >= m_size) error( SwizzleException(*this, oi, size()) );
    int index = isLocal ? m_index.local[oi] : m_index.ptr[oi];
    if(resultLocal) result.m_index.local[i] = index;
    else result.m_index.ptr[i] = index;
  }
  return result;
}

inline int Swizzle::operator[](int index) const
{
  if (index >= m_size || index < 0) error( SwizzleException(*this, index, m_size) );
  if(local()) return m_index.local[index];
  return m_index.ptr[index];
}

inline void Swizzle::copy(const Swizzle &other, bool islocal) 
{
  if(islocal) {
    m_index.intval = other.m_index.intval;
  } else {
    memcpy(m_index.ptr, other.m_index.ptr, sizeof(int)*m_size);
  }
}

inline void Swizzle::checkSrcSize(int index) 
{
  if (index < 0 || index >= m_srcSize) {
    error( SwizzleException(*this, index, m_srcSize) );
  }
}

inline bool Swizzle::alloc()
{
  if(local()) {
    m_index.intval = idswiz(); 
    return false; 
  } 
  m_index.ptr = new int[m_size];
  return true;
}

inline void Swizzle::dealloc()
{
  if(!local()) delete [] m_index.ptr;
}

inline bool Swizzle::local() const
{
  return (m_srcSize < 256 && m_size <= 4); 
}

inline int Swizzle::idswiz() const
{
// @todo type detect endianess correctly
// power pc's are not the only big endian machines...
#if defined(__PPC__) || defined(__BIG_ENDIAN__)
  return 0x00010203;
#else
  return 0x03020100;
#endif
}

inline bool Swizzle::identity() const
{
  if (m_size != m_srcSize) return false;
  if (local()) {
    // @todo type this is probably not portable...
    return m_index.intval == idswiz(); 
  } 
  for(int i = 0; i < m_size; ++i) {
    if(m_index.ptr[i] != i) return false;
  }
  return true; 
}

inline bool Swizzle::operator==(const Swizzle& other) const
{
  if (m_srcSize != other.m_srcSize) return false;
  if (m_size != other.m_size) return false;
  if (local()) return m_index.intval == other.m_index.intval;
  return memcmp(m_index.ptr, other.m_index.ptr, sizeof(int)*m_size) == 0;
}

inline bool Swizzle::operator<(const Swizzle& other) const
{
  if (m_srcSize < other.m_srcSize) return true;
  if (m_srcSize > other.m_srcSize) return false;
  if (m_size < other.m_size) return true;
  if (m_size > other.m_size) return false;
  if (local()) {
    return m_index.intval < other.m_index.intval;
  }
  return memcmp(m_index.ptr, other.m_index.ptr, sizeof(int)*m_size) < 0;
}

} // namespace SH

#endif
