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

#ifndef SHBITSET_HPP
#define SHBITSET_HPP

#include <cstddef>
#include <iosfwd>

namespace SH {

class ShBitRef {
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
class ShBitSet {
public:
  /** Construct a bitset with size bits.
   * By default all bits are set to 0.
   * @arg size number of bits in the bitset.
   */
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

  std::size_t size() const;

  bool operator[](std::size_t i) const;
  ShBitRef operator[](std::size_t i);

private:
  std::size_t m_size;
  unsigned int* m_data;
};

std::ostream& operator<<(std::ostream& out, const ShBitSet& bitset);

}

#endif
