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
#include "ShOptimizer.hpp"
#include <algorithm>
#include "ShDebug.hpp"

namespace SH {

ShOptimizer::ShOptimizer(ShCtrlGraphPtr graph)
  : m_graph(graph)
{
}

void ShOptimizer::optimize(int level)
{
  if (level == 0) return; // Level 0 means no optimizations at all.

  if (level >= 1) {
    copyPropagation();
    moveElimination();
    unusedTempsRemoval();
  }
}

struct CopyPropagator {
  CopyPropagator(ShOptimizer& o)
    : optimizer(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    optimizer.localCopyProp(node->block);
  }
  
  ShOptimizer& optimizer;
};

void ShOptimizer::copyPropagation()
{
  SH_DEBUG_PRINT("Doing Copy Propagation...");
  CopyPropagator c(*this);
  m_graph->dfs(c);
}

void ShOptimizer::localCopyProp(ShBasicBlockPtr block)
{
  if (!block) return;
  SH_DEBUG_PRINT("Doing local copy propagation");
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    SH_DEBUG_PRINT("Looking at a statement");
    if (opInfo[I->op].arity >= 1) {
      copyValue(I->src1);
    }
    if (opInfo[I->op].arity >= 2) {
      copyValue(I->src2);
    }
    if (opInfo[I->op].arity >= 3) {
      copyValue(I->src3);
    }
    removeACP(I->dest);

    if (I->op == SH_OP_ASN
        && I->dest.node() != I->src1.node()
        && I->dest.node()->kind() == SH_VAR_TEMP
        && I->dest.swizzle().identity()
        && I->src1.swizzle().identity()) {
      SH_DEBUG_PRINT("Adding something to the ACP");
      m_acp.push_back(std::make_pair(I->dest.node(), I->src1.node()));
    }
  }
  m_acp.clear();
}

void ShOptimizer::copyValue(ShVariable& var)
{
  for (ACP::const_iterator I = m_acp.begin(); I != m_acp.end(); ++I) {
    if (I->first == var.node()) {
      SH_DEBUG_PRINT("Replacing something");
      var.node() = I->second;
    }
  }
}

void ShOptimizer::removeACP(const ShVariable& var)
{
  for (ACP::iterator I = m_acp.begin(); I != m_acp.end(); ++I) {
    if (I->first == var.node() || I->second == var.node()) {
      SH_DEBUG_PRINT("Removing something");
      ACP::iterator J = I;
      J++;
      m_acp.erase(I);
      I = J;
    }
  }
}

bool inRHS(ShVariableNodePtr node, ShStatement& stmt)
{
  if (opInfo[stmt.op].arity >= 1) {
    if (stmt.src1.node() == node) return true;
  }
  if (opInfo[stmt.op].arity >= 2) {
    if (stmt.src2.node() == node) return true;
  }
  if (opInfo[stmt.op].arity >= 3) {
    if (stmt.src3.node() == node) return true;
  }
  return false;
}

struct MoveEliminator {
  MoveEliminator(ShOptimizer& o)
    : optimizer(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    optimizer.localMoveElim(node->block);
  }
  
  ShOptimizer& optimizer;
};

void ShOptimizer::moveElimination()
{
  SH_DEBUG_PRINT("Doing Move Elimination...");
  MoveEliminator m(*this);
  m_graph->dfs(m);
}

void ShOptimizer::localMoveElim(ShBasicBlockPtr block)
{
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    SH_DEBUG_PRINT("Looking at a statement");
    eliminateMove(*I);

    removeAME(I->dest.node());

    if (!inRHS(I->dest.node(), *I)
        && I->dest.node()->kind() == SH_VAR_TEMP
        && I->dest.swizzle().identity()) {
      SH_DEBUG_PRINT("Adding something to the AME");
      m_ame.push_back(*I);
    }
  }
  m_ame.clear();
}

void ShOptimizer::eliminateMove(ShStatement& stmt)
{
  if (stmt.op != SH_OP_ASN) return;
  if (stmt.src1.node()->kind() != SH_VAR_TEMP) return;
  if (!stmt.src1.swizzle().identity()) return;

  for (AME::const_iterator I = m_ame.begin(); I != m_ame.end(); ++I) {
    if (I->dest.node() == stmt.src1.node()) {
      SH_DEBUG_PRINT("Replacing something");
      ShVariable v = stmt.dest;
      stmt = *I;
      stmt.dest = v;
      break;
    }
  }
}

void ShOptimizer::removeAME(ShVariableNodePtr node)
{
  for (AME::iterator I = m_ame.begin(); I != m_ame.end(); ++I) {
    if (I->dest.node() == node || inRHS(node, *I)) {
      SH_DEBUG_PRINT("Removing something");
      AME::iterator J = I;
      J++;
      m_ame.erase(I);
      I = J;
    }
  }
}

// Unused Temporary Removal

struct TempFinder {
  TempFinder(ShOptimizer& o)
    : optimizer(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (opInfo[I->op].arity >= 1) {
        if (I->src1.node()->kind() == SH_VAR_TEMP) optimizer.m_tum.insert(I->src1.node());
      }
      if (opInfo[I->op].arity >= 2) {
        if (I->src2.node()->kind() == SH_VAR_TEMP) optimizer.m_tum.insert(I->src2.node());
      }
      if (opInfo[I->op].arity >= 3) {
        if (I->src3.node()->kind() == SH_VAR_TEMP) optimizer.m_tum.insert(I->src3.node());
      }
    }
  }
  
  ShOptimizer& optimizer;
};

struct TempRemover {
  TempRemover(ShOptimizer& o)
    : optimizer(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (!I->dest.node()) continue;
      if (I->dest.node()->kind() != SH_VAR_TEMP) continue;
      if (optimizer.m_tum.find(I->dest.node()) == optimizer.m_tum.end()) {
        //        ShBasicBlock::ShStmtList::iterator J = I;
        //        J++;
        block->erase(I);
        //        I = J;
      }
    }
  }
  
  ShOptimizer& optimizer;
};

void ShOptimizer::unusedTempsRemoval()
{
  TempFinder f(*this);

  m_graph->dfs(f);
  
  TempRemover r(*this);
  
  m_graph->dfs(r);

  m_tum.clear();
}

}
