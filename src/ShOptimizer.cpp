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
#include "ShInternals.hpp"

namespace SH {

ShOptimizer::ShOptimizer(ShCtrlGraphPtr graph)
  : m_graph(graph)
{
}

void ShOptimizer::optimize(int level)
{
  if (level == 0) return; // Level 0 means no optimizations at all.

  if (level >= 1) {
    bool changed;
    do {
      changed = false;

      /*
      inputOutputConversion(changed);
      */
      if( level >= 2 ) {
        copyPropagation(changed);
        moveElimination(changed);
        
        m_graph->computePredecessors();
        
        straighten(changed);

        insertBraInsts();
        
        solveReachDefs();
        buildUdDuChains();
        removeDeadCode(changed);
        
        removeBraInsts();
      }
    } while (changed);
  }
}

// Copy propagation

struct CopyPropagator {
  CopyPropagator(ShOptimizer& o, bool& changed)
    : o(o), changed(changed)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      for (int i = 0; i < opInfo[I->op].arity; i++) copyValue(I->src[i]);
      removeACP(I->dest);
      
      if (I->op == SH_OP_ASN
          && I->dest.node() != I->src[0].node()
          && I->dest.node()->kind() == SH_VAR_TEMP
          && I->dest.swizzle().identity()
          && I->src[0].swizzle().identity()) {
        m_acp.push_back(std::make_pair(I->dest, I->src[0]));
      }
    }
    m_acp.clear();
  }

  void removeACP(const ShVariable& var)
  {
    for (ACP::iterator I = m_acp.begin(); I != m_acp.end();) {
      if (I->first.node() == var.node() || I->second.node() == var.node()) {
        I = m_acp.erase(I);
        continue;
      }
      ++I;
    }
  }
  

  void copyValue(ShVariable& var)
  {
    for (ACP::const_iterator I = m_acp.begin(); I != m_acp.end(); ++I) {
      if (I->first.node() == var.node()) {
        changed = true;
        var.node() = I->second.node();
        var.neg() = var.neg() ^ (I->first.neg() ^ I->second.neg());
        break;
      }
    }
  }

  typedef std::list< std::pair<ShVariable, ShVariable> > ACP;
  ACP m_acp;
  
  ShOptimizer& o;
  bool& changed;
};

void ShOptimizer::copyPropagation(bool& changed)
{
  CopyPropagator c(*this, changed);
  m_graph->dfs(c);
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
  MoveEliminator(ShOptimizer& o, bool& changed)
    : optimizer(o), changed(changed)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
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
  
  void eliminateMove(ShStatement& stmt)
  {
    if (stmt.op != SH_OP_ASN) return;
    if (stmt.src[0].neg()) return;
    if (stmt.src[0].node()->kind() != SH_VAR_TEMP) return;
    if (!stmt.src[0].swizzle().identity()) return;

    for (AME::const_iterator I = m_ame.begin(); I != m_ame.end(); ++I) {
      if (I->dest.node() == stmt.src[0].node()) {
        ShVariable v = stmt.dest;
        stmt = *I;
        stmt.dest = v;
        changed = true;
        break;
      }
    }
  }

  void removeAME(ShVariableNodePtr node)
  {
    for (AME::iterator I = m_ame.begin(); I != m_ame.end();) {
      if (I->dest.node() == node || inRHS(node, *I)) {
        I = m_ame.erase(I);
        continue;
      }
      ++I;
    }
  }

  ShOptimizer& optimizer;
  bool& changed;
  typedef std::list<ShStatement> AME;
  AME m_ame;
};

void ShOptimizer::moveElimination(bool& changed)
{
  MoveEliminator m(*this, changed);
  m_graph->dfs(m);
}

