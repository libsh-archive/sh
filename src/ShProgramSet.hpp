// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHPROGRAMSET_HPP
#define SHPROGRAMSET_HPP

#include <list>
#include <map>
#include "ShProgramNode.hpp"

namespace SH {

class ShBackend;
class ShBackendSet;
class ShProgram;

class
SH_DLLEXPORT ShProgramSet : public ShRefCountable {
public:
  ShProgramSet(); // empty set
  explicit ShProgramSet(const ShProgram& a);
  ShProgramSet(const ShProgram& a, const ShProgram& b);

  typedef std::list<ShProgramNodePtr> NodeList;
  typedef NodeList::iterator iterator;
  typedef NodeList::const_iterator const_iterator;

  // Program sets are immutable
  NodeList::const_iterator begin() const { return m_nodes.begin(); }
  NodeList::const_iterator end() const { return m_nodes.end(); }

  ShPointer<ShBackendSet> backend_set(const ShPointer<ShBackend>&) const;
  
private:
  NodeList m_nodes;

  typedef std::map<ShPointer<ShBackend>, ShPointer<ShBackendSet> > BackendMap;
  mutable BackendMap m_backend_sets;

  // NOT IMPLEMENTED (but maybe they should be)
  ShProgramSet(const ShProgramSet& other);
  ShProgramSet& operator=(const ShProgramSet& other);
};

typedef ShPointer<ShProgramSet> ShProgramSetPtr;
typedef ShPointer<const ShProgramSet> ShProgramSetCPtr;

}

#endif
