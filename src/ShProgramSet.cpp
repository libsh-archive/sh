// Sh: A GPU metaprogramming language.
//
// Copyright 2005 Serious Hack Inc.
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
