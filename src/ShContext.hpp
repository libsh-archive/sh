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
#ifndef SHCONTEXT_HPP
#define SHCONTEXT_HPP

#include <string>
#include <map>
#include "ShDllExport.hpp"
#include "ShProgram.hpp"

namespace SH {

class
SH_DLLEXPORT ShContext {
public:
  static ShContext* current();

  /// 0 means no optimizations. The default level is 2.
  int optimization() const;
  void optimization(int level);

  /// Whether exceptions are being thrown instead of error messages
  /// printed to stdout. The default is to throw exceptions.
  bool throw_errors() const;
  void throw_errors(bool on);

  /// Disable a particular optimization. All optimizations are
  /// enabled by default. Disabling an optimization takes place in
  /// addition to whatever effects the optimization level has.
  void disable_optimization(const std::string& name);
  /// Enable a particular optimization (rather, stop disabling it)
  void enable_optimization(const std::string& name);
  /// Check whether an optimization is disabled
  bool optimization_disabled(const std::string& name) const;

  bool is_bound(const std::string& target);
  ShProgramNodePtr bound_program(const std::string& target);

  
  typedef std::map<std::string, ShProgram> BoundProgramMap;

  BoundProgramMap::iterator begin_bound();
  BoundProgramMap::iterator end_bound();

  /// \internal
  void set_binding(const std::string& unit, const ShProgram& program);
  /// \internal
  void unset_binding(const std::string& unit);

  /// The program currently being constructed. May be null.
  ShProgramNodePtr parsing();

  /// Start constructing the given program
  void enter(const ShProgramNodePtr& program);

  /// Finish constructing the current program
  void exit();
  
private:
  ShContext();

  int m_optimization;
  bool m_throw_errors;
  
  BoundProgramMap m_bound;
  std::stack<ShProgramNodePtr> m_parsing;

  std::set<std::string> m_disabled_optimizations;
  
  static ShContext* m_instance;

  // NOT IMPLEMENTED
  ShContext(const ShContext& other);
  ShContext& operator=(const ShContext& other);
};

typedef ShContext::BoundProgramMap::iterator ShBoundIterator;

/// Check whether a program is bound to the given target
SH_DLLEXPORT
bool shIsBound(const std::string& target);

/// Return the program bound to the given target
SH_DLLEXPORT
ShProgramNodePtr shBound(const std::string& target);

/// Get beginning of bound program map for current context
SH_DLLEXPORT
ShBoundIterator shBeginBound();

/// Get end of bound program map for current context
SH_DLLEXPORT
ShBoundIterator shEndBound();

}

#endif
