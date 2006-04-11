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
#ifndef SHINTERNALS_HPP 
#define SHINTERNALS_HPP

#include <map>
#include "DllExport.hpp"
#include "VariableNode.hpp"
#include "CtrlGraph.hpp"
#include "Backend.hpp"
#include "ProgramNode.hpp"
#include "Structural.hpp"
#include <map>

namespace SH { 

typedef std::map<VariableNodePtr, VariableNodePtr> VarMap;

struct
DLLEXPORT VariableReplacer {

  VariableReplacer(VarMap& v);

  // assignment operator could not be generated: declaration only
  VariableReplacer& operator=(VariableReplacer const&);

  // replaces variables in node based on varMap
  void operator()(const CtrlGraphNodePtr& node);

  // replaces variables in a ProgramNode::VarList based on varMap 
  void operator()(ProgramNode::VarList &varList);

  // replaces variables in a StructuralNode and all Structural
  // Nodes in its region.
  void operator()(const StructuralNodePtr& node);

  // replaces node in a single variable using varMap
  void repVar(Variable& var);

  VarMap& varMap;
};


/** Keeps a (potentially composite) mapping from new variables (ex. when compiling)
 * to the originals and supports querying for the original variable from which
 * the given "transformed" variable was derived.
 */
class
DLLEXPORT VarTransformMap {
private:
  VarMap m_NewToOldMap;

public:
  VarTransformMap() {}
  explicit VarTransformMap(const VarTransformMap &m)
    : m_NewToOldMap(m.m_NewToOldMap)
  {}
  VarTransformMap & operator=(const VarTransformMap &m)
  {
    m_NewToOldMap = m.m_NewToOldMap;
    return *this;
  }

  /// Clear the map out
  void clear() { m_NewToOldMap.clear(); }

  /// Indicate that we are creating a new transformed variable derived from the
  /// given original variable. If the original variable is already in the map, the
  /// mapping from new to old will follow the chain all the way back.
  void add_variable_transform(const VariableNodePtr& origVar, const VariableNodePtr& newVar);

  /// Get the original variable (ALL the way back) that gave rise to the given
  /// transformed variable. If a mapping is not found for the given variable,
  /// the variable itself is return (ie. it has not been transformed at all).
  VariableNodePtr get_original_variable(const VariableNodePtr& newVar) const
  {
    VarMap::const_iterator i = m_NewToOldMap.find(newVar);
    return (i != m_NewToOldMap.end() ? i->second : newVar);
  }
};


}

#endif

