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
      }
    } else {
      m_backendCode->freeRegister(I->var);
    }
  }
}

}

