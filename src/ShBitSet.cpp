// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#include <cstring>
#include <iostream>
#include "ShBitSet.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"

#define WORD_SIZE sizeof(unsigned int)

namespace {

std::size_t wordsize(std::size_t size)
{
  return size / WORD_SIZE + (size % WORD_SIZE ? 1 : 0);
}

std::size_t fullwordsize(std::size_t size)
{
  return size / WORD_SIZE;
}

}

namespace SH {

ShBitRef::operator bool() const
{
  return (((*m_byte) & m_mask) == m_mask);
}

ShBitRef& ShBitRef::operator=(bool b)
{
  if (b) {
    *m_byte |= m_mask;
  } else {
    *m_byte &= ~m_mask;
  }

  return (*this);
}

ShBitRef::ShBitRef(unsigned int* byte, unsigned int mask)
  : m_byte(byte), m_mask(mask)
{
}

ShBitSet::ShBitSet()
  : m_size(0), m_data(0)
{
}

ShBitSet::ShBitSet(std::size_t size)
  : m_size(size), m_data(new unsigned int[wordsize(m_size)])
{
  for (std::size_t i = 0; i < wordsize(m_size); i++) {
    m_data[i] = 0;
  }
}

ShBitSet::ShBitSet(const ShBitSet& other)
  : m_size(other.m_size), m_data(new unsigned int[wordsize(m_size)])
{
  memcpy(m_data, other.m_data, wordsize(m_size) * WORD_SIZE);
}

ShBitSet::~ShBitSet()
{
  delete [] m_data;
}

ShBitSet& ShBitSet::operator=(const ShBitSet& other)
{
  if (&other == this) return *this;
  
  delete [] m_data;
  m_size = other.m_size;
  m_data = new unsigned int[wordsize(m_size)];
  memcpy(m_data, other.m_data, wordsize(m_size) * WORD_SIZE);

  return (*this);
}

ShBitSet& ShBitSet::operator&=(const ShBitSet& other)
{
  if (m_size != other.m_size) shError( ShException( "ShBitSet operands of &= must be the same size." ) );
  for (std::size_t i = 0; i < wordsize(m_size); i++)
    m_data[i] &= other.m_data[i];
  return *this;
}

ShBitSet& ShBitSet::operator|=(const ShBitSet& other)
{
  if (m_size != other.m_size) shError( ShException( "ShBitSet operands of |= must be the same size." ) );
  for (std::size_t i = 0; i < wordsize(m_size); i++)
    m_data[i] |= other.m_data[i];
  return *this;
}

ShBitSet& ShBitSet::operator^=(const ShBitSet& other)
{
  if (m_size != other.m_size) shError( ShException( "ShBitSet operands of ^= must be the same size." ) );
  for (std::size_t i = 0; i < wordsize(m_size); i++)
    m_data[i] ^= other.m_data[i];
  return *this;
}

ShBitSet ShBitSet::operator&(const ShBitSet& other) const
{
  ShBitSet ret(*this);
  ret &= other;
  return ret;
}

ShBitSet ShBitSet::operator|(const ShBitSet& other) const
{
  ShBitSet ret(*this);
  ret |= other;
  return ret;
}

ShBitSet ShBitSet::operator^(const ShBitSet& other) const
{
  ShBitSet ret(*this);
  ret ^= other;
  return ret;
}

ShBitSet ShBitSet::operator~() const
{
  ShBitSet ret(m_size);
  for (std::size_t i = 0; i < wordsize(m_size); i++)
    ret.m_data[i] = ~m_data[i];
  return ret;
}

bool ShBitSet::operator==(const ShBitSet& other) const
{
  if (m_size != other.m_size) return false;
  for (unsigned int i = 0; i < fullwordsize(m_size); i++) {
    if (m_data[i] != other.m_data[i]) return false;
  }
  if (m_size % WORD_SIZE) {
    unsigned int mask = (1 << ((m_size % WORD_SIZE) + 1)) - 1;
    return ((m_data[m_size / WORD_SIZE] & mask) == (other.m_data[m_size / WORD_SIZE] & mask));
  }
  return true;
}

bool ShBitSet::operator!=(const ShBitSet& other) const
{
  return !(*this == other);
}

std::size_t ShBitSet::size() const
{
  return m_size;
}

bool ShBitSet::full() const
{
  for (std::size_t i = 0; i < fullwordsize(m_size); i++) {
    if (~m_data[i]) return false;
  }
  if (m_size % WORD_SIZE) {
    unsigned int mask = (1 << ((m_size % WORD_SIZE) + 1)) - 1;
    return (m_data[m_size / WORD_SIZE] & mask) == mask;
  }
  
  return true;
}

bool ShBitSet::empty() const
{
  for (std::size_t i = 0; i < wordsize(m_size); i++) if (m_data[i]) return false;
  return true;
}

bool ShBitSet::operator[](std::size_t i) const
{
  return ((m_data[i / WORD_SIZE] & (1 << (i % WORD_SIZE))) != 0);
}

ShBitRef ShBitSet::operator[](std::size_t i)
{
  return ShBitRef(m_data + (i / WORD_SIZE), 1 << (i % WORD_SIZE));
}

std::ostream& operator<<(std::ostream& out, const ShBitSet& bitset)
{
  for (std::size_t i = 0; i < bitset.size(); i++) {
    out << (bitset[i] ? '1' : '0');
  }
  return out;
}

}
