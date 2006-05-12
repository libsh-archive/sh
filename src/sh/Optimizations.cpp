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
#include "Optimizations.hpp"
#include <map>
#include <set>
#include <utility>
#include "BitSet.hpp"
#include "CtrlGraph.hpp"
#include "Debug.hpp"
#include "Evaluate.hpp"
#include "Context.hpp"
#include "Syntax.hpp"
#include <sstream>
#include <fstream>

//#define SH_DEBUG_OPTIMIZER

namespace {

using namespace SH;

// Branch instruction insertion/removal

struct BraInstInserter {
  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;

    for (CtrlGraphNode::SuccessorIt I = node->successors_begin();
         I != node->successors_end(); ++I) {
      if (!node->block) node->block = new BasicBlock();
      node->block->addStatement(Statement(I->cond, OP_OPTBRA, I->cond));
    }
  }
};

struct BraInstRemover {
  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end();) {
      if (I->op == OP_OPTBRA) {
        I = block->erase(I);
        continue;
      }
      ++I;
    }
  }
};

// Straightening
struct Straightener {
  Straightener(const CtrlGraphPtr& graph, bool& changed)
    : graph(graph), changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  Straightener& operator=(Straightener const&);

  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;
    if (!node->follower()) return;
    if (node == graph->entry()) return;
    if (node->follower() == graph->exit()) return;
    if (!node->successors_empty()) return;
    if (node->follower()->predecessors_size() > 1) return;

    // conditions added for maintaining section START/END marker invariants
    // node cannot end in an ENDSEC, node->follower() cannot start with a STARTSEC
    if (node->block && !node->block->empty() && node->block->rbegin()->op == OP_ENDSEC) return; 
    if (node->follower()->block && !node->follower()->block->empty() && node->follower()->block->begin()->op == OP_STARTSEC) return; 
    
    if (!node->block) node->block = new BasicBlock();
    if (!node->follower()->block) node->follower()->block = new BasicBlock();

    // Merge statements
    for (BasicBlock::StmtList::iterator I = node->follower()->block->begin(); I != node->follower()->block->end(); ++I) {
      node->block->addStatement(*I);
    }

    // Merge in declarations
    node->insert_decls(node->follower()->decl_begin(), node->follower()->decl_end());

    // Move successors
    for (CtrlGraphNode::SuccessorIt i = node->follower()->successors_begin();
         i != node->follower()->successors_end();) {
      node->append(i->node, i->cond);
      i = node->follower()->successors_erase(i);
    }

    // Update our follower (this should also release the previous one)
    node->follower(node->follower()->follower());

    changed = true;
  }
  
  CtrlGraphPtr graph;
  bool& changed;
};


// Remove empty blocks
struct EmptyBlockRemover {
  EmptyBlockRemover(const CtrlGraphPtr& graph, bool& changed)
    : graph(graph), changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  EmptyBlockRemover& operator=(EmptyBlockRemover const&);

  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;
    if (!node->follower()) return;
    if (node == graph->entry()) return;
    if (!node->successors_empty()) return;

    // TODO: section stuff?
    if (node->block && !node->block->empty()) return;

    to_remove.push_back(node);
  }

  void finish()
  {
    for (std::list<CtrlGraphNode*>::iterator I = to_remove.begin(); I != to_remove.end(); ++I) {
      remove(*I);
    }
  }

  void remove(CtrlGraphNode* node)
  {
    // Drop this node from the graph
    graph->release_owned_node(node);
    changed = true;
  }

  CtrlGraphPtr graph;
  bool& changed;

  std::list<CtrlGraphNode*> to_remove;
};


// Remove redundant edges
struct RedundantEdgeRemover {
  RedundantEdgeRemover(bool& changed)
    : changed(changed)
  {
  }

  // assignment operator could not be generated: declaration only
  RedundantEdgeRemover& operator=(RedundantEdgeRemover const&);

  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;
    if (!node->follower()) return;
    if (node->successors_empty()) return;

