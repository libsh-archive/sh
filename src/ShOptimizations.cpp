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
#include "ShOptimizations.hpp"
#include <map>
#include <set>
#include <utility>
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShEvaluate.hpp"
#include "ShContext.hpp"
#include "ShSyntax.hpp"
#include <sstream>
#include <fstream>

namespace {

using namespace SH;

// Branch instruction insertion/removal

struct BraInstInserter {
  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;

    for (std::vector<ShCtrlGraphBranch>::const_iterator I = node->successors.begin();
         I != node->successors.end(); ++I) {
      if (!node->block) node->block = new ShBasicBlock();
      node->block->addStatement(ShStatement(I->cond, SH_OP_OPTBRA, I->cond));
    }
  }
};

struct BraInstRemover {
  void operator()(const ShCtrlGraphNodePtr& node)
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

// Straightening
struct Straightener {
  Straightener(const ShCtrlGraphPtr& graph, bool& changed)
    : graph(graph), changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  Straightener& operator=(Straightener const&);

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    if (!node->follower) return;
    if (node == graph->entry()) return;
    if (node->follower == graph->exit()) return;
    if (!node->successors.empty()) return;
    if (node->follower->predecessors.size() > 1) return;

    // conditions added for maintaining section START/END marker invariants
    // node cannot end in an ENDSEC, node->follower cannot start with a STARTSEC
    if (node->block && !node->block->empty() && node->block->rbegin()->op == SH_OP_ENDSEC) return; 
    if (node->follower->block && !node->follower->block->empty() && node->follower->block->begin()->op == SH_OP_STARTSEC) return; 
    
    if (!node->block) node->block = new ShBasicBlock();
    if (!node->follower->block) node->follower->block = new ShBasicBlock();

    for (ShBasicBlock::ShStmtList::iterator I = node->follower->block->begin(); I != node->follower->block->end(); ++I) {
      node->block->addStatement(*I);
    }
    node->successors = node->follower->successors;

    // merge in declarations
    node->insert_decls(node->follower->decl_begin(), node->follower->decl_end());

    // Update predecessors
    
    for (std::vector<ShCtrlGraphBranch>::iterator I = node->follower->successors.begin();
         I != node->follower->successors.end(); ++I) {
      replacePredecessors(I->node, node->follower.object(), node.object());
    }
    if (node->follower->follower) replacePredecessors(node->follower->follower, node->follower.object(), node.object());
    
    node->follower = node->follower->follower;

    changed = true;
  }

  void replacePredecessors(const ShCtrlGraphNodePtr& node,
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
  
  ShCtrlGraphPtr graph;
  bool& changed;
};


// Remove empty blocks
struct EmptyBlockRemover {
  EmptyBlockRemover(const ShCtrlGraphPtr& graph, bool& changed)
    : graph(graph), changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  EmptyBlockRemover& operator=(EmptyBlockRemover const&);

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    if (!node->follower) return;
    if (node == graph->entry()) return;
    if (!node->successors.empty()) return;

    // TODO: section stuff?
    if (node->block && !node->block->empty()) return;

    to_remove.push_back(node);
  }

  void finish()
  {
    for (std::list<ShCtrlGraphNodePtr>::iterator I = to_remove.begin(); I != to_remove.end(); ++I) {
      remove(*I);
    }
  }

  void remove(const ShCtrlGraphNodePtr& node)
  {
    for (ShCtrlGraphNode::ShPredList::iterator P = node->predecessors.begin(); P != node->predecessors.end(); ++P) {
      ShCtrlGraphNode* pred = *P;
      for (std::vector<ShCtrlGraphBranch>::iterator S = pred->successors.begin(); S != pred->successors.end(); ++S) {
        if (S->node == node) {
          S->node = node->follower;
        }
      }
      if (pred->follower == node) {
        pred->follower = node->follower;
      }
    }

    // Remove ourselves from our follower's preds
    for (ShCtrlGraphNode::ShPredList::iterator P = node->follower->predecessors.begin(); P != node->follower->predecessors.end();) {
      if (*P == node.object()) {
        P = node->follower->predecessors.erase(P);
      } else {
        ++P;
      }
    }

    // Add in our predecessors to our follower's preds.
    for (ShCtrlGraphNode::ShPredList::iterator P = node->predecessors.begin(); P != node->predecessors.end(); ++P) {
      if (std::find(node->follower->  predecessors.begin(), node->follower->predecessors.end(), *P) == node->follower->predecessors.end()) {
        node->follower->predecessors.push_back(*P);
      }
    }

    changed = true;
  }

  ShCtrlGraphPtr graph;
  bool& changed;

  std::list<ShCtrlGraphNodePtr> to_remove;
};


