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

// Straightening
struct Straightener {
  Straightener(const ShCtrlGraphPtr& graph, bool& changed)
    : graph(graph), changed(changed)
  {
  }

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    if (!node->follower) return;
    if (node == graph->entry()) return;
    if (node->follower == graph->exit()) return;
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
  
  ShCtrlGraphPtr graph;
  bool& changed;
};

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
      if (I->dest.node()->kind() != SH_TEMP
          || I->dest.node()->uniform()
          || I->op == SH_OP_KIL
          /*
          || I->op == SH_OP_FETCH // Keep stream fetches, since these
                                  // are like inputs.
          */
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


// Copy propagation

struct CopyPropagator {
  CopyPropagator(bool& changed)
    : changed(changed)
  {
  }

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

  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin();
         I != block->end(); ++I) {
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

void straighten(ShProgram& p, bool& changed)
{
  Straightener s(p.node()->ctrlGraph, changed);
  p.node()->ctrlGraph->dfs(s);
}

void remove_dead_code(ShProgram& p, bool& changed)
{
  DeadCodeWorkList w;

  ShCtrlGraphPtr graph = p.node()->ctrlGraph;
  
  InitLiveCode init(w);
  graph->dfs(init);

  while (!w.empty()) {
    ShStatement* stmt = w.front(); w.pop();
    ValueTracking* vt = stmt->template get_info<ValueTracking>();
    if (!vt) continue; // Should never happen!
    
    for (int i = 0; i < opInfo[stmt->op].arity; i++) {
      
      for (ValueTracking::TupleUseDefChain::iterator C = vt->defs[i].begin();
           C != vt->defs[i].end(); ++C) {
        for (ValueTracking::UseDefChain::iterator I = C->begin(); I != C->end(); ++I) {
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

    copy_propagate(p, changed);
    forward_substitute(p, changed);
    
    p.node()->ctrlGraph->computePredecessors();

    straighten(p, changed);
    
    insert_branch_instructions(p);

    add_value_tracking(p);
    propagate_constants(p);

    add_value_tracking(p); // TODO: Really necessary?
    remove_dead_code(p, changed);

    remove_branch_instructions(p);

#ifdef SH_DEBUG_OPTIMIZER
    SH_DEBUG_PRINT("---Optimizer pass " << pass << " END---");
    pass++;
#endif
  } while (changed);
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
