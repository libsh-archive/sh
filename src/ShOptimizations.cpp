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

// Uncomment this to turn on optimizer debugging using dot.
// #define SH_DEBUG_OPTIMIZER

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
          if (stmt->src[i].uniform()) {
            src[i].push_back(Cell(Cell::UNIFORM, stmt->src[i], j));
          } else {
            src[i].push_back(Cell(Cell::TOP));
          }
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
      }
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::LINEAR) {
      // Consider each tuple element in turn.
      // Dest and sources are guaranteed to be of the same length.
      // Except that sources might be scalar.
      for (int i = 0; i < stmt->dest.size(); i++) {
        bool alluniform = true;
        bool allconst = true;
        for (int s = 0; s < opInfo[stmt->op].arity; s++) {
          if (src[s][idx(i,s)].state != Cell::CONSTANT) {
            allconst = false;
          }
          if (src[s][idx(i,s)].state != Cell::UNIFORM) {
            alluniform = false;
          }
        }
        if (allconst) {
          ShVariable tmpdest(new ShVariableNode(SH_CONST, 1));
          ShStatement eval(*stmt);
          eval.dest = tmpdest;
          for (int k = 0; k < opInfo[stmt->op].arity; k++) {
            ShVariable tmpsrc(new ShVariableNode(SH_CONST, 1));
            tmpsrc.setValue(0, src[k][idx(i,k)].value);
            eval.src[k] = tmpsrc;
          }
          evaluate(eval);
          dest.push_back(Cell(Cell::CONSTANT, tmpdest.getValue(0)));
          worklist.push(ValueTracking::Def(stmt, i));
        } else if (alluniform) {
          dest.push_back(Cell(Cell::UNIFORM, this, i));
          worklist.push(ValueTracking::Def(stmt, i));
        } else {
          dest.push_back(Cell(Cell::BOTTOM));
        }
      }      
    } else if (opInfo[stmt->op].result_source == ShOperationInfo::ALL) {
      // build statement ONLY if ALL elements of ALL sources are constant
      bool allconst = true;
      bool alluniform = true;
      for (int s = 0; s < opInfo[stmt->op].arity; s++) {
        for (unsigned int k = 0; k < src[s].size(); k++) {
          if (src[s][k].state != Cell::CONSTANT) {
            allconst = false;
          }
          if (src[s][k].state != Cell::UNIFORM) {
            alluniform = false;
          }
        }
      }
      if (allconst) {
        ShVariable tmpdest(new ShVariableNode(SH_CONST, stmt->dest.size()));
        ShStatement eval(*stmt);
        eval.dest = tmpdest;
        for (int i = 0; i < opInfo[stmt->op].arity; i++) {
          ShVariable tmpsrc(new ShVariableNode(SH_CONST, stmt->src[i].size()));
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
      } else if (alluniform) {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::UNIFORM, this, i));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      } else {
        for (int i = 0; i < stmt->dest.size(); i++) {
          dest.push_back(Cell(Cell::BOTTOM));
          worklist.push(ValueTracking::Def(stmt, i));
        }
      }
    } else {
      SH_DEBUG_ASSERT(0 && "Invalid result source type");
    }
  }

  typedef int ValueNum;

  struct Uniform {
    Uniform()
      : valuenum(-1)
    {
    }
    
    Uniform(int valuenum, int index, bool neg)
      : valuenum(valuenum), index(index), neg(neg)
    {
    }

    bool operator==(const Uniform& other) const
    {
      if (valuenum != other.valuenum) return false;
      if (index != other.index) return false;
      if (neg != other.neg) return false;

      return true;
    }

    bool operator!=(const Uniform& other) const
    {
      return !(*this == other);
    }

    ValueNum valuenum;
    int index;
    bool neg;
  };

  class Value {
  public:
    enum Type {
      NODE,
      STMT
    };

    Type type;
    
    // Only for type == NODE:
    ShVariableNodePtr node;

    // Only for type == STMT:
    ShOperation op;
    int destsize;
    std::vector<Uniform> src[3];

    static void clear()
    {
      m_values.clear();
    }
    
    static ValueNum lookup(const ShVariableNodePtr& node)
    {
      for (int i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type == NODE && m_values[i]->node == node) return i;
      }
      m_values.push_back(new Value(node));
      return m_values.size() - 1;
    }

    static ValueNum lookup(ConstProp* cp)
    {
      for (int i = 0; i < m_values.size(); i++) {
        if (m_values[i]->type != STMT) continue;
        if (m_values[i]->op != cp->stmt->op) continue;

        bool mismatch = false;
        for (int j = 0; j < opInfo[cp->stmt->op].arity; j++) {
          if (cp->src[j].size() != m_values[i]->src[j].size()) {
            mismatch = true;
            break;
          }
          
          for (int k = 0; k < cp->src[j].size(); k++) {
            if (cp->src[j][k].uniform != m_values[i]->src[j][k]) {
              mismatch = true;
              break;
            }
          }
          if (mismatch) break;
        }
        if (!mismatch) return i;
      }
      m_values.push_back(new Value(cp));
      return m_values.size() - 1;
    }

    static Value* get(ValueNum n)
    {
      return m_values[n];
    }

    static void dump(std::ostream& out);
    
  private:
    Value(const ShVariableNodePtr& node)
      : type(NODE), node(node)
    {
    }

    Value(ConstProp* cp)
      : type(STMT), node(0), op(cp->stmt->op), destsize(cp->stmt->dest.size())
    {
      for (int i = 0; i < opInfo[cp->stmt->op].arity; i++) {
        for (int j = 0; j < cp->src[i].size(); j++) {
          src[i].push_back(cp->src[i][j].uniform);
        }
      }
    }
    
    static std::vector<Value*> m_values;
  };
  
  struct Cell {
    enum State {
      BOTTOM,
      CONSTANT,
      UNIFORM,
      TOP
    };

    Cell(State state, float value = 0.0)
      : state(state), value(value)
    {
    }

    Cell(State state, const ShVariable& var, int index) 
      : state(state), value(0.0),
        uniform(Value::lookup(var.node()), var.swizzle()[index], var.neg())
    {
    }

    Cell(State state, ConstProp* cp, int index) 
      : state(state), value(0.0),
        uniform(Value::lookup(cp), index, false)
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
    float value; // Only for state == CONSTANT

    Uniform uniform; // Only for state == UNIFORM
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
  if (a.state != b.state
      && (a.state != ConstProp::Cell::TOP && b.state != ConstProp::Cell::TOP)) {
    return ConstProp::Cell(ConstProp::Cell::BOTTOM);
  }
  // At this point either the cells are the same or one of them is
  // top.
  if (a.state == b.state) {
    if (a.state == ConstProp::Cell::CONSTANT) {
      if (a.value == b.value) {
        return a;
      } else {
        return ConstProp::Cell(ConstProp::Cell::BOTTOM);
      }
    }
    if (a.state == ConstProp::Cell::UNIFORM) {
      if (a.uniform == b.uniform) return a;
      return ConstProp::Cell(ConstProp::Cell::BOTTOM);
    }
  }

  if (a.state != ConstProp::Cell::TOP) return a;
  if (b.state != ConstProp::Cell::TOP) return b;
    
  return ConstProp::Cell(ConstProp::Cell::TOP);
}

