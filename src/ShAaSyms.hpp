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
#ifndef SHAASYMS_HPP
#define SHAASYMS_HPP

#include <set>
#include <vector>
#include <iosfwd>
#include "ShSwizzle.hpp"
#include "ShDllExport.hpp"

namespace SH {

/** @file ShAaSyms.hpp
 * Classes used to hold sets of statically assigned affine arithmetic error symbols,
 * for use by the ShAaSymPlacer.
 *
 * @see ShAaSymPlacer.hpp
 */

/** Represents a set of error symbol indices. */
struct ShAaIndexSet {
  typedef std::set<int> IndexSet;
  typedef IndexSet::iterator iterator;
  typedef IndexSet::const_iterator const_iterator;

  IndexSet m_idx;

  /* Merges this index set with the other index set */
  ShAaIndexSet& operator|=(const ShAaIndexSet& other);

  /* Merges this index set with the given index */
  ShAaIndexSet& operator|=(int idx);

  /* Returns whether this has any indices in the set */
  bool empty() const { return m_idx.empty(); }

  /* Returns the size of the index set */
  int size() const { return m_idx.size(); }

  /* Returns the smallest error symbol, bombs out with an assertion if no such 
   * symbol exists. */
  int first() const;

  /* Set Ops (union, intesrection, difference) 
   * @{ */
  friend ShAaIndexSet operator|(const ShAaIndexSet &a, const ShAaIndexSet &b);
  friend ShAaIndexSet operator&(const ShAaIndexSet &a, const ShAaIndexSet &b);
  friend ShAaIndexSet operator-(const ShAaIndexSet &a, const ShAaIndexSet &b);
  // @}

  friend std::ostream& operator<<(std::ostream& out, const ShAaIndexSet& a); 


  /* Iterators 
   * @{ */
  iterator begin() { return m_idx.begin(); } 
  const_iterator begin() const { return m_idx.begin(); } 
  iterator end() { return m_idx.end(); } 
  const_iterator end() const { return m_idx.end(); } 
  // @}
};


/** Represents the sequence of ShAaIndexSets needed for a given tuple */
struct ShAaSyms {
  typedef std::vector<ShAaIndexSet> TupleIndexSet;
  typedef TupleIndexSet::iterator iterator;
  typedef TupleIndexSet::const_iterator const_iterator;

  TupleIndexSet m_tidx;

  // Generates empty syms.
  ShAaSyms(); 

  // Generates empty index sets for a tuple with the given size. 
  ShAaSyms(int size);

  // Generates index sets for a tuple with the given size, assigning
  // one unique sym per set starting from cur_index.
  ShAaSyms(int size, int& cur_index);

  // Generates index sets for a tuple with the size of the swizzle, 
  // assigning one unique sym per swizzle index starting from cur_index.
  ShAaSyms(const ShSwizzle& swiz, int& cur_index);

  // Returns size of syms
  int size() const { return m_tidx.size(); } 

  // Resizes 
  void resize(int size) { m_tidx.resize(size); }

  // Returns the index set for a given tuple element 
  // @{
  const ShAaIndexSet& operator[](int i) const { return m_tidx[i]; }
  ShAaIndexSet& operator[](int i) { return m_tidx[i]; }
  // @}

  // Iterators for the sequence of index sets
  // @{
  iterator begin() { return m_tidx.begin(); }
  const_iterator begin() const { return m_tidx.begin(); }
  iterator end() { return m_tidx.end(); }
  const_iterator end() const { return m_tidx.end(); }
  // @}

  // Componentwise unions idx[i] with other.idx[i]
  ShAaSyms& operator|=(const ShAaSyms& other);

  // applies swizzle tuple-wise to other before merging with this
  void merge(const ShSwizzle& swiz, const ShAaSyms& other);

  // applies swizzle tuple-wise to this before merging with other 
  void mask_merge(const ShSwizzle& swiz, const ShAaSyms& other);

  // Unions idx[i] with other
  void merge(int i, const ShAaIndexSet& other);

  // Returns whether all index sets are empty 
  bool empty() const;

  /* Set Ops (union, intesrection, difference) 
   * @{ */
  friend ShAaSyms operator|(const ShAaSyms &a, const ShAaSyms &b);
  friend ShAaSyms operator&(const ShAaSyms &a, const ShAaSyms &b);
  friend ShAaSyms operator-(const ShAaSyms &a, const ShAaSyms &b);
  // @}
  friend std::ostream& operator<<(std::ostream& out, const ShAaSyms& syms);
};

}

#endif
