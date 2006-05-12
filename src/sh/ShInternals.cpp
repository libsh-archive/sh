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
#include "ShInternals.hpp"
#include "ShDebug.hpp"

namespace SH { 

ShVariableReplacer::ShVariableReplacer(ShVarMap& v)
  : varMap(v) 
{
}

void ShVariableReplacer::operator()(ShCtrlGraphNode* node) 
{
  // replace variables that are conditions in branches
  ShCtrlGraphNode::SuccessorIt I;
  for(I = node->successors_begin(); I != node->successors_end(); ++I) {
    repVar(I->cond);
  }

  // replace variables in the block
  if (!node) return;
  ShBasicBlockPtr block = node->block;
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    repVar(I->dest);
    for (int i = 0; i < 3; i++) {
      repVar(I->src[i]);
    }
  }

}

void ShVariableReplacer::operator()(ShStructuralNode* node) 
{
  if(node->cfg_node) operator()(node->cfg_node);
  for (ShStructuralNode::StructNodeList::iterator I = node->structnodes.begin(); 
      I != node->structnodes.end(); ++I) {
    operator()(*I);
  }
}

void ShVariableReplacer::operator()(ShProgramNode::VarList &varList) 
{
  ShProgramNode::VarList::iterator I;
  for(I = varList.begin(); I != varList.end();) {
    if(varMap.count(*I) > 0) {
      varList.insert(I, varMap[*I]);
      I = varList.erase(I);
    } else ++I;
  }
}

void ShVariableReplacer::repVar(ShVariable& var) 
{
  if(var.null()) return;
  ShVarMap::iterator I = varMap.find(var.node());
  if (I == varMap.end()) return;
  var = ShVariable(I->second, var.swizzle(), var.neg());
}



void ShVarTransformMap::add_variable_transform(const ShVariableNodePtr& origVar, const ShVariableNodePtr& newVar)
{
  // We need only go one level of depth since we store the mappings to the ORIGINAL,
  // instead of traversing the chain each time.
  ShVarMap::iterator i = m_NewToOldMap.find(origVar);
  m_NewToOldMap[newVar] = (i == m_NewToOldMap.end() ? origVar : i->second);
}

}


