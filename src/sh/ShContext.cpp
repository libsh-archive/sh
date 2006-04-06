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
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"
#include "binreloc.h"

namespace SH {

ShContext* ShContext::m_instance = 0;

ShContext* ShContext::current()
{
  if (!m_instance) {
    m_instance = new ShContext();

    // must be done this way since
    // init_types requires a ShContext object, 
    ShTypeInfo::init();

    // Enable binary relocation (from autopackage)
    BrInitError error;
    int init_passed = br_init_lib(&error);
    if (!init_passed && error != BR_INIT_ERROR_DISABLED) {
      SH_DEBUG_WARN("BinReloc failed to initialize (error code " 
        << error << "). Will fallback to hardcoded default paths.");
    }
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

void ShContext::disable_optimization(const std::string& name)
{
  m_disabled_optimizations.insert(name);
}

void ShContext::enable_optimization(const std::string& name)
{
  m_disabled_optimizations.erase(name);
}

bool ShContext::optimization_disabled(const std::string& name) const
{
  return m_disabled_optimizations.find(name) != m_disabled_optimizations.end();
}

bool ShContext::is_bound(const std::string& target)
{
  return bound_program(target);
}

ShProgramNodePtr ShContext::bound_program(const std::string& target)
{
  // Look for an exact match first
  if (m_bound.find(target) != m_bound.end()) return m_bound[target].node();

  // Look for a derived target
  std::list<std::string> derived_targets = ShBackend::derived_targets(target);
  for (std::list<std::string>::const_iterator i = derived_targets.begin(); 
       i != derived_targets.end(); i++) {
    if (m_bound.find(*i) != m_bound.end()) return m_bound[*i].node();
  }

  return 0;
}

void ShContext::set_binding(const std::string& unit, const ShProgram& program)
{
  if (!program.node()) {
    m_bound.erase(unit);
  } else {
    m_bound[unit] = program;
  }
}

void ShContext::unset_binding(const std::string& unit)
{
  m_bound.erase(unit);
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

bool shIsBound(const std::string& target)
{
  return ShContext::current()->is_bound(target);
}

ShProgramNodePtr shBound(const std::string& target)
{
  return ShContext::current()->bound_program(target);
}

} // namespace SH
