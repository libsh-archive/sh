#include "ShOptimizations.hpp"
#include <map>
#include <set>
#include <utility>
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShEvaluate.hpp"

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
      if (I->op != SH_OP_KIL && I->op != SH_OP_OPTBRA && I->dest.node()->kind() == SH_TEMP) {
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
        if (I->src[j].node()->kind() == SH_TEMP) {
          ValueTracking* vt = I->template get_info<ValueTracking>();
          if (!vt) {
            vt = new ValueTracking(&(*I));
            I->add_info(vt);
          }
          for (int i = 0; i < I->src[j].size(); i++) {
            const DefSet& ds = defs[TupleElement(I->src[j].node(), I->src[j].swizzle()[i])];

            vt->defs[j][i] = ds;
            for (DefSet::iterator J = ds.begin(); J != ds.end(); J++) {
              ValueTracking* ut = J->stmt->template get_info<ValueTracking>();
              if (!ut) {
                ut = new ValueTracking(J->stmt);
                J->stmt->add_info(ut);
              }
              ut->uses[J->index].insert(ValueTracking::Use(&(*I), j, i));
            }
          }
        } 
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
      I->template destroy_info<ValueTracking>();
    }
  }
};

struct UdDuDumper {
  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      ValueTracking* vt = I->template get_info<ValueTracking>();
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
      if (I->dest.node()->kind() == SH_OUTPUT
          || I->dest.node()->kind() == SH_INOUT
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

typedef std::queue<ValueTracking::Def> ConstWorkList;

struct ConstProp : public ShStatementInfo {
  ConstProp(ShStatement* stmt,
            ConstWorkList& worklist)
    : stmt(stmt)
  {
    for (int i = 0; i < opInfo[stmt->op].arity ; i++) {
      for (int j = 0; j < stmt->src[i].size(); j++) {
        switch(stmt->src[i].node()->kind()) {
        case SH_INPUT:
        case SH_OUTPUT:
        case SH_INOUT:
        case SH_TEXTURE:
        case SH_STREAM:
          src[i].push_back(Cell(Cell::BOTTOM));
          break;
        case SH_TEMP:
          src[i].push_back(Cell(Cell::TOP));
          break;
        case SH_CONST:
          src[i].push_back(Cell(Cell::CONSTANT, stmt->src[i].getValue(j)));
          break;
        }
      }
    }
    updateDest(worklist);
  }

  ShStatementInfo* clone() const;

  int idx(int destindex, int source)
  {
    return (stmt->src[source].size() == 1 ? 0 : destindex);
  }

  void updateDest(ConstWorkList& worklist)
  {
    dest.clear();

    // Ignore KIL, optbra, etc.
    if (opInfo[stmt->op].result_source == ShOperationInfo::IGNORE) return;
    
    if (opInfo[stmt->op].result_source == ShOperationInfo::EXTERNAL) {
      // This statement never results in a constant
      // E.g. texture fetches, stream fetches.
      for (int i = 0; i < stmt->dest.size(); i++) {
        dest.push_back(Cell(Cell::BOTTOM));
        worklist.push(ValueTracking::Def(stmt, i));
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::LINEAR) {
      // Consider each tuple element in turn.
      // Dest and sources are guaranteed to be of the same length.
      // Except that sources might be scalar.
      for (int i = 0; i < stmt->dest.size(); i++) {
        bool allconst = true;
        bool bottom = false;
        for (int s = 0; s < opInfo[stmt->op].arity; s++) {
          if (src[s][idx(i,s)].state != Cell::CONSTANT) {
            allconst = false;
          }
          if (src[s][idx(i,s)].state == Cell::BOTTOM) {
            bottom = true;
          }
        }
        if (allconst) {
          ShVariable tmpdest(new ShVariableNode(SH_TEMP, 1));
          ShStatement eval(*stmt);
          eval.dest = tmpdest;
          for (int k = 0; k < opInfo[stmt->op].arity; k++) {
            ShVariable tmpsrc(new ShVariableNode(SH_TEMP, 1));
            tmpsrc.setValue(0, src[k][idx(i,k)].value);
            eval.src[k] = tmpsrc;
          }
          evaluate(eval);
          dest.push_back(Cell(Cell::CONSTANT, tmpdest.getValue(0)));
          worklist.push(ValueTracking::Def(stmt, i));
        } else if (bottom) {
          dest.push_back(Cell(Cell::BOTTOM));
          worklist.push(ValueTracking::Def(stmt, i));
        } else {
          dest.push_back(Cell(Cell::TOP));
        }
      }      
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::ALL) {
      // build statement ONLY if ALL elements of ALL sources are constant
      bool allconst = true;
      bool bottom = false;
      for (int s = 0; s < opInfo[stmt->op].arity; s++) {
        for (unsigned int k = 0; k < src[s].size(); k++) {
          if (src[s][k].state != Cell::CONSTANT) {
            allconst = false;
          }
          if (src[s][k].state == Cell::BOTTOM) {
            bottom = true;
          }
        }
      }
      if (allconst) {
        ShVariable tmpdest(new ShVariableNode(SH_TEMP, stmt->dest.size()));
        ShStatement eval(*stmt);
        eval.dest = tmpdest;
        for (int i = 0; i < opInfo[stmt->op].arity; i++) {
          ShVariable tmpsrc(new ShVariableNode(SH_TEMP, stmt->src[i].size()));
          for (int j = 0; j < stmt->src[i].size(); j++) {
            tmpsrc.setValue(j, src[i][j].value);
          }
          eval.src[i] = tmpsrc;
        }
        evaluate(eval);
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::CONSTANT, tmpdest.getValue(i)));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      } else if (bottom) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::BOTTOM));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      } else {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::TOP));
        }
      }
    } else {
      SH_DEBUG_ASSERT(0 && "Invalid result source type");
    }
  }
  
  struct Cell {
    enum State {
      BOTTOM,
      CONSTANT,
      TOP
    };

    Cell(State state, float value = 0.0)
      : state(state), value(value)
    {
    }

    bool operator==(const Cell& other) const
    {
      return state == other.state && value == other.value;
    }
    bool operator!=(const Cell& other) const
    {
      return !(*this == other);
    }
    
    State state;
    float value;
  };

  ShStatement* stmt;
  std::vector<Cell> dest;
  std::vector<Cell> src[3];
};

