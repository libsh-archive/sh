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

// Uncomment to enable use-def chain debugging 
// #define SH_DEBUG_VALUETRACK

#ifdef SH_DEBUG_OPTIMIZER
#ifndef SH_DEBUG_VALUETRACK
#define SH_DEBUG_VALUETRACK
#endif
#endif
namespace {
using namespace SH;


// Data related to doing reaching definitions
struct ReachingDefs {
  ReachingDefs()
    : defsize(0)
  {
  }
  
  struct Definition {
    Definition(ShStatement* stmt,
               const ShCtrlGraphNodePtr& node,
               int offset,
               ShBitSet disable_mask)
      : stmt(stmt), node(node), offset(offset),
        disable_mask(disable_mask)
    {
    }
    
    ShStatement* stmt;
    ShCtrlGraphNodePtr node;
    int offset;
    
    // Sometimes we want to ignore some of the elements, when
    // constructing gen, because they are overwritten by later
    // statements. In that case we mark the ignored components here.
    ShBitSet disable_mask;
  };

  typedef std::map<ShCtrlGraphNodePtr, int> SizeMap;
  typedef std::map<ShCtrlGraphNodePtr, ShBitSet> ReachingMap;
  
  std::vector<Definition> defs;
  int defsize;

  ReachingMap gen, prsv;
  ReachingMap rchin;
};

struct DefFinder {
  DefFinder(ReachingDefs& r)
    : r(r), offset(0)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    // Use this data structure to mark assignments to node elements,
    // so that we can ignore earlier assignments to them in this block.
    std::map<ShVariableNodePtr, ShBitSet> disable_map;

    // Iterate backwards over the list of statements...
    ShBasicBlock::ShStmtList::iterator I = block->end();
    while (1) {
      if (I == block->begin()) break;
      --I;
      if (I->op != SH_OP_KIL && I->op != SH_OP_OPTBRA /*&& I->dest.node()->kind() == SH_TEMP*/) {
        // Construct a disable map if this node has not yet been
        // assigned to in a later statement in this block.
        if (disable_map.find(I->dest.node()) == disable_map.end()) {
          disable_map[I->dest.node()] = ShBitSet(I->dest.node()->size());
        }

        // Construct a bitset containing all elements which this
        // definition affects.
        ShBitSet defn_map(I->dest.node()->size());
        for (int i = 0; i < I->dest.size(); i++) defn_map[I->dest.swizzle()[i]] = true;

        // Skip this definition if all its components have already
        // been defined in a later statement.
        if ((defn_map & disable_map[I->dest.node()]) == defn_map) continue;

        // Otherwise, add this definition
        r.defs.push_back(ReachingDefs::Definition(&(*I), node, offset,
                                                  disable_map[I->dest.node()]));
        offset += I->dest.size();
        r.defsize += I->dest.size();

        // And update the disable map for the destination node.
        disable_map[I->dest.node()] |= defn_map;
      }
    }
  }

  ReachingDefs& r;
  int offset;
};

struct InitRch {
  InitRch(ReachingDefs& r)
    : r(r)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;

    r.rchin[node] = ShBitSet(r.defsize);
    r.gen[node] = ShBitSet(r.defsize);
    r.prsv[node] = ~ShBitSet(r.defsize);
      
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    // Initialize gen
    for (unsigned int i = 0; i < r.defs.size(); i++) {
      if (r.defs[i].node == node) {
        for (int j = 0; j < r.defs[i].stmt->dest.size(); j++) {
          if (!r.defs[i].disable_mask[j]) {
            r.gen[node][r.defs[i].offset + j] = true;
          }
        }
      }
    }

