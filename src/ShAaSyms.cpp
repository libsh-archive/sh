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

#include <map>
#include <iostream>
#include <algorithm>
#include "ShAaSyms.hpp"


namespace SH {

ShAaIndexSet& ShAaIndexSet::operator|=(const ShAaIndexSet& other) 
{
  m_idx.insert(other.begin(), other.end());
  return *this;
}

ShAaIndexSet& ShAaIndexSet::operator|=(int idx)
{
  m_idx.insert(idx);
  return *this;
}

int ShAaIndexSet::first() const
{
  SH_DEBUG_ASSERT(!m_idx.empty());
  return *m_idx.begin();
}

ShAaIndexSet operator|(const ShAaIndexSet &a, const ShAaIndexSet &b)
{
  ShAaIndexSet result(a);
  result |= b;
  return result;
}

ShAaIndexSet operator&(const ShAaIndexSet &a, const ShAaIndexSet &b)
{
  ShAaIndexSet result;
  set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(result.m_idx, result.begin())); 
  return result;
}

ShAaIndexSet operator-(const ShAaIndexSet &a, const ShAaIndexSet &b)
{
  ShAaIndexSet result;
  set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(result.m_idx, result.begin())); 
  return result;
}

std::ostream& operator<<(std::ostream& out, const ShAaIndexSet& a) 
{
  out << "{";
  for(ShAaIndexSet::const_iterator I = a.begin(); I != a.end(); ++I) {
    if(I != a.begin()) out << ",";
    out << *I; 
  }
  out << "}";
  return out;
}

ShAaSyms::ShAaSyms()
{}

ShAaSyms::ShAaSyms(int size)
  : m_tidx(size)
{}

ShAaSyms::ShAaSyms(int size, int& cur_index)
  : m_tidx(size)
{
  for(iterator I = m_tidx.begin(); I != m_tidx.end(); ++I) {
    (*I) |= cur_index++; 
  }
}

ShAaSyms::ShAaSyms(const ShSwizzle& swiz, int& cur_index)
  : m_tidx(swiz.size())
{
  std::map<int, int> swizIdxMap;
  iterator I = begin(); 
  for(int i = 0; i < swiz.size(); ++i, ++I) { 
    if(swizIdxMap.find(swiz[i]) != swizIdxMap.end()) {
      *I |= swizIdxMap[swiz[i]]; 
    } else {
      *I |= swizIdxMap[swiz[i]] = cur_index++;
    }
  }
}

ShAaSyms& ShAaSyms::operator|=(const ShAaSyms& other)
{
  SH_DEBUG_ASSERT(size() == other.size());
  iterator T = begin();
  const_iterator O = other.begin();
  for(; T != end(); (*T) |= (*O), ++T, ++O); 
  return *this;
}

void ShAaSyms::merge(const ShSwizzle& swiz, const ShAaSyms& other)
{
  SH_DEBUG_ASSERT(swiz.size() == size());
  for(int i = 0; i < swiz.size(); ++i) {
    m_tidx[i] |= other[swiz[i]];
  }
}

void ShAaSyms::mask_merge(const ShSwizzle& swiz, const ShAaSyms& other)
{
  SH_DEBUG_ASSERT(swiz.size() == other.size());
  for(int i = 0; i < swiz.size(); ++i) {
    SH_DEBUG_ASSERT(swiz[i] < size()); 
    m_tidx[swiz[i]] |= other[i];
  }
}

void ShAaSyms::merge(int i, const ShAaIndexSet& other) 
{
  SH_DEBUG_ASSERT(0 <= i && i < size());
  m_tidx[i] |= other;
}

bool ShAaSyms::empty() const
{
  bool result = true;
  for(const_iterator I = begin(); I != end(); ++I) {
    result &= I->empty();
  }
  return result;
}

ShAaSyms operator|(const ShAaSyms &a, const ShAaSyms &b)
{
  ShAaSyms result(a);
  result |= b;
  return result;
}

ShAaSyms operator&(const ShAaSyms &a, const ShAaSyms &b)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  ShAaSyms result(a.size());

  ShAaSyms::const_iterator A, B;
  ShAaSyms::iterator R;
  for(A = a.begin(), B = b.begin(), R = result.begin(); A != a.end(); ++A, ++B, ++R) {
    (*R) = (*A) & (*B);
  }
  return result;
}

ShAaSyms operator-(const ShAaSyms &a, const ShAaSyms &b)
{
  SH_DEBUG_ASSERT(a.size() == b.size());
  ShAaSyms result(a.size());

  ShAaSyms::const_iterator A, B;
  ShAaSyms::iterator R;
  for(A = a.begin(), B = b.begin(), R = result.begin(); A != a.end(); ++A, ++B, ++R) {
    (*R) = (*A) - (*B);
  }
  return result;
}

std::ostream& operator<<(std::ostream& out, const ShAaSyms& syms) 
{
  out << "(";
  for(ShAaSyms::const_iterator I = syms.begin(); I != syms.end(); ++I) {
    if(I != syms.begin()) out << " ";
    out << *I; 
  }
  out << ")";
  return out;
}

}