std::vector<ConstProp::Value*> ConstProp::Value::m_values = std::vector<ConstProp::Value*>();

std::ostream& operator<<(std::ostream& out, const ConstProp::Uniform& uniform)
{
  if (uniform.neg) out << '-';
  out << "v" << uniform.valuenum << "[" << uniform.index << "]";
  return out;
}

void ConstProp::Value::dump(std::ostream& out)
{
  out << "--- uniform values ---" << std::endl;
  for (int i = 0; i < m_values.size(); i++) {
    out << i << ": ";
    if (m_values[i]->type == NODE) {
      out << "node " << m_values[i]->node->name() << std::endl;
    } else if (m_values[i]->type == STMT) {
      out << "stmt " << opInfo[m_values[i]->op].name << " ";
      for (int j = 0; j < opInfo[m_values[i]->op].arity; j++) {
        if (j) out << ", ";
        for (std::vector<Uniform>::iterator U = m_values[i]->src[j].begin();
             U != m_values[i]->src[j].end(); ++U) {
          out << *U;
        }
      }
      out << ";" << std::endl;
    }
  }
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
  case ConstProp::Cell::UNIFORM:
    out << "<" << cell.uniform << ">";
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
  FinishConstProp(bool lift_uniforms)
    : lift_uniforms(lift_uniforms)
  {
  }
  
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

        if (I->op != SH_OP_ASN || I->src[0].node()->kind() != SH_CONST) {
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
            //SH_DEBUG_PRINT("Replaced {" << *I << "} with " << newconst);
            *I = ShStatement(I->dest, SH_OP_ASN, newconst);
          } else {
            // otherwise, do the same for each source field.
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              if (I->src[s].node()->kind() == SH_CONST) continue;
            
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
                //SH_DEBUG_PRINT("Replaced {" << *I << "}.src[" << s << "] with " << newconst);
                I->src[s] = newconst;
              }
            }
          }

          if (!lift_uniforms) continue;
          
          bool alluniform = true;
          for (int s = 0; s < opInfo[I->op].arity; s++) {
            for (int i = 0; i < I->src[s].size(); i++) {
              if (cp->src[s][i].state != ConstProp::Cell::UNIFORM) {
                alluniform = false;
                break;
              }
            }
            if (alluniform) break; // We don't care about all-uniforms.
            // Except we might care in the case of output assignments.
          }
          if (!alluniform) {
            for (int s = 0; s < opInfo[I->op].arity; s++) {
              bool mixed = false;
              bool neg = false;
              ConstProp::ValueNum uniform = -1;
              std::vector<int> indices;
              
              for (int i = 0; i < I->src[s].size(); i++) {
                if (cp->src[s][i].state == ConstProp::Cell::UNIFORM) {
                  if (uniform < 0) {
                    uniform = cp->src[s][i].uniform.valuenum;
                    neg = cp->src[s][i].uniform.neg;
                    indices.push_back(cp->src[s][i].uniform.index);
                  } else {
                    if (uniform != cp->src[s][i].uniform.valuenum
                        || neg != cp->src[s][i].uniform.neg) {
                      mixed = true;
                      break;
                    }
                    indices.push_back(cp->src[s][i].uniform.index);
                  }
                }
              }
              
              
              if (mixed) continue;
              if (uniform < 0) continue;
              ConstProp::Value* value = ConstProp::Value::get(uniform);

              if (value->type == ConstProp::Value::NODE) continue;

              SH_DEBUG_PRINT("Lifting uniform with " << indices.size() << " indices");
              
              ShSwizzle swizzle(value->destsize, indices.size(), &(*indices.begin()));
              SH_DEBUG_PRINT("Swizzle = " << swizzle);
              // Build a uniform to represent this computation.
              ShVariableNodePtr node = build_uniform(value, uniform);
              I->src[s] = ShVariable(node, swizzle, neg);
            }
          }
        }
      }

      // Remove constant propagation information.
      I->template destroy_info<ConstProp>();
    }
  }

  ShVariableNodePtr build_uniform(ConstProp::Value* value,
                                  ConstProp::ValueNum valuenum)
  {
    ShContext::current()->enter(0);
    ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize);
    ShContext::current()->exit();

    SH_DEBUG_PRINT("Lifting value #" << valuenum);
    
    ShProgram prg = SH_BEGIN_PROGRAM("uniform") {
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
    } SH_END;

    {
      std::ostringstream s;
      s << "lifted_" << valuenum;
      std::string dotfilename(s.str() + ".dot");
      std::ofstream dot(dotfilename.c_str());
      prg.node()->ctrlGraph->graphvizDump(dot);
      dot.close();
      std::string cmdline = std::string("dot -Tps -o ") + s.str() + ".ps " + s.str() + ".dot";
      system(cmdline.c_str());
    }

    node->attach(prg.node());

    value->type = ConstProp::Value::NODE;
    value->node = node;
    
    return node;
  }

  ShVariable compute(const std::vector<ConstProp::Uniform>& src)
  {
    ConstProp::ValueNum v = -1;
    bool allsame = true;
    bool neg = false;
    std::vector<int> indices;
    for (int i = 0; i < src.size(); i++) {
      if (v < 0) {
        v = src[i].valuenum;
        neg = src[i].neg;
      } else {
        if (v != src[i].valuenum || neg != src[i].neg) {
          allsame = false;
        }
      }
      indices.push_back(src[i].index);
    }
    if (!allsame) {
      // TODO: Make intermediate variables, combine them together.
      SH_DEBUG_PRINT("Source is not heterogenous");
      return ShVariable();
    }

    ConstProp::Value* value = ConstProp::Value::get(v);
    
    if (value->type == ConstProp::Value::NODE) {
      SH_DEBUG_PRINT("compute: Returning simple uniform with " << indices.size() << " indices");
      ShSwizzle swizzle(value->node->size(), indices.size(), &*indices.begin());
      SH_DEBUG_PRINT("compute: Swizzle = " << swizzle);
      return ShVariable(value->node, swizzle, neg);
    }
    if (value->type == ConstProp::Value::STMT) {
      ShVariableNodePtr node = new ShVariableNode(SH_TEMP, value->destsize);
      ShStatement stmt(node, value->op);

      for (int i = 0; i < opInfo[value->op].arity; i++) {
        stmt.src[i] = compute(value->src[i]);
      }

      ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
      SH_DEBUG_PRINT("compute: Returning computed uniform with " << indices.size() << " indices");
      ShSwizzle swizzle(node->size(), indices.size(), &*indices.begin());
      SH_DEBUG_PRINT("compute: Swizzle = " << swizzle);
      return ShVariable(node, swizzle, neg);
    }

    SH_DEBUG_PRINT("Reached invalid point");

    // Should never reach here.
    return ShVariable();
  }

  bool lift_uniforms;
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

