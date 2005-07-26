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
#include "ShProgramSet.hpp"
#include "ShBackend.hpp"

namespace SH {

ShProgramSet::ShProgramSet()
{
}

ShProgramSet::ShProgramSet(const ShProgram& a)
{
  m_nodes.push_back(shref_const_cast<ShProgramNode>(a.node()));
}

ShProgramSet::ShProgramSet(const ShProgram& a, const ShProgram& b)
{
  m_nodes.push_back(shref_const_cast<ShProgramNode>(a.node()));
  m_nodes.push_back(shref_const_cast<ShProgramNode>(b.node()));
}

ShPointer<ShBackendSet> ShProgramSet::backend_set(const ShPointer<ShBackend>& backend) const
{
  if (!backend) return 0;

  if (m_backend_sets.find(backend) == m_backend_sets.end()) {
    ShBackendSetPtr bs = backend->generate_set(*this);
    if (!bs) return 0;
    m_backend_sets[backend] = bs;
  }
  
  return m_backend_sets[backend];
}

}
