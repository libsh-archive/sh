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
#ifndef SHPROGRAMSET_HPP
#define SHPROGRAMSET_HPP

#include <list>
#include <map>
#include "ProgramNode.hpp"

namespace SH {

class Backend;
class BackendSet;
class Program;

class
SH_DLLEXPORT ProgramSet : public RefCountable {
public:
  ProgramSet(); // empty set
  explicit ProgramSet(const Program& a);
  ProgramSet(const Program& a, const Program& b);

  typedef std::list<ProgramNodePtr> NodeList;
  typedef NodeList::iterator iterator;
  typedef NodeList::const_iterator const_iterator;

  // Program sets are immutable
  NodeList::const_iterator begin() const { return m_nodes.begin(); }
  NodeList::const_iterator end() const { return m_nodes.end(); }

  Pointer<BackendSet> backend_set(const Pointer<Backend>&) const;
  
private:
  NodeList m_nodes;

  typedef std::map<Pointer<Backend>, Pointer<BackendSet> > BackendMap;
  mutable BackendMap m_backend_sets;

  // NOT IMPLEMENTED (but maybe they should be)
  ProgramSet(const ProgramSet& other);
  ProgramSet& operator=(const ProgramSet& other);
};

typedef Pointer<ProgramSet> ProgramSetPtr;
typedef Pointer<const ProgramSet> ProgramSetCPtr;

}

#endif
