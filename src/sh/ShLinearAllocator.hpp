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
#ifndef SHLINEARALLOCATOR_HPP
#define SHLINEARALLOCATOR_HPP

#include <map>
#include <set>
#include <algorithm>
#include "ShDebug.hpp"
#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShBackend.hpp"

namespace SH {

struct ShLifeTime {
  ShLifeTime()
  {
  }
  
  ShLifeTime(void *var, int first)
    : var(var), first(first), last(first)
  {
  }
  
  void *var;
  int first, last;

  void mark(int index)
  {
    if (first > index) first = index;
    if (last < index) last = index;
  }
  
  bool operator<(const ShLifeTime& other) const
  {
    return first < other.first;
  }
};

struct LifeToken {
  LifeToken(void *var, int index, bool end)
    : var(var), index(index), end(end) 
  {
  }
  
  void *var;
  int index;
  bool end;

  /**
   * Order by index, then end.
   * Strict weak ordering:
   * if (a<b) -> !(b<a).
   */
  bool operator<(const LifeToken& other) const
  {
    if (index == other.index) {
      return !end && other.end;
    }
    return index < other.index;
  }
};

/** A simple, basic-block based linear register allocator.
 */
template<typename T>
class ShLinearAllocatorBase {
public:
  typedef ShPointer<T> TPtr;

  ShLinearAllocatorBase(const ShBackendCodePtr& backendCode)
    : m_backendCode(backendCode)
  {
  }
  
  // Mark that a variable is alive at a given index.
  void mark(const TPtr& var, int index)
  {
    if (!var) return;
    LifetimeMap::iterator I = m_lifetimes.find(var.object());

    if (I == m_lifetimes.end()) {
      m_lifetimes[var.object()] = ShLifeTime(var.object(), index);
    } else {
      I->second.mark(index);
    }
  }

  // Dump the life times to stderr
  void debugDump()
  {
#ifdef SH_DEBUG
    for (LifetimeMap::const_iterator I = m_lifetimes.begin(); I != m_lifetimes.end(); ++I) {
      SH_DEBUG_PRINT(I->first << " = {" << I->second.first << ", " << I->second.last << "}");
    }
#endif
  }
  
  // Calls back the backend with register allocation/deallocation requests.
  void allocate()
  {
    std::multiset<LifeToken> temps;

    for (LifetimeMap::const_iterator I = m_lifetimes.begin(); I != m_lifetimes.end(); ++I) {
      temps.insert(LifeToken(I->first, I->second.first, false));
      temps.insert(LifeToken(I->first, I->second.last, true));
    }

    for (std::multiset<LifeToken>::const_iterator I = temps.begin(); I != temps.end(); ++I) {
      if (!I->end) {
        if (!m_backendCode->allocateRegister(TPtr(reinterpret_cast<T *>(I->var)))) {
          // TODO: Error
          SH_DEBUG_WARN("Error allocating a register for " << I->var);
        }
      } else {
        m_backendCode->freeRegister(TPtr(reinterpret_cast<T *>(I->var)));
      }
    }
  }

private:
  ShBackendCodePtr m_backendCode;
  typedef std::map<void *, ShLifeTime> LifetimeMap;
  LifetimeMap m_lifetimes;
};

typedef ShLinearAllocatorBase<ShVariableNode> ShLinearAllocator;

}

#endif
