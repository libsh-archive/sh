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

  bool operator<(const LifeToken& other) const
  {
    if (index == other.index) return !end;
    return index < other.index;
  }
};

}

namespace SH {

ShLinearAllocator::ShLinearAllocator(ShBackendCodePtr backendCode)
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
  std::set<LifeToken> starters;
  std::set<LifeToken> enders;

  for (LifetimeMap::const_iterator I = m_lifetimes.begin(); I != m_lifetimes.end(); ++I) {
    starters.insert(LifeToken(I->first, I->second.first, false));
    enders.insert(LifeToken(I->first, I->second.last, true));
  }

  std::list<LifeToken> tokens;

  std::back_insert_iterator< std::list<LifeToken> > bii(tokens);
  
  std::set_union(starters.begin(), starters.end(),
                 enders.begin(), enders.end(),
                 bii);

  for (std::list<LifeToken>::const_iterator I = tokens.begin(); I != tokens.end(); ++I) {
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

