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
#include "ShInternals.hpp"
#include "ShDebug.hpp"

namespace SH { 

ShVariableReplacer::ShVariableReplacer(VarMap& v)
  : varMap(v) {
}

void ShVariableReplacer::operator()(ShCtrlGraphNodePtr node) {
  if (!node) return;
  ShBasicBlockPtr block = node->block;
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    if(!I->dest.null()) repVar(I->dest);
    for (int i = 0; i < 3; i++) {
      if( !I->src[i].null() ) repVar(I->src[i]);
    }
  }
}

void ShVariableReplacer::repVar(ShVariable& var) {
  VarMap::iterator I = varMap.find(var.node());
  if (I == varMap.end()) return;
  var.node() = I->second;
}

typedef std::map<ShCtrlGraphNodePtr, ShCtrlGraphNodePtr> CtrlGraphCopyMap;

struct CtrlGraphCopier {
  CtrlGraphCopier(CtrlGraphCopyMap& copyMap)
    : copyMap(copyMap)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShCtrlGraphNodePtr newNode = new ShCtrlGraphNode(*node);
    copyMap[node] = newNode;
  }

  CtrlGraphCopyMap& copyMap;
};

void copyCtrlGraph(ShCtrlGraphNodePtr head, ShCtrlGraphNodePtr tail,
                   ShCtrlGraphNodePtr& newHead, ShCtrlGraphNodePtr& newTail)
{
  CtrlGraphCopyMap copyMap;
  copyMap[0] = 0;
  
  CtrlGraphCopier copier(copyMap);
  SH_DEBUG_ASSERT(head);
  SH_DEBUG_ASSERT(tail); // catch empty tails
  head->clearMarked();
  head->dfs(copier);

  // Replace the successors and followers in the new graph with their new equivalents
  for (CtrlGraphCopyMap::iterator I = copyMap.begin(); I != copyMap.end(); ++I) {
    ShCtrlGraphNodePtr node = I->second; // Get the new node
    if (!node) continue;
    for (std::vector<ShCtrlGraphBranch>::iterator J = node->successors.begin(); J != node->successors.end(); ++J) {
      J->node = copyMap[J->node];
    }
    node->follower = copyMap[node->follower];
    if (node->block) {
      ShBasicBlockPtr new_block = new ShBasicBlock(*node->block);
      node->block = new_block;
    }
  }
  newHead = copyMap[head];
  newTail = copyMap[tail];
  SH_DEBUG_ASSERT(newHead);
  SH_DEBUG_ASSERT(newTail);

  head->clearMarked();
}

ShProgram cloneProgram(ShProgram p) {
  ShCtrlGraphNodePtr head, tail;
  copyCtrlGraph(p->ctrlGraph->entry(), p->ctrlGraph->exit(), head, tail);

  ShProgram result = new ShProgramNode(p->target());
  result->ctrlGraph = new ShCtrlGraph(head, tail);
  result->inputs = p->inputs;
  result->outputs = p->outputs;
  return result;
}

}


