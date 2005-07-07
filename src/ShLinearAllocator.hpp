// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShBackend.hpp"

namespace SH {

struct
SH_DLLEXPORT ShLifeTime {
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
class
SH_DLLEXPORT ShLinearAllocator {
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

}

#endif
