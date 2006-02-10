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
#include "ShLinearAllocator.hpp"
#include <set>
#include <algorithm>
#include "ShDebug.hpp"

namespace {

struct LifeToken {
  LifeToken(const SH::ShVariableNodePtr& var, int index, bool end)
    : var(var), index(index), end(end) 
  {
  }
  
  SH::ShVariableNodePtr var;
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

}

namespace SH {

ShLinearAllocator::ShLinearAllocator(const ShBackendCodePtr& backendCode)
  : m_backendCode(backendCode)
{
}

void ShLinearAllocator::mark(const ShVariableNodePtr& var, int index)
{
  if (!var) return;
  LifetimeMap::iterator I = m_lifetimes.find(var);

  if (I == m_lifetimes.end()) {
    m_lifetimes[var] = ShLifeTime(var, index);
  } else {
    I->second.mark(index);
  }
}

void ShLinearAllocator::debugDump()
{
#ifdef SH_DEBUG
  for (LifetimeMap::const_iterator I = m_lifetimes.begin(); I != m_lifetimes.end(); ++I) {
    SH_DEBUG_PRINT(I->first->name() << " = {" << I->second.first << ", " << I->second.last << "}");
  }
#endif
}

void ShLinearAllocator::allocate()
{
  std::multiset<LifeToken> temps;

  for (LifetimeMap::const_iterator I = m_lifetimes.begin(); I != m_lifetimes.end(); ++I) {
    temps.insert(LifeToken(I->first, I->second.first, false));
    temps.insert(LifeToken(I->first, I->second.last, true));
  }

  for (std::multiset<LifeToken>::const_iterator I = temps.begin(); I != temps.end(); ++I) {
    if (!I->end) {
      if (!m_backendCode->allocateRegister(I->var)) {
        // TODO: Error
        SH_DEBUG_WARN("Error allocating a register for " << I->var->name());
      }
    } else {
      m_backendCode->freeRegister(I->var);
    }
  }
}

}

