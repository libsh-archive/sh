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
#include "Internals.hpp"
#include "Debug.hpp"

namespace SH { 

VariableReplacer::VariableReplacer(VarMap& v)
  : varMap(v) 
{
}

void VariableReplacer::operator()(const CtrlGraphNodePtr& node) 
{
  // replace variables that are conditions in branches
  CtrlGraphNode::SuccessorList::iterator I;
  for(I = node->successors.begin(); I != node->successors.end(); ++I) {
    repVar(I->cond);
  }

  // replace variables in the block
  if (!node) return;
  BasicBlockPtr block = node->block;
  if (!block) return;
  for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
    repVar(I->dest);
    for (int i = 0; i < 3; i++) {
      repVar(I->src[i]);
    }
  }

}

void VariableReplacer::operator()(const StructuralNodePtr& node) 
{
  if(node->cfg_node) operator()(node->cfg_node);
  for (StructuralNode::StructNodeList::iterator I = node->structnodes.begin(); 
      I != node->structnodes.end(); ++I) {
    operator()(*I);
  }
}

void VariableReplacer::operator()(ProgramNode::VarList &varList) 
{
  ProgramNode::VarList::iterator I;
  for(I = varList.begin(); I != varList.end();) {
    if(varMap.count(*I) > 0) {
      varList.insert(I, varMap[*I]);
      I = varList.erase(I);
    } else ++I;
  }
}

void VariableReplacer::repVar(Variable& var) 
{
  if(var.null()) return;
  VarMap::iterator I = varMap.find(var.node());
  if (I == varMap.end()) return;
  var = Variable(I->second, var.swizzle(), var.neg());
}



void VarTransformMap::add_variable_transform(const VariableNodePtr& origVar, const VariableNodePtr& newVar)
{
  // We need only go one level of depth since we store the mappings to the ORIGINAL,
  // instead of traversing the chain each time.
  VarMap::iterator i = m_NewToOldMap.find(origVar);
  m_NewToOldMap[newVar] = (i == m_NewToOldMap.end() ? origVar : i->second);
}

}


