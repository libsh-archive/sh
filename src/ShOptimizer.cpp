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
#include <queue>
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

    insertBraInsts();
    
    m_graph->computePredecessors();
    solveReachDefs();
    buildUdDuChains();
    removeDeadCode();

    removeBraInsts();
  }
}

// Copy propagation

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
  CopyPropagator c(*this);
  m_graph->dfs(c);
}

void ShOptimizer::localCopyProp(ShBasicBlockPtr block)
{
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    for (int i = 0; i < opInfo[I->op].arity; i++) copyValue(I->src[i]);
    removeACP(I->dest);

    if (I->op == SH_OP_ASN
        && I->dest.node() != I->src[0].node()
        && I->dest.node()->kind() == SH_VAR_TEMP
        && I->dest.swizzle().identity()
        && I->src[0].swizzle().identity()) {
      m_acp.push_back(std::make_pair(I->dest.node(), I->src[0].node()));
    }
  }
  m_acp.clear();
}

void ShOptimizer::copyValue(ShVariable& var)
{
  for (ACP::const_iterator I = m_acp.begin(); I != m_acp.end(); ++I) {
    if (I->first == var.node()) {
      var.node() = I->second;
      break;
    }
  }
}

void ShOptimizer::removeACP(const ShVariable& var)
{
  for (ACP::iterator I = m_acp.begin(); I != m_acp.end();) {
    if (I->first == var.node() || I->second == var.node()) {
      I = m_acp.erase(I);
      continue;
    }
    ++I;
  }
}

/// A utility routine
bool inRHS(ShVariableNodePtr node, ShStatement& stmt)
{
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    if (stmt.src[i].node() == node) return true;
  }
  
  return false;
}

// Move elimination

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
  MoveEliminator m(*this);
  m_graph->dfs(m);
}

void ShOptimizer::localMoveElim(ShBasicBlockPtr block)
{
  if (!block) return;
  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
    eliminateMove(*I);

    removeAME(I->dest.node());

    if (!inRHS(I->dest.node(), *I)
        && I->dest.node()->kind() == SH_VAR_TEMP
        && I->dest.swizzle().identity()) {
      m_ame.push_back(*I);
    }
  }
  m_ame.clear();
}

void ShOptimizer::eliminateMove(ShStatement& stmt)
{
  if (stmt.op != SH_OP_ASN) return;
  if (stmt.src[0].node()->kind() != SH_VAR_TEMP) return;
  if (!stmt.src[0].swizzle().identity()) return;

  for (AME::const_iterator I = m_ame.begin(); I != m_ame.end(); ++I) {
    if (I->dest.node() == stmt.src[0].node()) {
      ShVariable v = stmt.dest;
      stmt = *I;
      stmt.dest = v;
      break;
    }
  }
}

void ShOptimizer::removeAME(ShVariableNodePtr node)
{
  for (AME::iterator I = m_ame.begin(); I != m_ame.end();) {
    if (I->dest.node() == node || inRHS(node, *I)) {
      I = m_ame.erase(I);
      continue;
    }
    ++I;
  }
}

// Solve the reaching definitions problem

struct DefFinder {
  DefFinder(ShOptimizer& o)
    : o(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->op != SH_OP_KIL && I->op != SH_OP_OPTBRA && I->dest.node()->kind() == SH_VAR_TEMP) {
        o.m_defs.push_back(&(*I));
        o.m_defNodes.push_back(node);
      }
    }
  }

  ShOptimizer& o;
};

struct InitRch {
  InitRch(ShOptimizer& o, ShOptimizer::ReachingMap& gen, ShOptimizer::ReachingMap& prsv)
    : o(o), gen(gen), prsv(prsv)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;

    o.m_reachIn[node] = ShBitSet(o.m_defs.size());
    gen[node] = ShBitSet(o.m_defs.size());
    prsv[node] = ~ShBitSet(o.m_defs.size());
      
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    // Initialize gen
    for (int i = 0; i < o.m_defs.size(); i++) {
      if (o.m_defNodes[i] == node) gen[node][i] = true;
    }