    // Initialize prsv
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA
          /*|| I->dest.node()->kind() != SH_TEMP*/) continue;
      for (unsigned int i = 0; i < r.defs.size(); i++) {
        if (r.defs[i].stmt->dest.node() != I->dest.node()) continue;

        for (int j = 0; j < I->dest.size(); ++j) {
          for (int k = 0; k < r.defs[i].stmt->dest.size(); ++k) {
            if (r.defs[i].stmt->dest.swizzle()[k] == I->dest.swizzle()[j]) {
              r.prsv[node][r.defs[i].offset + k] = false;
            }
          }
        }
      }
    }
  }

  ReachingDefs& r;
};

struct IterateRch {
  IterateRch(ReachingDefs& r, bool& changed)
    : r(r), changed(changed)
  {
  }

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    SH_DEBUG_ASSERT(r.rchin.find(node) != r.rchin.end());
    ShBitSet newRchIn(r.defsize);
    
    for (ShCtrlGraphNode::ShPredList::iterator I = node->predecessors.begin();
         I != node->predecessors.end(); ++I) {
      SH_DEBUG_ASSERT(r.gen.find(*I) != r.gen.end());
      SH_DEBUG_ASSERT(r.prsv.find(*I) != r.prsv.end());
      SH_DEBUG_ASSERT(r.rchin.find(*I) != r.rchin.end());
      
      newRchIn |= (r.gen[*I] | (r.rchin[*I] & r.prsv[*I]));
    }
    if (newRchIn != r.rchin[node]) {
      r.rchin[node] = newRchIn;
      changed = true;
    }
  }

  ReachingDefs& r;
  bool& changed;
};

struct UdDuBuilder {
  UdDuBuilder(ReachingDefs& r)
    : r(r)
  {
  }

  struct TupleElement {
    TupleElement(const ShVariableNodePtr& node, int index)
      : node(node), index(index)
    {
    }

    bool operator<(const TupleElement& other) const
    {
      if (node < other.node) return true;
      if (node == other.node) return index < other.index;
      return false;
    }
    
    ShVariableNodePtr node;
    int index;
  };
  
  void operator()(ShCtrlGraphNodePtr node) {
    typedef std::set<ValueTracking::Def> DefSet;
    typedef std::map<TupleElement, DefSet> DefMap;

    // defs contains all of the possible contributors to the key's
    // definition at the current point.
    DefMap defs;
    
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    // TODO: Handle "non assigning" statements like KIL

    // initialize defs at the start of the block, using the reaching
    // definitions solution.
    for (std::size_t i = 0; i < r.defs.size(); i++) {
      for (int j = 0; j < r.defs[i].stmt->dest().size(); j++) {
        if (r.rchin[node][r.defs[i].offset + j]) {
          ValueTracking::Def def(r.defs[i].stmt, j);
          defs[TupleElement(r.defs[i].stmt->dest.node(),
                            r.defs[i].stmt->dest.swizzle()[j])].insert(def);
        }
      }
    }

    // Now consider each statement in turn.
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      // Compute the ud chains for the statement's source variables,
      // and contribute to the du chains of the source variables' definitions.
      for (int j = 0; j < opInfo[I->op].arity; j++) {
        //if (I->src[j].node()->kind() == SH_TEMP) {
          ValueTracking* vt = I->get_info<ValueTracking>();
          if (!vt) {
            vt = new ValueTracking(&(*I));
            I->add_info(vt);
          }
          for (int i = 0; i < I->src[j].size(); i++) {
            const DefSet& ds = defs[TupleElement(I->src[j].node(), I->src[j].swizzle()[i])];

            vt->defs[j][i] = ds;
            for (DefSet::const_iterator J = ds.begin(); J != ds.end(); J++) {
              ValueTracking* ut = J->stmt->get_info<ValueTracking>();
              if (!ut) {
                ut = new ValueTracking(J->stmt);
                J->stmt->add_info(ut);
              }
              ut->uses[J->index].insert(ValueTracking::Use(&(*I), j, i));
            }
          }
        //} 
      }

      // Now update the defs structure.
      for (int i = 0; i < I->dest.size(); ++i) {
        defs[TupleElement(I->dest.node(), I->dest.swizzle()[i])].clear();
        ValueTracking::Def def(&(*I), i);
        defs[TupleElement(I->dest.node(), I->dest.swizzle()[i])].insert(def);
      }
    }

  }

  ReachingDefs& r;
};

