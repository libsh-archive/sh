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

// enable this to turn on all AA debugging
//#define SH_DBG_AA

namespace SH {

/** @file ShAaSyms.hpp
 * Classes used to hold sets of statically assigned affine arithmetic error symbols,
 * for use by the ShAaSymPlacer.
 *
 * @see ShAaSymPlacer.hpp
 */

/** Represents a set of error symbol indices. */
struct ShAaIndexSet {
  typedef int value_type; // @TODO this was stuck in after the fact, and doesn't necessarily make sense
  typedef std::set<int> IndexSet;
  typedef IndexSet::iterator iterator;
  typedef IndexSet::const_iterator const_iterator;


  /* Merges this index set with the other index set */
  ShAaIndexSet& operator|=(const ShAaIndexSet& other);

  /* Merges this index set with the given index */
  ShAaIndexSet& operator|=(int idx);

  /* Returns whether this has any indices in the set */
  bool empty() const { return m_idx.empty(); }

  /* Returns the size of the index set */
  int size() const { return m_idx.size(); }

  /* Clears this index set */
  void clear() { m_idx.clear(); }

  /* Returns the largest error symbol, bombs out with an assertion if no such 
   * symbol exists. */
  int last() const;

  /* Returns the smallest error symbol */
  int first() const;

  /* Returns whether the index set contains a certain error symbol */
  bool contains(int idx) const;

  /* Set Ops (union, intesrection, difference) 
   * @{ */
  friend ShAaIndexSet operator|(const ShAaIndexSet &a, const ShAaIndexSet &b);
  friend ShAaIndexSet operator&(const ShAaIndexSet &a, const ShAaIndexSet &b);
  friend ShAaIndexSet operator-(const ShAaIndexSet &a, const ShAaIndexSet &b);
  friend bool operator!=(const ShAaIndexSet &a, const ShAaIndexSet &b);
  // @}

  friend std::ostream& operator<<(std::ostream& out, const ShAaIndexSet& a); 


  /* Iterators 
   * @{ */
  iterator begin() { return m_idx.begin(); } 
  const_iterator begin() const { return m_idx.begin(); } 
  iterator end() { return m_idx.end(); } 
  const_iterator end() const { return m_idx.end(); } 
  // @}
  
  private:
    IndexSet m_idx;
};


/** Represents the sequence of ShAaIndexSets needed for a given tuple */
struct ShAaSyms {
  typedef std::vector<ShAaIndexSet> TupleIndexSet;
  typedef TupleIndexSet::iterator iterator;
  typedef TupleIndexSet::const_iterator const_iterator;

  TupleIndexSet m_tidx;

  // Generates empty syms.
  ShAaSyms(); 

  // Generates index sets for a tuple with the given size, assigning
  // one unique sym per set. 
  //
  // @param empty - don't assign syms at all
  ShAaSyms(int size, bool empty=true);

  // Generates index sets for a tuple with the size of the swizzle, 
  // assigning one unique sym per swizzle index starting from. 
  ShAaSyms(const ShSwizzle& swiz);

  // Returns size of syms
  int size() const { return m_tidx.size(); } 

  // Resizes 
  void resize(int size) { m_tidx.resize(size); }

  // Resizes 
  void clear() { for(iterator I = begin(); I != end(); ++I) I->clear(); } 

  // Returns a swizzling of this aasyms
  ShAaSyms swizSyms(const ShSwizzle& swiz) const;

  // Returns the index set for a given tuple element 
  // @{
  const ShAaIndexSet& operator[](int i) const { return m_tidx[i]; }
  ShAaIndexSet& operator[](int i) { return m_tidx[i]; }
  // @}
  
  // Returns whether each tuple element contains exacly one noise sym
  bool isSingles() const;

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

  // Picks out the last element of each index set 
  ShAaSyms last() const;

  // Picks out the first element of each index set 
  ShAaSyms first() const;

  // Returns whether all index sets are empty 
  bool empty() const;

  /* Set Ops (union, intesrection, difference) 
   * @{ */
  friend ShAaSyms operator|(const ShAaSyms &a, const ShAaSyms &b);
  friend ShAaSyms operator&(const ShAaSyms &a, const ShAaSyms &b);
  friend ShAaSyms operator-(const ShAaSyms &a, const ShAaSyms &b);
  // @}
  friend std::ostream& operator<<(std::ostream& out, const ShAaSyms& syms);
private:
  static long m_max_index;
};

}

#endif
