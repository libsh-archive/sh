#ifndef SHLINEARALLOCATOR_HPP
#define SHLINEARALLOCATOR_HPP

// TODO: This should probably move to backends/ or something.

#include <map>
#include "ShVariableNode.hpp"
#include "ShBackend.hpp"

namespace SH {

struct ShLifeTime {
  ShLifeTime()
  {
  }
  
  ShLifeTime(const ShVariableNodePtr& var, int first)
    : var(var), first(first), last(first)
  {
  }
  
  SH::ShVariableNodePtr var;
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

/** A simple, basic-block based linear register allocator.
 */
class ShLinearAllocator {
public:
  ShLinearAllocator(ShBackendCodePtr backendCode);
  
  // Mark that a variable is alive at a given index.
  void mark(const ShVariableNodePtr& var, int index);

  // Dump the life times to stderr
  void debugDump();
  
  // Calls back the backend with register allocation/deallocation requests.
  void allocate();

private:
  ShBackendCodePtr m_backendCode;
  typedef std::map<ShVariableNodePtr, ShLifeTime> LifetimeMap;
  LifetimeMap m_lifetimes;
};

};

#endif