struct UdDuClearer {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      I->destroy_info<ValueTracking>();
    }
  }
};

struct UdDuDumper {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      ValueTracking* vt = I->get_info<ValueTracking>();
      if (!vt) {
        SH_DEBUG_PRINT(*I << " HAS NO VALUE TRACKING");
        continue;
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        //SH_DEBUG_PRINT("  ud[" << i << "]");
        int e = 0;
        for (ValueTracking::TupleUseDefChain::iterator E = vt->defs[i].begin();
             E != vt->defs[i].end(); ++E, ++e) {
          for (ValueTracking::UseDefChain::iterator J = E->begin(); J != E->end(); ++J) {
            SH_DEBUG_PRINT("{" << *I << "}.src" << i << "[" << e << "] comes from {" << *J->stmt << "}.dst[" << J->index << "]");
          }
        }
      }
      int e = 0;
      for (ValueTracking::TupleDefUseChain::iterator E = vt->uses.begin();
           E != vt->uses.end(); ++E, ++e) {
        for (ValueTracking::DefUseChain::iterator J = E->begin(); J != E->end(); ++J) {
          SH_DEBUG_PRINT("{" << *I << "}.dst[" << e << "] contributes to {" << *J->stmt << "}.src" << J->source << "[" << J->index << "]");
        }
      }
    }
  }
};

}

namespace SH {

ValueTracking::ValueTracking(ShStatement* stmt)
  : uses(stmt->dest.node() ? stmt->dest.size() : 0)
{
#ifdef SH_DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Adding value tracking to " << *stmt);
#endif
  for (int i = 0; i < opInfo[stmt->op].arity; i++) {
    for (int j = 0; j < (stmt->src[i].node() ? stmt->src[i].size() : 0); j++) {
      defs[i].push_back(std::set<Def>());
    }
  }
}

ShInfo* ValueTracking::clone() const
{
  return new ValueTracking(*this);
}

void add_value_tracking(ShProgram& p)
{
  ReachingDefs r;

  ShCtrlGraphPtr graph = p.node()->ctrlGraph;
  
  DefFinder finder(r);
  graph->dfs(finder);

  InitRch init(r);
  graph->dfs(init);

  bool changed;
  IterateRch iter(r, changed);
  do {
    changed = false;
    graph->dfs(iter);
  } while (changed);

#ifdef SH_DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Dumping Reaching Defs");
  SH_DEBUG_PRINT("defsize = " << r.defsize);
  SH_DEBUG_PRINT("defs.size() = " << r.defs.size());
  for(unsigned int i = 0; i < r.defs.size(); ++i) {
    SH_DEBUG_PRINT("  stmt[" << i << "]: " << *(r.defs[i].stmt));
    SH_DEBUG_PRINT("  node[" << i << "]: " << r.defs[i].node.object());
    SH_DEBUG_PRINT("offset[" << i << "]: " << r.defs[i].offset);
  }
  std::cerr << std::endl;

  for (ReachingDefs::ReachingMap::const_iterator I = r.rchin.begin(); I != r.rchin.end(); ++I) {
    ShCtrlGraphNodePtr node = I->first;
    SH_DEBUG_PRINT(" rchin[" << node.object() << "]: " << I->second);
    SH_DEBUG_PRINT("   gen[" << node.object() << "]: " << r.gen[I->first]);
    SH_DEBUG_PRINT("  prsv[" << node.object() << "]: " << r.prsv[I->first]);
    std::cerr << std::endl;
  }
#endif

  UdDuClearer clearer;
  graph->dfs(clearer);
  
  UdDuBuilder builder(r);
  graph->dfs(builder);

#ifdef SH_DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Uddu Dump");
  UdDuDumper dumper;
  graph->dfs(dumper);
#endif
}



}