    for (CtrlGraphNode::SuccessorIt i = node->successors_begin();
         i != node->successors_end();) {
      if (i->node == node->follower()) {
        i = node->successors_erase(i);
      } else {
        ++i;
      }
    }
  }

  bool& changed;
};

typedef std::queue<Statement*> DeadCodeWorkList;

struct InitLiveCode {
  InitLiveCode(const ProgramNodeCPtr& p, DeadCodeWorkList& w)
    : p(p), w(w)
  {
  }
  
  // assignment operator could not be generated: declaration only
  InitLiveCode& operator=(InitLiveCode const&);

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;

    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
#ifdef SH_DEBUG_OPTIMIZER
      if(!I->dest.null() && I->dest.node()->kind() == SH_TEMP && !p->hasDecl(I->dest.node())) {
        std::cerr << "No decl for " << I->dest.name() << std::endl;
      }
#endif
      // @todo range check if IGNORE is right
      // maybe use a different flag
      // @todo range conditions here are probably too conservative for 
      // inputs, outputs used in computation -> use the valuetracking
      if (opInfo[I->op].result_source == OperationInfo::IGNORE
          || I->dest.node()->kind() != SH_TEMP
          || I->dest.node()->uniform()
//          || !p->hasDecl(I->dest.node())
          || I->op == OP_OPTBRA) {
        I->marked = true;
        w.push(&(*I));
        continue;
      }
      I->marked = false;
    }
  }

  ProgramNodeCPtr p;
  DeadCodeWorkList& w;
};

struct DeadCodeRemover {
  DeadCodeRemover(bool& changed)
    : changed(changed)
  {
  }
  
  // assignment operator could not be generated: declaration only
  DeadCodeRemover& operator=(DeadCodeRemover const&);

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end();) {
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

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      for (int i = 0; i < opInfo[I->op].arity; i++) copyValue(I->src[i]);
      removeACP(I->dest);
      
      if (I->op == OP_ASN
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

  void removeACP(const Variable& var)
  {
    for (ACP::iterator I = m_acp.begin(); I != m_acp.end();) {
      if (I->first.node() == var.node() || I->second.node() == var.node()) {
        I = m_acp.erase(I);
        continue;
      }
      ++I;
    }
  }
  

  void copyValue(Variable& var)
  {
    for (ACP::const_iterator I = m_acp.begin(); I != m_acp.end(); ++I) {
      if (I->first.node() == var.node()) {
        changed = true;
        var = Variable(I->second.node(), var.swizzle(),
                         var.neg() ^ (I->first.neg() ^ I->second.neg()));
        break;
      }
    }
  }

  typedef std::list< std::pair<Variable, Variable> > ACP;
  ACP m_acp;
  
  bool& changed;
};


/// Determine whether node is used in the RHS of stmt
bool inRHS(const VariableNodePtr& node,
           const Statement& stmt)
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

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin();
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
  
  void substitute(Statement& stmt)
  {
    if (stmt.op != OP_ASN) return;
    if (stmt.src[0].neg()) return;
    if (stmt.src[0].node()->kind() != SH_TEMP) return;
    if (!stmt.src[0].swizzle().identity()) return;
    // Added to preserve casts
    if (stmt.dest.valueType() != stmt.src[0].valueType()) return;

    for (AME::const_iterator I = m_ame.begin(); I != m_ame.end(); ++I) {
      if (I->dest.node() == stmt.src[0].node()) {
        Variable v = stmt.dest;
        stmt = *I;
        stmt.dest = v;
        changed = true;
        break;
      }
    }
  }

  void removeAME(const VariableNodePtr& node)
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
  typedef std::list<Statement> AME;
  AME m_ame;
};

