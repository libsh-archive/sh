#include "ShOptimizations.hpp"
#include <map>
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
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
               int offset)
      : stmt(stmt), node(node), offset(offset)
    {
    }
    
    ShStatement* stmt;
    ShCtrlGraphNodePtr node;
    int offset;
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
    SH_DEBUG_PRINT("Finding a definition");
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      SH_DEBUG_PRINT("Considering definition");
      if (I->op != SH_OP_KIL && I->op != SH_OP_OPTBRA && I->dest.node()->kind() == SH_TEMP) {
        SH_DEBUG_PRINT("Marking definition");
        r.defs.push_back(ReachingDefs::Definition(&(*I), node, offset));
        offset += I->dest.size();
        r.defsize += I->dest.size();
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
        for (unsigned int j = 0; j < r.defs[i].stmt->dest.size(); j++) {
          r.gen[node][r.defs[i].offset + j] = true;
        }
      }
    }

    // Initialize prsv
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA
          || I->dest.node()->kind() != SH_TEMP) continue;
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
  
}

namespace SH {

ValueTracking::ValueTracking(ShStatement* stmt)
  : uses(stmt->dest.node() ? stmt->dest.size() : 0)
{
  for (int i = 0; i < opInfo[stmt->op].arity; i++) {
    for (int j = 0; j < (stmt->src[i].node() ? stmt->src[i].size() : 0); j++) {
      defs[i].push_back(std::set<Def>());
    }
  }
}

void add_value_tracking(ShProgram& p)
{
  ReachingDefs r;

  ShCtrlGraphPtr graph = p.node()->ctrlGraph;
  graph->computePredecessors();
  
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

#if 1
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
  
}

}