// Remove redundant edges
struct RedundantEdgeRemover {
  RedundantEdgeRemover(bool& changed)
    : changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  RedundantEdgeRemover& operator=(RedundantEdgeRemover const&);

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    if (!node->follower) return;
    if (node->successors.empty()) return;

    ShCtrlGraphNode::SuccessorList::iterator I = node->successors.end();
    --I;
    while (1) {
      if (I->node == node->follower) {
        I = node->successors.erase(I);
        if (I != node->successors.begin()) {
          --I;
        } else {
          break;
        }
      } else {
        break;
      }
    }
  }

  bool& changed;
};

typedef std::queue<ShStatement*> DeadCodeWorkList;

struct InitLiveCode {
  InitLiveCode(const ShProgramNodeCPtr& p, DeadCodeWorkList& w)
    : p(p), w(w)
  {
  }
  
  // assignment operator could not be generated: declaration only
  InitLiveCode& operator=(InitLiveCode const&);

  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
#ifdef SH_DEBUG_OPTIMIZER
      if(!I->dest.null() && I->dest.node()->kind() == SH_TEMP && !p->hasDecl(I->dest.node())) {
        SH_DEBUG_PRINT("No decl for " << I->dest.name());
      }
#endif
      // @todo range check if IGNORE is right
      // maybe use a different flag
      // @todo range conditions here are probably too conservative for 
      // inputs, outputs used in computation -> use the valuetracking
      if (opInfo[I->op].result_source == ShOperationInfo::IGNORE
          || I->dest.node()->kind() != SH_TEMP
          || I->dest.node()->uniform()
          || !p->hasDecl(I->dest.node())
          || I->op == SH_OP_OPTBRA) {
        I->marked = true;
        w.push(&(*I));
        continue;
      }
      I->marked = false;
    }
  }

  ShProgramNodeCPtr p;
  DeadCodeWorkList& w;
};

struct DeadCodeRemover {
  DeadCodeRemover(bool& changed)
    : changed(changed)
  {
  }
  
  // assignment operator could not be generated: declaration only
  DeadCodeRemover& operator=(DeadCodeRemover const&);

  void operator()(const ShCtrlGraphNodePtr& node) {
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


// Copy propagation

struct CopyPropagator {
  CopyPropagator(bool& changed)
    : changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  CopyPropagator& operator=(CopyPropagator const&);

  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      for (int i = 0; i < opInfo[I->op].arity; i++) copyValue(I->src[i]);
      removeACP(I->dest);
      
      if (I->op == SH_OP_ASN
          && I->dest.node() != I->src[0].node()
          && I->dest.node()->kind() == SH_TEMP
          && I->dest.swizzle().identity()
          && I->src[0].swizzle().identity()

		  // added to preserve casts
          && (I->dest.valueType() == I->src[0].valueType())) {
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
        var = ShVariable(I->second.node(), var.swizzle(),
                         var.neg() ^ (I->first.neg() ^ I->second.neg()));
        break;
      }
    }
  }

  typedef std::list< std::pair<ShVariable, ShVariable> > ACP;
  ACP m_acp;
  
  bool& changed;
};


/// Determine whether node is used in the RHS of stmt
bool inRHS(const ShVariableNodePtr& node,
           const ShStatement& stmt)
{
  for (int i = 0; i < opInfo[stmt.op].arity; i++) {
    if (stmt.src[i].node() == node) return true;
  }
  
  return false;
}

struct ForwardSubst {
  ForwardSubst(bool& changed)
    : changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  ForwardSubst& operator=(ForwardSubst const&);

  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin();
         I != block->end(); ++I) {
      if(I->dest.null()) continue;
      substitute(*I);
      
      removeAME(I->dest.node());
      
      if (!inRHS(I->dest.node(), *I)
          && I->dest.node()->kind() == SH_TEMP
          && I->dest.swizzle().identity()) {
        m_ame.push_back(*I);
      }
    }
    m_ame.clear();
  }
  
  void substitute(ShStatement& stmt)
  {
    if (stmt.op != SH_OP_ASN) return;
    if (stmt.src[0].neg()) return;
    if (stmt.src[0].node()->kind() != SH_TEMP) return;
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

  void removeAME(const ShVariableNodePtr& node)
  {
    for (AME::iterator I = m_ame.begin(); I != m_ame.end();) {
      if (I->dest.node() == node || inRHS(node, *I)) {
        I = m_ame.erase(I);
        continue;
      }
      ++I;
    }
  }

  bool& changed;
  typedef std::list<ShStatement> AME;
  AME m_ame;
};

}