struct ForwardPlacement {
  
  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin();
         I != block->end(); ++I) {
      if(I->dest.null()) continue;

      for (MovableList::iterator J = m_movable.begin(); 
           J != m_movable.end();) {
        if (inRHS((*J)->dest.node(), *I) || 
            inRHS(I->dest.node(), **J)) {
          block->m_statements.insert(I, **J);
          block->m_statements.erase(*J);
          J = m_movable.erase(J);
        }
        else
          ++J;
      }
      m_movable.push_back(I);
    }
    m_movable.clear();
  }
  
  typedef std::list<BasicBlock::StmtList::iterator> MovableList;
  MovableList m_movable;
};

}

namespace SH {

void insert_branch_instructions(Program& p)
{
  BraInstInserter r;
  p.node()->ctrlGraph->dfs(r);
}

void remove_branch_instructions(Program& p)
{
  BraInstRemover r;
  p.node()->ctrlGraph->dfs(r);
}

void remove_redundant_edges(Program& p, bool& changed)
{
  RedundantEdgeRemover r(changed);
  p.node()->ctrlGraph->dfs(r);
}

void straighten(Program& p, bool& changed)
{
  Straightener s(p.node()->ctrlGraph, changed);
  p.node()->ctrlGraph->dfs(s);
}

void remove_empty_blocks(Program& p, bool& changed)
{
  EmptyBlockRemover e(p.node()->ctrlGraph, changed);
  p.node()->ctrlGraph->dfs(e);
  e.finish();
}

void remove_dead_code(Program& p, bool& changed)
{
  DeadCodeWorkList w;

  CtrlGraphPtr graph = p.node()->ctrlGraph;
  
  InitLiveCode init(p.node(), w);
  graph->dfs(init);

  while (!w.empty()) {
    Statement* stmt = w.front(); w.pop();
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

void copy_propagate(Program& p, bool& changed)
{
  CopyPropagator c(changed);
  p.node()->ctrlGraph->dfs(c);
}

void forward_substitute(Program& p, bool& changed)
{
  ForwardSubst f(changed);
  p.node()->ctrlGraph->dfs(f);
}

void forward_placement(Program& p)
{
  ForwardPlacement r;
  p.node()->ctrlGraph->dfs(r);
}

void optimize(Program& p, int level)
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
    p.node()->ctrlGraph->graphviz_dump(out);
    out.close();
    std::string cmdline = std::string("dot -Tps -o ") + s.str() + ".ps " + s.str() + ".dot";
    system(cmdline.c_str());
#endif

    changed = false;

    if (!Context::current()->optimization_disabled("copy propagation")) {
      copy_propagate(p, changed);
    }
    if (!Context::current()->optimization_disabled("forward substitution")) {
      forward_substitute(p, changed);
    }

    if (!Context::current()->optimization_disabled("remove_empty_blocks")) {
      remove_empty_blocks(p, changed);
    }

    if (!Context::current()->optimization_disabled("straightening")) {
      straighten(p, changed);
    }

    if (!Context::current()->optimization_disabled("remove_redundant_edges")) {
      remove_redundant_edges(p, changed);
    }

    insert_branch_instructions(p);

    if (level >= 2 &&
        !Context::current()->optimization_disabled("propagation")) {
      add_value_tracking(p);
      propagate_constants(p);
    }

    if (!Context::current()->optimization_disabled("deadcode")) {
      add_value_tracking(p); // TODO: Really necessary?
      remove_dead_code(p, changed);
    }

    remove_branch_instructions(p);

    if (!Context::current()->optimization_disabled("forward placement")) {
      forward_placement(p);
    }

#ifdef SH_DEBUG_OPTIMIZER
    SH_DEBUG_PRINT("---Optimizer pass " << pass << " END---");
    pass++;
#endif
  } while (changed);
  p.node()->collectVariables();
}

void optimize(const ProgramNodePtr& n, int level)
{
  Program p(n);
  optimize(p, level);
}

void optimize(Program& p)
{
  optimize(p, Context::current()->optimization());
}

void optimize(const ProgramNodePtr& n)
{
  Program p(n);
  optimize(p);
}

}
