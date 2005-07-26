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
#ifndef SHBITSET_HPP
#define SHBITSET_HPP

#include <cstddef>
#include <iosfwd>
#include "ShDllExport.hpp"

namespace SH {

/* A reference to a single bit in a set.
 * @see ShBitSet
 */
class
SH_DLLEXPORT ShBitRef {
public:
  operator bool() const;
  ShBitRef& operator=(bool b);

private:
  friend class ShBitSet;
  
  ShBitRef(unsigned int* byte, unsigned int mask);

  unsigned int* m_byte;
  unsigned int m_mask;

  ShBitRef(const ShBitRef& other);
  ShBitRef& operator=(const ShBitRef& other);
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
  ShBitSet();
  explicit ShBitSet(std::size_t size);
  ShBitSet(const ShBitSet& other);
  
  ~ShBitSet();

  ShBitSet& operator=(const ShBitSet& other);
  ShBitSet& operator&=(const ShBitSet& other);
  ShBitSet& operator|=(const ShBitSet& other);
  ShBitSet& operator^=(const ShBitSet& other);

  ShBitSet operator&(const ShBitSet& other) const;
  ShBitSet operator|(const ShBitSet& other) const;
  ShBitSet operator^(const ShBitSet& other) const;
  
  ShBitSet operator~() const;

  bool operator==(const ShBitSet& other) const;
  bool operator!=(const ShBitSet& other) const;

  // Return true iff all entries are 1
  bool full() const;
  // Return true iff all entries are 0
  bool empty() const;
  
  std::size_t size() const;

  bool operator[](std::size_t i) const;
  ShBitRef operator[](std::size_t i);

private:
  std::size_t m_size;
  unsigned int* m_data;
};

SH_DLLEXPORT
std::ostream& operator<<(std::ostream& out, const ShBitSet& bitset);

}

#endif
