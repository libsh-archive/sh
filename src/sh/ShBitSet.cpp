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

#include <cstring>
#include <iostream>
#include "ShBitSet.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"



namespace SH {

ShBitSet& ShBitSet::operator=(const ShBitSet& other)
{
  if (&other == this) return *this;
  
  delete [] m_data;
  m_size = other.m_size;
  m_data = new unsigned int[wordsize(m_size)];
  memcpy(m_data, other.m_data, wordsize(m_size) * WORD_SIZE());

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
  if (m_size % WORD_SIZE()) {
    unsigned int mask = (1 << ((m_size % WORD_SIZE()) + 1)) - 1;
    return ((m_data[m_size / WORD_SIZE()] & mask) == (other.m_data[m_size / WORD_SIZE()] & mask));
  }
  return true;
}



bool ShBitSet::full() const
{
  for (std::size_t i = 0; i < fullwordsize(m_size); i++) {
    if (~m_data[i]) return false;
  }
  if (m_size % WORD_SIZE()) {
    unsigned int mask = (1 << ((m_size % WORD_SIZE()) + 1)) - 1;
    return (m_data[m_size / WORD_SIZE()] & mask) == mask;
  }
  
  return true;
}

bool ShBitSet::empty() const
{
  for (std::size_t i = 0; i < wordsize(m_size); i++) if (m_data[i]) return false;
  return true;
}


std::ostream& operator<<(std::ostream& out, const ShBitSet& bitset)
{
  for (std::size_t i = 0; i < bitset.size(); i++) {
    out << (bitset[i] ? '1' : '0');
  }
  return out;
}

}