    // Initialize prsv
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA
          || I->dest.node()->kind() != SH_VAR_TEMP
          || I->dest.node()->size() != I->dest.size()) continue;
      for (int i = 0; i < o.m_defs.size(); i++) {
        if (o.m_defs[i]->dest.node() == I->dest.node()) {
          prsv[node][i] = false; // i gets killed by this block
        }
      }
    }
  }

  ShOptimizer& o;
  ShOptimizer::ReachingMap& gen;
  ShOptimizer::ReachingMap& prsv;
};

struct IterateRch {
  IterateRch(ShOptimizer& o, ShOptimizer::ReachingMap& gen, ShOptimizer::ReachingMap& prsv,
             bool& changed)
    : o(o), gen(gen), prsv(prsv), changed(changed)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    SH_DEBUG_ASSERT(o.m_reachIn.find(node) != o.m_reachIn.end());
    ShBitSet newRchIn(o.m_defs.size());
    
    for (ShCtrlGraphNode::ShPredList::iterator I = node->predecessors.begin(); I != node->predecessors.end(); ++I) {
      SH_DEBUG_ASSERT(gen.find(*I) != gen.end());
      SH_DEBUG_ASSERT(prsv.find(*I) != prsv.end());
      SH_DEBUG_ASSERT(o.m_reachIn.find(*I) != o.m_reachIn.end());
      
      newRchIn |= (gen[*I] | (o.m_reachIn[*I] & prsv[*I]));
    }
    if (newRchIn != o.m_reachIn[node]) {
      o.m_reachIn[node] = newRchIn;
      changed = true;
    }
  }

  ShOptimizer& o;
  ShOptimizer::ReachingMap& gen;
  ShOptimizer::ReachingMap& prsv;
  bool& changed;
};

struct DumpRch {
  DumpRch(ShOptimizer::ReachingMap& r)
    : r(r)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    SH_DEBUG_PRINT(node.object() << " = " <<r[node]);
  }

  ShOptimizer::ReachingMap& r;
};

void ShOptimizer::solveReachDefs()
{
  ReachingMap gen;
  ReachingMap prsv;

  m_defs.clear();
    
  DefFinder finder(*this);
  m_graph->dfs(finder);

  m_reachIn.clear();

  InitRch init(*this, gen, prsv);
  m_graph->dfs(init);

  bool changed;
  IterateRch iter(*this, gen, prsv, changed);
  do {
    changed = false;
    m_graph->dfs(iter);
  } while (changed);

#ifdef SH_DEBUG
  SH_DEBUG_PRINT("Dumping RCHin");
  DumpRch dump(m_reachIn);
  m_graph->dfs(dump);
#endif
}

// Build du- and ud- chains

struct UdDuBuilder {
  UdDuBuilder(ShOptimizer& o)
    : o(o)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    typedef std::set<ShStatement*> StmtSet;
    typedef std::map<ShVariableNodePtr, StmtSet> DefMap;
    // defs contains all of the possible contributor's to the key's
    // definition at the current point.
    DefMap defs;
    
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    // TODO: Handle "non assigning" statements like KIL
    // TODO: What about branch conditions. They need to have ud/du
    // links too. Perhaps we should insert bogus branch instructions
    // at the end of each block while we're doing the optimizations,
    // and remove them later. I think this is the cleanest approach.
    
    // initialize defs at the start of the block, using the reaching
    // definitions solution.
    for (std::size_t i = 0; i < o.m_defs.size(); i++) {
      if (o.m_reachIn[node][i])
        defs[o.m_defs[i]->dest.node()].insert(o.m_defs[i]);
    }