// Output Convertion to temporaries 
struct InputOutputConvertor {
  InputOutputConvertor(ShOptimizer& o, bool& changed)
    : optimizer(o), changed(changed)
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convertIO(*I);
    }
  }
  
  void convertIO(ShStatement& stmt)
  {
    if(!stmt.dest.null() && stmt.dest.node()->kind() == SH_VAR_INPUT) {
        ShVariableNodePtr &oldNode = stmt.dest.node();
      if( varMap.find( oldNode ) == varMap.end() ) {
        varMap[oldNode] = new ShVariableNode(SH_VAR_TEMP, 
            oldNode->size(), oldNode->specialType());
      }
    }
    for(int i = 0; i < 3; ++i) {
      if(!stmt.src[i].null() && stmt.src[i].node()->kind() == SH_VAR_OUTPUT) {
        ShVariableNodePtr &oldNode = stmt.src[i].node();
        if( varMap.find( oldNode ) == varMap.end() ) {
          varMap[oldNode] = new ShVariableNode(SH_VAR_TEMP, 
              oldNode->size(), oldNode->specialType());
        }
      }
    }
    if( !varMap.empty() ) changed = true;
  }

  void updateGraph() {
    if( varMap.empty() ) return;

    // create block after exit
    ShCtrlGraphNodePtr newExit = new ShCtrlGraphNode(); 
    ShCtrlGraphNodePtr oldExit = optimizer.m_graph->exit();
    oldExit->block = new ShBasicBlock();
    oldExit->append( newExit );
    optimizer.m_graph->setExit( newExit );

    // create block before entry 
    ShCtrlGraphNodePtr newEntry = new ShCtrlGraphNode(); 
    ShCtrlGraphNodePtr oldEntry = optimizer.m_graph->entry();
    newEntry->block = new ShBasicBlock();
    newEntry->append( oldEntry );
    optimizer.m_graph->setEntry( newEntry ); 

    // generate code in the old entry and exit blocks 
    if( !oldExit->block ) {
      oldExit->block = new ShBasicBlock(); 
    }
    if( !oldEntry->block ) {
      oldEntry->block = new ShBasicBlock(); 
    }
    for(ShVariableReplacer::VarMap::iterator it = varMap.begin(); it != varMap.end(); ++it) {
      // assign temporary to output
      ShVariableNodePtr oldNode = it->first; 
      if( oldNode->kind() == SH_VAR_OUTPUT ) {
        oldExit->block->addStatement(ShStatement(
              ShVariable(oldNode), SH_OP_ASN, ShVariable(it->second)));
      } else {
        oldEntry->block->addStatement(ShStatement(
              ShVariable(it->second), SH_OP_ASN, ShVariable(oldNode)));
      }
    }
  }

  ShOptimizer& optimizer;
  bool& changed;
  ShVariableReplacer::VarMap varMap; // maps from outputs used as srcs in computation to their temporary variables
};

void ShOptimizer::inputOutputConversion(bool& changed)
{
  SH_DEBUG_PRINT( "Input Output Conversion" );
  InputOutputConvertor ioc(*this, changed);
  m_graph->dfs(ioc);

  ShVariableReplacer vr( ioc.varMap );
  m_graph->dfs(vr);

  ioc.updateGraph(); 
}

// Straightening
struct Straightener {
  Straightener(ShOptimizer& o, bool& changed)
    : o(o), changed(changed)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    if (!node->follower) return;
    if (node == o.m_graph->entry()) return;
    if (node->follower == o.m_graph->exit()) return;
    if (!node->successors.empty()) return;
    if (node->follower->predecessors.size() > 1) return;
    
    if (!node->block) node->block = new ShBasicBlock();
    if (!node->follower->block) node->follower->block = new ShBasicBlock();

    for (ShBasicBlock::ShStmtList::iterator I = node->follower->block->begin(); I != node->follower->block->end(); ++I) {
      node->block->addStatement(*I);
    }
    node->successors = node->follower->successors;

    // Update predecessors
    
    for (std::vector<ShCtrlGraphBranch>::iterator I = node->follower->successors.begin();
         I != node->follower->successors.end(); ++I) {
      replacePredecessors(I->node, node->follower.object(), node.object());
    }
    if (node->follower->follower) replacePredecessors(node->follower->follower, node->follower.object(), node.object());
    
    node->follower = node->follower->follower;

    changed = true;
  }

  void replacePredecessors(ShCtrlGraphNodePtr node,
                           ShCtrlGraphNode* old,
                           ShCtrlGraphNode* replacement)
  {
    for (ShCtrlGraphNode::ShPredList::iterator I = node->predecessors.begin(); I != node->predecessors.end(); ++I) {
      if (*I == old) {
        *I = replacement;
        break;
      }
    }
  }
  
  ShOptimizer& o;
  bool& changed;
};

void ShOptimizer::straighten(bool& changed)
{
  Straightener s(*this, changed);
  m_graph->dfs(s);
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
    for (unsigned int i = 0; i < o.m_defs.size(); i++) {
      if (o.m_defNodes[i] == node) gen[node][i] = true;
    }

    // Initialize prsv
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA
          || I->dest.node()->kind() != SH_VAR_TEMP
          || I->dest.node()->size() != I->dest.size()) continue;
      for (unsigned int i = 0; i < o.m_defs.size(); i++) {
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

#if 0
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

#if 0
  UdDuDumper dumper;
  m_graph->dfs(dumper);
#endif
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
  DeadCodeRemover(bool& changed)
    : changed(changed)
  {
  }
  
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
      if (!I->marked) {
        changed = true;
        I = block->erase(I);
        continue;
      }
      ++I;
    }
  }

  bool& changed;
};

void ShOptimizer::removeDeadCode(bool& changed)
{
  DeadCodeWorkList w;

  InitLiveCode init(w);
  m_graph->dfs(init);

  while (!w.empty()) {
    ShStatement* stmt = w.front(); w.pop();
    for (int i = 0; i < 3; i++) {
      for (std::set<ShStatement*>::iterator J = stmt->ud[i].begin(); J != stmt->ud[i].end(); ++J) {
        if ((*J)->marked) continue;
        (*J)->marked = true;
        w.push(*J);
      }
    }
  }

  DeadCodeRemover r(changed);
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