#ifdef SH_DEBUG_OPTIMIZER
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

#ifdef SH_DEBUG_OPTIMIZER
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

  //ConstProp::Value::clear();
  
  InitConstProp init(worklist);
  graph->dfs(init);

  while (!worklist.empty()) {
    ValueTracking::Def def = worklist.front(); worklist.pop();
    ValueTracking* vt = def.stmt->template get_info<ValueTracking>();
    if (!vt) {
      //SH_DEBUG_PRINT("Statement on worklist does not have VT information?");
      //SH_DEBUG_PRINT(*def.stmt);
      continue;
    }


    for (ValueTracking::DefUseChain::iterator use = vt->uses[def.index].begin();
         use != vt->uses[def.index].end(); ++use) {
      ConstProp* cp = use->stmt->template get_info<ConstProp>();
      if (!cp) {
        //SH_DEBUG_PRINT("Use does not have const prop information!");
        continue;
      }

      ConstProp::Cell cell = cp->src[use->source][use->index];

      ValueTracking* ut = use->stmt->template get_info<ValueTracking>();
      if (!ut) {
        // Should never happen...
        //SH_DEBUG_PRINT("Statement on worklist does not have VT information?");
        //SH_DEBUG_PRINT(*def.stmt);
        continue;
      }

      ConstProp::Cell new_cell = cell;
      
      for (ValueTracking::UseDefChain::iterator possdef
             = ut->defs[use->source][use->index].begin();
           possdef != ut->defs[use->source][use->index].end(); ++possdef) {
        ConstProp* dcp = possdef->stmt->template get_info<ConstProp>();
        if (!dcp) {
          //SH_DEBUG_PRINT("Statement on worklist does not have CP information?");
          continue;
        }
        
        ConstProp::Cell destcell = dcp->dest[possdef->index];
        if (destcell.state == ConstProp::Cell::CONSTANT
            && use->stmt->src[use->source].neg()) {
          destcell.value = -destcell.value;
        }
        if (destcell.state == ConstProp::Cell::UNIFORM
            && use->stmt->src[use->source].neg()) {
          destcell.uniform.neg = !destcell.uniform.neg;
        }
        new_cell = meet(destcell, new_cell);
      }
      
      if (cell != new_cell) {
        cp->src[use->source][use->index] = new_cell;
        cp->updateDest(worklist);
      }
    }
  }
  // Now do something with our glorious newfound information.

  
  //#ifdef SH_DEBUG_OPTIMIZER
  ConstProp::Value::dump(std::cerr);

  DumpConstProp dump;
  graph->dfs(dump);
  //#endif
  
  FinishConstProp finish(p.node()->target() != "uniform");
  graph->dfs(finish);
  
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
#endif

  SH_DEBUG_PRINT("Begin optimization for program with target " << p.node()->target());
  
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