    // Now consider each statement in turn.
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      // Compute the ud chains for the statement's source variables,
      // and contribute to the du chains of the source variables' definitions.
      for (int j = 0; j < opInfo[I->op].arity; j++) {
        if (I->src[j].node()->kind() == SH_VAR_TEMP) {
          I->ud[j] = defs[I->src[j].node()];
          for (StmtSet::iterator J = defs[I->src[j].node()].begin(); J != defs[I->src[j].node()].end(); J++) {
            (*J)->du.insert(&(*I));
          }
        }
      }
      // Now update the defs structure.
      
      if (I->dest.size() == I->dest.node()->size()) {
        // If we override all of the variable's contents, we replace
        // defs completely.
        defs[I->dest.node()].clear();
      }
      
      // This statement contributes to the possible definitions of I->dest
      defs[I->dest.node()].insert(&(*I));
    }

  }

  ShOptimizer& o;
};

struct UdDuDumper {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      SH_DEBUG_PRINT(*I);
      for (int i = 0; i < 3; i++) {
        SH_DEBUG_PRINT("  ud[" << i << "]");
        for (std::set<ShStatement*>::iterator J = I->ud[i].begin(); J != I->ud[i].end(); ++J) {
          SH_DEBUG_PRINT("   - " << **J);
        }
      }
      SH_DEBUG_PRINT("  du");
      for (std::set<ShStatement*>::iterator J = I->du.begin(); J != I->du.end(); ++J) {
        SH_DEBUG_PRINT("   - " << **J);
      }
    }
  }
};

void ShOptimizer::buildUdDuChains()
{
  UdDuBuilder builder(*this);
  m_graph->dfs(builder);

  m_reachIn.clear(); // We don't need the reaching definitions
                     // structures anymore.

  UdDuDumper dumper;
  m_graph->dfs(dumper);
}

// Dead code removal

typedef std::queue<ShStatement*> DeadCodeWorkList;

struct InitLiveCode {
  InitLiveCode(DeadCodeWorkList& w)
    : w(w)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->dest.node()->kind() != SH_VAR_TEMP
          || I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA) {
        I->marked = true;
        w.push(&(*I));
        continue;
      }
      I->marked = false;
    }
  }

  DeadCodeWorkList& w;
};

struct DeadCodeRemover {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
      if (!I->marked) {
        I = block->erase(I);
        continue;
      }
      ++I;
    }
  }
};

void ShOptimizer::removeDeadCode()
{
  DeadCodeWorkList w;

  InitLiveCode init(w);
  m_graph->dfs(init);

  SH_DEBUG_PRINT("Initialized the worklist. It now has " << w.size() << " elements")
  
  while (!w.empty()) {
    ShStatement* stmt = w.front(); w.pop();
    SH_DEBUG_PRINT("Processing " << *stmt);
    for (int i = 0; i < 3; i++) {
      for (std::set<ShStatement*>::iterator J = stmt->ud[i].begin(); J != stmt->ud[i].end(); ++J) {
        if ((*J)->marked) continue;
        SH_DEBUG_PRINT("Marking " << **J);
        (*J)->marked = true;
        w.push(*J);
      }
    }
  }

  DeadCodeRemover r;
  m_graph->dfs(r);
}

struct BraInstInserter {
  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;

    for (std::vector<ShCtrlGraphBranch>::const_iterator I = node->successors.begin();
         I != node->successors.end(); ++I) {
      if (!node->block) node->block = new ShBasicBlock();
      node->block->addStatement(ShStatement(I->cond, SH_OP_OPTBRA, I->cond));
    }
  }
  
};

void ShOptimizer::insertBraInsts()
{
  BraInstInserter i;
  m_graph->dfs(i);
}

struct BraInstRemover {
  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
      if (I->op == SH_OP_OPTBRA) {
        I = block->erase(I);
        continue;
      }
      ++I;
    }
  }
};

void ShOptimizer::removeBraInsts()
{
  BraInstRemover r;
  m_graph->dfs(r);
}

}