namespace SH {

void insert_branch_instructions(ShProgram& p)
{
  BraInstInserter r;
  p.node()->ctrlGraph->dfs(r);
}

void remove_branch_instructions(ShProgram& p)
{
  BraInstRemover r;
  p.node()->ctrlGraph->dfs(r);
}

void remove_redundant_edges(ShProgram& p, bool& changed)
{
  RedundantEdgeRemover r(changed);
  p.node()->ctrlGraph->dfs(r);
}

void straighten(ShProgram& p, bool& changed)
{
  Straightener s(p.node()->ctrlGraph, changed);
  p.node()->ctrlGraph->dfs(s);
}

void remove_empty_blocks(ShProgram& p, bool& changed)
{
  EmptyBlockRemover e(p.node()->ctrlGraph, changed);
  p.node()->ctrlGraph->dfs(e);
  e.finish();
}

void remove_dead_code(ShProgram& p, bool& changed)
{
  DeadCodeWorkList w;

  ShCtrlGraphPtr graph = p.node()->ctrlGraph;
  
  InitLiveCode init(p.node(), w);
  graph->dfs(init);

  while (!w.empty()) {
    ShStatement* stmt = w.front(); w.pop();
    ValueTracking* vt = stmt->get_info<ValueTracking>();
    if (!vt) continue; // Should never happen!
    
    for (int i = 0; i < opInfo[stmt->op].arity; i++) {
      
      for (ValueTracking::TupleUseDefChain::iterator C = vt->defs[i].begin();
           C != vt->defs[i].end(); ++C) {
        for (ValueTracking::UseDefChain::iterator I = C->begin(); I != C->end(); ++I) {
          if (I->kind != ValueTracking::Def::STMT) continue;
          if (I->stmt->marked) continue;
          I->stmt->marked = true;
          w.push(I->stmt);
        }
      }
    }
  }

  DeadCodeRemover r(changed);
  graph->dfs(r);
}

void copy_propagate(ShProgram& p, bool& changed)
{
  CopyPropagator c(changed);
  p.node()->ctrlGraph->dfs(c);
}

void forward_substitute(ShProgram& p, bool& changed)
{
  ForwardSubst f(changed);
  p.node()->ctrlGraph->dfs(f);
}

void optimize(ShProgram& p, int level)
{
  if (level <= 0) return;

  p.node()->collectDecls();
#ifdef SH_DEBUG_OPTIMIZER
  SH_DEBUG_PRINT("After collecting declarations");
  SH_DEBUG_PRINT(p.node()->describe_decls());
#endif

#ifdef SH_DEBUG_OPTIMIZER
  int pass = 0;
  SH_DEBUG_PRINT("Begin optimization for program with target " << p.node()->target());
#endif
  
  bool changed;
  do {

#ifdef SH_DEBUG_OPTIMIZER
    SH_DEBUG_PRINT("---Optimizer pass " << pass << " BEGIN---");
    std::ostringstream s;
    s << "opt_" << pass;
    std::string filename = s.str() + ".dot";
    std::ofstream out(filename.c_str());
    p.node()->ctrlGraph->graphvizDump(out);
    out.close();
    std::string cmdline = std::string("dot -Tps -o ") + s.str() + ".ps " + s.str() + ".dot";
    system(cmdline.c_str());
#endif

    changed = false;

    if (!ShContext::current()->optimization_disabled("copy propagation")) {
      copy_propagate(p, changed);
    }
    if (!ShContext::current()->optimization_disabled("forward substitution")) {
      forward_substitute(p, changed);
    }
    
    p.node()->ctrlGraph->computePredecessors();

    if (!ShContext::current()->optimization_disabled("remove_empty_blocks")) {
      remove_empty_blocks(p, changed);
    }

    if (!ShContext::current()->optimization_disabled("straightening")) {
      straighten(p, changed);
    }

    if (!ShContext::current()->optimization_disabled("remove_redundant_edges")) {
      remove_redundant_edges(p, changed);
    }

    insert_branch_instructions(p);

    if (level >= 2 &&
        !ShContext::current()->optimization_disabled("propagation")) {
      add_value_tracking(p);
      propagate_constants(p);
    }

    if (!ShContext::current()->optimization_disabled("deadcode")) {
      add_value_tracking(p); // TODO: Really necessary?
      remove_dead_code(p, changed);
    }

    remove_branch_instructions(p);

#ifdef SH_DEBUG_OPTIMIZER
    SH_DEBUG_PRINT("---Optimizer pass " << pass << " END---");
    pass++;
#endif
  } while (changed);
  p.node()->collectVariables();
}

void optimize(const ShProgramNodePtr& n, int level)
{
  ShProgram p(n);
  optimize(p, level);
}

void optimize(ShProgram& p)
{
  optimize(p, ShContext::current()->optimization());
}

void optimize(const ShProgramNodePtr& n)
{
  ShProgram p(n);
  optimize(p);
}

}