ShStatementInfo* ConstProp::clone() const
{
  return new ConstProp(*this);
}

ConstProp::Cell meet(const ConstProp::Cell& a, const ConstProp::Cell& b)
{
  if (a.state == ConstProp::Cell::BOTTOM || b.state == ConstProp::Cell::BOTTOM) {
    return ConstProp::Cell(ConstProp::Cell::BOTTOM);
  }
  if (a.state == ConstProp::Cell::CONSTANT && b.state == ConstProp::Cell::CONSTANT) {
    if (a.value == b.value) {
      return a;
    } else {
      return ConstProp::Cell(ConstProp::Cell::BOTTOM);
    }
  }
  if (a.state == ConstProp::Cell::CONSTANT) return a;
  if (b.state == ConstProp::Cell::CONSTANT) return b;
  
  return ConstProp::Cell(ConstProp::Cell::TOP);
}

std::ostream& operator<<(std::ostream& out, const ConstProp::Cell& cell)
{
  switch(cell.state) {
  case ConstProp::Cell::BOTTOM:
    out << "[bot]";
    break;
  case ConstProp::Cell::CONSTANT:
    out << "[" << cell.value << "]";
    break;
  case ConstProp::Cell::TOP:
    out << "[top]";
    break;
  }
  return out;
}

struct InitConstProp {
  InitConstProp(ConstWorkList& worklist)
    : worklist(worklist)
  {
  }

  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      I->template destroy_info<ConstProp>();
      ConstProp* cp = new ConstProp(&(*I), worklist);
      I->add_info(cp);
    }
  }

  ConstWorkList& worklist;
};

struct DumpConstProp {
  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      std::cerr << "{" << *I << "} --- ";
      ConstProp* cp = I->template get_info<ConstProp>();

      if (!cp) {
        std::cerr << "NO CP INFORMATION" << std::endl;
        continue;
      }

