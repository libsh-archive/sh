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
#include "DllExport.hpp"

namespace SH {

/* A reference to a single bit in a set.
 * @see BitSet
 */
class
SH_DLLEXPORT BitRef {
public:
  operator bool() const;
  BitRef& operator=(bool b);

private:
  friend class BitSet;
  
  BitRef(unsigned int* byte, unsigned int mask);

  unsigned int* m_byte;
  unsigned int m_mask;

  BitRef(const BitRef& other);
  BitRef& operator=(const BitRef& other);
};

/** A bitset.
 * The best of both worlds from std::vector<bool> and
 * std::bitset. Run-time sized bitset with all the bit operations one
 * may need.
 */
class
SH_DLLEXPORT BitSet {
public:
  /** Construct a bitset with size bits.
   * By default all bits are set to 0.
   * @arg size number of bits in the bitset.
   */
  BitSet();
  explicit BitSet(std::size_t size);
  BitSet(const BitSet& other);
  
  ~BitSet();

  BitSet& operator=(const BitSet& other);
  BitSet& operator&=(const BitSet& other);
  BitSet& operator|=(const BitSet& other);
  BitSet& operator^=(const BitSet& other);

  BitSet operator&(const BitSet& other) const;
  BitSet operator|(const BitSet& other) const;
  BitSet operator^(const BitSet& other) const;
  
  BitSet operator~() const;

  bool operator==(const BitSet& other) const;
  bool operator!=(const BitSet& other) const;

  // Return true iff all entries are 1
  bool full() const;
  // Return true iff all entries are 0
  bool empty() const;
  
  std::size_t size() const;

  bool operator[](std::size_t i) const;
  BitRef operator[](std::size_t i);

private:
  std::size_t m_size;
  unsigned int* m_data;
};

SH_DLLEXPORT
std::ostream& operator<<(std::ostream& out, const BitSet& bitset);

}

#endif
