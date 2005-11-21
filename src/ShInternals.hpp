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
#ifndef SHINTERNALS_HPP 
#define SHINTERNALS_HPP

#include <map>
#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShBackend.hpp"
#include "ShProgramNode.hpp"
#include "ShStructural.hpp"
#include <map>

namespace SH { 

typedef std::map<ShVariableNodePtr, ShVariableNodePtr> ShVarMap;

struct
SH_DLLEXPORT ShVariableReplacer {

  ShVariableReplacer(ShVarMap& v);

  // assignment operator could not be generated: declaration only
  ShVariableReplacer& operator=(ShVariableReplacer const&);

  // replaces variables in node based on varMap
  void operator()(const ShCtrlGraphNodePtr& node);

  // replaces variables in a ShProgramNode::VarList based on varMap 
  void operator()(ShProgramNode::VarList &varList);

  // replaces variables in a ShStructuralNode and all Structural
  // Nodes in its region.
  void operator()(const ShStructuralNodePtr& node);

  // replaces node in a single variable using varMap
  void repVar(ShVariable& var);

  ShVarMap& varMap;
};


/** Keeps a (potentially composite) mapping from new variables (ex. when compiling)
 * to the originals and supports querying for the original variable from which
 * the given "transformed" variable was derived.
 */
class
SH_DLLEXPORT ShVarTransformMap {
private:
  ShVarMap m_NewToOldMap;

public:
  ShVarTransformMap() {}
  explicit ShVarTransformMap(const ShVarTransformMap &m)
    : m_NewToOldMap(m.m_NewToOldMap)
  {}
  ShVarTransformMap & operator=(const ShVarTransformMap &m)
  {
    m_NewToOldMap = m.m_NewToOldMap;
    return *this;
  }

  /// Clear the map out
  void clear() { m_NewToOldMap.clear(); }

  /// Indicate that we are creating a new transformed variable derived from the
  /// given original variable. If the original variable is already in the map, the
  /// mapping from new to old will follow the chain all the way back.
  void add_variable_transform(const ShVariableNodePtr& origVar, const ShVariableNodePtr& newVar);

  /// Get the original variable (ALL the way back) that gave rise to the given
  /// transformed variable. If a mapping is not found for the given variable,
  /// the variable itself is return (ie. it has not been transformed at all).
  ShVariableNodePtr get_original_variable(const ShVariableNodePtr& newVar) const
  {
    ShVarMap::const_iterator i = m_NewToOldMap.find(newVar);
    return (i != m_NewToOldMap.end() ? i->second : newVar);
  }
};


}

#endif