      std::cerr << "dest = {";
      for (int i = 0; i < cp->dest.size(); i++) {
        std::cerr << cp->dest[i];
      }
      std::cerr << "}; ";
      for (int s = 0; s < opInfo[I->op].arity; s++) {
        if (s) std::cerr << ", ";
        std::cerr << "src" << s << " = {";
        for (int i = 0; i < cp->src[s].size(); i++) {
          std::cerr << cp->src[s][i];
        }
        std::cerr << "}";
      }
      std::cerr << std::endl;
    }
    
  }
};

struct FinishConstProp
{
  void operator()(const ShCtrlGraphNodePtr& node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      ConstProp* cp = I->template get_info<ConstProp>();

      if (!cp) continue;

      if (!cp->dest.empty()) {
        // if all dest fields are constants, replace this with a
        // constant assignment
        
        ShVariable newconst(new ShVariableNode(SH_CONST, I->dest.size()));
        bool allconst = true;
        for (int i = 0; i < I->dest.size(); i++) {
          if (cp->dest[i].state != ConstProp::Cell::CONSTANT) {
            allconst = false;
            break;
          }
          newconst.setValue(i, cp->dest[i].value);
        }
        if (allconst) {
          *I = ShStatement(I->dest, SH_OP_ASN, newconst);
        } else {
          // otherwise, do the same for each source field.
          for (int s = 0; s < opInfo[I->op].arity; s++) {
            
            ShVariable newconst(new ShVariableNode(SH_CONST, I->src[s].size()));
            bool allconst = true;
            for (int i = 0; i < I->src[s].size(); i++) {
              if (cp->src[s][i].state != ConstProp::Cell::CONSTANT) {
                allconst = false;
                break;
              }
              newconst.setValue(i, cp->src[s][i].value);
            }
            if (allconst) {
              I->src[s] = newconst;
            }
          }
        }
      }

      // Remove constant propagation information.
      I->template destroy_info<ConstProp>();
    }
  }
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

ShStatementInfo* ValueTracking::clone() const
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

#if 0
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

#if 1
  UdDuDumper dumper;
  graph->dfs(dumper);
#endif
}

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

void propagate_constants(ShProgram& p)
{
  ShCtrlGraphPtr graph = p.node()->ctrlGraph;

  ConstWorkList worklist;
  
  InitConstProp init(worklist);
  graph->dfs(init);

  while (!worklist.empty()) {
    ValueTracking::Def def = worklist.front(); worklist.pop();
    ValueTracking* vt = def.stmt->template get_info<ValueTracking>();
    if (!vt) {
      SH_DEBUG_PRINT("Statement on worklist does not have VT information?");
      SH_DEBUG_PRINT(*def.stmt);
      continue;
    }

    ConstProp* dcp = def.stmt->template get_info<ConstProp>();
    if (!dcp) {
      SH_DEBUG_PRINT("Statement on worklist does not have CP information?");
      continue;
    }
    
    for (ValueTracking::DefUseChain::iterator I = vt->uses[def.index].begin(); I != vt->uses[def.index].end(); ++I) {
      ConstProp* cp = I->stmt->template get_info<ConstProp>();
      if (!cp) {
        SH_DEBUG_PRINT("Use does not have const prop information!");
        continue;
      }

      ConstProp::Cell cell = cp->src[I->source][I->index];
      ConstProp::Cell new_cell = meet(cell, dcp->dest[def.index]);
      if (cell != new_cell) {
        cp->src[I->source][I->index] = new_cell;
        cp->updateDest(worklist);
      }
    }
  }
  // Now do something with our glorious newfound information.

  DumpConstProp dump;
  graph->dfs(dump);
  
  FinishConstProp finish;
  graph->dfs(finish);
  
}

void optimize(ShProgram& p, int level)
{
  if (level <= 0) return;

  bool changed;
  do {
    SH_DEBUG_PRINT("===================Optimizer Pass Begins========================");
    changed = false;

    p.node()->ctrlGraph->computePredecessors();

    straighten(p, changed);
    
    insert_branch_instructions(p);

    add_value_tracking(p);
    propagate_constants(p);

    add_value_tracking(p); // TODO: Really necessary?
    remove_dead_code(p, changed);

    remove_branch_instructions(p);

    SH_DEBUG_PRINT("==================Optimizer Pass Finished=======================");
  } while (changed);
}


}
