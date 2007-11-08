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
#include <iostream>
#include "Context.hpp"
#include "Debug.hpp"
#include "TypeInfo.hpp"
#include "BaseTexture.hpp"
#include "binreloc.h"

namespace SH {

Context* Context::m_instance = 0;
Context::ContextVec Context::m_contexts;

Context* Context::current()
{
  if (!m_instance) {
    m_instance = createContext(); 

    // must be done this way since
    // init_types requires a Context object, 
    TypeInfo::init();

#ifndef __APPLE__
    // Enable binary relocation (from autopackage)
    BrInitError error;
    int init_passed = br_init_lib(&error);
    if (!init_passed && error != BR_INIT_ERROR_DISABLED) {
      SH_DEBUG_WARN("BinReloc failed to initialize (error code " 
        << error << "). Will fallback to hardcoded default paths.");
    }
#endif
  }
  return m_instance;
}

Context::Context()
  : m_optimization(2),
    m_throw_errors(true)
{
}


int Context::optimization() const
{
  return m_optimization;
}

void Context::optimization(int level)
{
  m_optimization = level;
}

bool Context::throw_errors() const
{
  return m_throw_errors;
}

void Context::throw_errors(bool on)
{
  m_throw_errors = on;
}

void Context::disable_optimization(const std::string& name)
{
  m_disabled_optimizations.insert(name);
}

void Context::enable_optimization(const std::string& name)
{
  m_disabled_optimizations.erase(name);
}

bool Context::optimization_disabled(const std::string& name) const
{
  return m_disabled_optimizations.find(name) != m_disabled_optimizations.end();
}

void Context::set_flag(const std::string& name, bool value) {
  if(value) { 
    m_flags.erase(name);
  } else if (!value) {
    m_flags.insert(name);
  }
}

bool Context::get_flag(const std::string& name) const{
  return m_flags.find(name) == m_flags.end();
}

bool Context::is_bound(const std::string& target)
{
  return bound_program(target);
}

std::ostream& Context::dump_stats(std::ostream& out) const {
  for(std::map<std::string, float>::const_iterator S = m_stat.begin(); S != m_stat.end(); ++S) {
    out << S->first << ": " << S->second << std::endl;
  }
  return out;
}

ProgramNodePtr Context::bound_program(const std::string& target)
{
  // Look for an exact match first
  if (m_bound.find(target) != m_bound.end()) return m_bound[target].node();

  // Look for a derived target
  std::list<std::string> derived_targets = Backend::derived_targets(target);
  for (std::list<std::string>::const_iterator i = derived_targets.begin(); 
       i != derived_targets.end(); i++) {
    if (m_bound.find(*i) != m_bound.end()) return m_bound[*i].node();
  }

  return 0;
}

void Context::set_binding(const std::string& unit, const Program& program)
{
  if (!program.node()) {
    m_bound.erase(unit);
  } else {
    m_bound[unit] = program;
  }
}

void Context::unset_binding(const std::string& unit)
{
  m_bound.erase(unit);
}

ProgramNodePtr Context::parsing()
{
  if (m_parsing.empty()) return 0;
  return m_parsing.top();
}

void Context::enter(const ProgramNodePtr& program)
{
  m_parsing.push(program);
}

void Context::exit()
{
  SH_DEBUG_ASSERT(!m_parsing.empty());
  m_parsing.pop();
}

bool isBound(const std::string& target)
{
  return Context::current()->is_bound(target);
}

ProgramNodePtr bound(const std::string& target)
{
  return Context::current()->bound_program(target);
}

} // namespace SH
