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
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

ShContext* ShContext::m_instance = 0;

ShContext* ShContext::current()
{
  if (!m_instance) {
    m_instance = new ShContext();

    // must be done this way since
    // init_types requires a ShContext object, 
    shTypeInfoInit();
  }
  return m_instance;
}

ShContext::ShContext()
  : m_optimization(2),
    m_throw_errors(true)
{
}


int ShContext::optimization() const
{
  return m_optimization;
}

void ShContext::optimization(int level)
{
  m_optimization = level;
}

bool ShContext::throw_errors() const
{
  return m_throw_errors;
}

void ShContext::throw_errors(bool on)
{
  m_throw_errors = on;
}

ShContext::BoundProgramMap::iterator ShContext::begin_bound()
{
  return m_bound.begin();
}

ShContext::BoundProgramMap::iterator ShContext::end_bound()
{
  return m_bound.end();
}

void ShContext::set_binding(const std::string& unit, const ShProgram program)
{
  if (!program.node()) {
    m_bound.erase(unit);
  } else {
    m_bound[unit] = program;
  }
}

ShProgramNodePtr ShContext::parsing()
{
  if (m_parsing.empty()) return 0;
  return m_parsing.top();
}

void ShContext::enter(const ShProgramNodePtr& program)
{
  m_parsing.push(program);
}

void ShContext::exit()
{
  SH_DEBUG_ASSERT(!m_parsing.empty());
  m_parsing.pop();
}

int ShContext::type_index(const std::string &typeName) const
{
  TypeNameIndexMap::const_iterator I = m_type_index.find(typeName);
  if(I != m_type_index.end()) {
    return I->second; 
  }
  SH_DEBUG_ASSERT(0);
  // TODO throw some kind of error
  return -1;
}

int ShContext::num_types() const
{
  return m_type_index.size();
}

ShTypeInfoPtr ShContext::type_info(int typeIndex) const
{
  SH_DEBUG_ASSERT(typeIndex > 0 && typeIndex <= (int)m_type_info.size());
  return m_type_info[typeIndex - 1];
}

void ShContext::addTypeInfo(ShTypeInfoPtr typeInfo)
{
  // TODO check that type_name does not already exist
  m_type_info.push_back(typeInfo);
  SH_DEBUG_ASSERT(m_type_index.count(typeInfo->name()) == 0);
  m_type_index[typeInfo->name()] = m_type_info.size();
}

ShBoundIterator shBeginBound()
{
  return ShContext::current()->begin_bound();
}

ShBoundIterator shEndBound()
{
  return ShContext::current()->end_bound();
}

ShTypeInfoPtr shTypeInfo(int type_index)
{
  return ShContext::current()->type_info(type_index);
}

}
