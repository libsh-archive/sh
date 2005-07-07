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

  // replaces variables in node based on varMap
  void operator()(ShCtrlGraphNodePtr node);

  // replaces variables in a ShProgramNode::VarList based on varMap 
  void operator()(ShProgramNode::VarList &varList);

  // replaces variables in a ShStructuralNode and all Structural
  // Nodes in its region.
  void operator()(ShStructuralNodePtr node);

  // replaces node in a single variable using varMap
  void repVar(ShVariable& var);

  ShVarMap& varMap;
};

}

#endif

