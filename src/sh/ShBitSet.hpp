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
#ifndef SHBITSET_HPP
#define SHBITSET_HPP

#include <cstddef>
#include <iosfwd>
#include <string.h>
#include "ShDllExport.hpp"

namespace SH {

/* A reference to a single bit in a set.
 * @see ShBitSet
 */
class
SH_DLLEXPORT ShBitRef {
public:
  operator bool() const
  {
    return (((*m_byte) & m_mask) == m_mask);
  }

  ShBitRef& operator=(bool b)
  {
    if (b) {
      *m_byte |= m_mask;
    } else {
      *m_byte &= ~m_mask;
    }
    return (*this);
  }

private:
  ShBitRef(unsigned int* byte, unsigned int mask) 
    : m_byte(byte), m_mask(mask) {}

  unsigned int* m_byte;
  unsigned int m_mask;

  ShBitRef(const ShBitRef& other);
  ShBitRef& operator=(const ShBitRef& other);

  friend class ShBitSet;
};

/** A bitset.
 * The best of both worlds from std::vector<bool> and
 * std::bitset. Run-time sized bitset with all the bit operations one
 * may need.
 */
class
SH_DLLEXPORT ShBitSet {
public:
  /** Construct a bitset with size bits.
   * By default all bits are set to 0.
   * @arg size number of bits in the bitset.
   */
  ShBitSet() : m_size(0), m_data(0) {}
  ShBitSet(std::size_t size, bool init_value = false) : m_size(size)
  {
    std::size_t ws = wordsize(m_size);
    m_data = new unsigned int[ws];
    if (init_value) {
      memset(m_data, 0xff, ws * WORD_SIZE());
    } else {
      memset(m_data, 0x00, ws * WORD_SIZE());
    }
  }
  ShBitSet(const ShBitSet& other) : m_size(other.m_size)
  {
    std::size_t ws = wordsize(m_size);
    m_data = new unsigned int[ws];
    memcpy(m_data, other.m_data, ws * WORD_SIZE());
  }
  
  ~ShBitSet() { delete [] m_data; }


  ShBitSet& operator=(const ShBitSet& other);
  ShBitSet& operator&=(const ShBitSet& other);
  ShBitSet& operator|=(const ShBitSet& other);
  ShBitSet& operator^=(const ShBitSet& other);

  ShBitSet operator&(const ShBitSet& other) const;
  ShBitSet operator|(const ShBitSet& other) const;
  ShBitSet operator^(const ShBitSet& other) const;
  
  ShBitSet operator~() const;

  bool operator==(const ShBitSet& other) const;
  bool operator!=(const ShBitSet& other) const { return !(*this == other); }

  // Return true iff all entries are 1
  bool full() const;
  // Return true iff all entries are 0
  bool empty() const;
  
  std::size_t size() const { return m_size; }


  bool operator[](std::size_t i) const
  {
    return ((m_data[i / WORD_SIZE()] & (1 << (i % WORD_SIZE()))) != 0);
  }
  ShBitRef operator[](std::size_t i)
  {
    return ShBitRef(m_data + (i / WORD_SIZE()), 1 << (i % WORD_SIZE()));
  }


private:
  inline std::size_t WORD_SIZE() const { return sizeof(unsigned int); }
  inline std::size_t wordsize(std::size_t size) const 
  {
    return size / WORD_SIZE() + (size % WORD_SIZE() ? 1 : 0);
  }
  inline std::size_t fullwordsize(std::size_t size) const
  {
    return size / WORD_SIZE();
  }


private:
  std::size_t m_size;
  unsigned int* m_data;
};

SH_DLLEXPORT
std::ostream& operator<<(std::ostream& out, const ShBitSet& bitset);

}

#endif
