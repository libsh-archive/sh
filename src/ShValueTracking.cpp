#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ShOptimizations.hpp"
#include "ShBitSet.hpp"
#include "ShCtrlGraph.hpp"
#include "ShDebug.hpp"
#include "ShEvaluate.hpp"
#include "ShContext.hpp"
#include "ShSyntax.hpp"
#include "ShProgramNode.hpp"

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
      : varnode(stmt->dest.node()), stmt(stmt), node(node), offset(offset),
        disable_mask(disable_mask)
    {}

    Definition(ShVariableNodePtr varnode,
               const ShCtrlGraphNodePtr& node,
               int offset,
               ShBitSet disable_mask)
      : varnode(varnode), stmt(0), node(node), offset(offset), 
        disable_mask(disable_mask)
    {}

    bool isInput() const
    { 
      return !stmt; 
    }

    int size() const
    {
      if(isInput()) return varnode->size();
      else return stmt->dest.size();
    }
    
    /* returns whether the i'th element is disabled */
    int isDisabled(int i) const
    {
      return disable_mask[index(i)];
    }
    
    /* returns the offset of the i'th tuple element */
    int off(int i) const
    {
      return offset + i;
    }

    /* returns unswizzled index for i'th tuple element */
    int index(int i) const
    {
      if(isInput()) return i;
      return stmt->dest.swizzle()[i];
    }

    /* returns Def for i'th tuple element */
    ValueTracking::Def toDef(int i) const
    {
      if(isInput()) return ValueTracking::Def(varnode, i); 
      return ValueTracking::Def(stmt, i);
    }

    friend std::ostream& operator<<(std::ostream& out, const Definition& def)
    {
      out << "{";
      out << " off " << def.offset;
      out << ", node " << def.node.object();
      out << ", dsbl " << def.disable_mask << ", ";
      if(def.isInput()) out << "input " << def.varnode->name();
      else out << "stmt " << *def.stmt;
      out << "}";
      return out;
    }
    
    ShVariableNodePtr varnode;
    ShStatement* stmt;
    ShCtrlGraphNodePtr node;
    int offset;
    
    // Sometimes we want to ignore some of the elements, when
    // constructing gen, because they are overwritten by later
    // statements. In that case we mark the ignored components here.
    // unswizzled disable mask
    ShBitSet disable_mask;
  };

  void addDefinition(const Definition& d)
  {
    // Otherwise, add this definition
    defs.push_back(d);
    defsize += d.size(); 
  }

  typedef std::map<ShCtrlGraphNodePtr, int> SizeMap;
  typedef std::map<ShCtrlGraphNodePtr, ShBitSet> ReachingMap;
  
  std::vector<Definition> defs;
  int defsize; ///< current defsize, offset for next added Definition

  ReachingMap gen, prsv;
  ReachingMap rchin;
};

struct DefFinder {
  DefFinder(ReachingDefs& r, ShCtrlGraphNodePtr entry, const ShProgramNode::VarList& inputs)
    : entry(entry), inputs(inputs), r(r), offset(0)
  {
  }

  void operator()(ShCtrlGraphNodePtr node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;

    // Use this data structure to mark assignments to node elements,
    // so that we can ignore earlier assignments to them in this block.
    std::map<ShVariableNodePtr, ShBitSet> disable_map;

    if(block) {
      // Iterate backwards over the list of statements...
      ShBasicBlock::ShStmtList::iterator I = block->end();
      while (1) {
        if (I == block->begin()) break;
        --I;
        if (!I->dest.null() && I->op != SH_OP_KIL && I->op != SH_OP_OPTBRA /*&& I->dest.node()->kind() == SH_TEMP*/) {
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
          r.addDefinition(ReachingDefs::Definition(&(*I), node, r.defsize,
                                                    disable_map[I->dest.node()]));

          // And update the disable map for the destination node.
          disable_map[I->dest.node()] |= defn_map;
        }
      }
    }

    // Add in defs for the inputs
    if(node != entry) return; 
    for(ShProgramNode::VarList::const_iterator I = inputs.begin(); 
        I != inputs.end(); ++I) {
      if(disable_map.find(*I) == disable_map.end()) {
        disable_map[*I] = ShBitSet((*I)->size());
      }

      // check whether input does not reach end of block
      if(disable_map[*I].full()) continue;

      r.addDefinition(ReachingDefs::Definition(*I, node, r.defsize,
            disable_map[*I]));
    }
  }

  ShCtrlGraphNodePtr entry; // @todo range - this is a touch clumsy
  const ShProgramNode::VarList& inputs;
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

    // Initialize gen
    for (unsigned int i = 0; i < r.defs.size(); i++) {
      ReachingDefs::Definition &d = r.defs[i];
      if (d.node == node) {
        for (int j = 0; j < d.size(); j++) {
// @todo range - I think this needs to be fixed for swizzling?
//               it's been fixed above in isDisabled
         // if (!r.defs[i].disable_mask[j]) {
          if (!d.isDisabled(j)) {
            r.gen[node][d.off(j)] = true;
          }
        }
      }
    }

    if(!block) return;

    // Initialize prsv
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->dest.null() 
          || I->op == SH_OP_KIL
          || I->op == SH_OP_OPTBRA
          /*|| I->dest.node()->kind() != SH_TEMP*/) continue;
      for (unsigned int i = 0; i < r.defs.size(); i++) {
        ReachingDefs::Definition &d = r.defs[i];
        if (d.varnode != I->dest.node()) continue;

        for (int j = 0; j < I->dest.size(); ++j) {
          for (int k = 0; k < d.size(); ++k) {
            if (d.index(k) == I->dest.swizzle()[j]) {
              r.prsv[node][d.off(k)] = false;
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

// Builds ud and du chains per statement and
// also gathers input du and output ud chains into the program
struct UdDuBuilder {
  UdDuBuilder(ReachingDefs& r, ShProgramNodePtr p) 
    : r(r), m_exit(p->ctrlGraph->exit()), p(p),
      intrack(new InputValueTracking()),
      outtrack(new OutputValueTracking())
  {
    p->destroy_info<InputValueTracking>();
    p->destroy_info<OutputValueTracking>();
    p->add_info(intrack);
    p->add_info(outtrack);
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

    // TODO: Handle "non assigning" statements like KIL

    // initialize defs at the start of the block, using the reaching
    // definitions solution.
    for (std::size_t i = 0; i < r.defs.size(); i++) {
      for (int j = 0; j < r.defs[i].size(); j++) {
        if (r.rchin[node][r.defs[i].offset + j]) {
          ValueTracking::Def def(r.defs[i].toDef(j));
          defs[TupleElement(r.defs[i].varnode,
                            r.defs[i].index(j))].insert(def);
        }
      }
    }

    if(block) {
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
            ShVariableNodePtr srcNode = I->src[j].node();
            for (int i = 0; i < I->src[j].size(); i++) {
              const DefSet& ds = defs[TupleElement(srcNode, I->src[j].swizzle()[i])];

              vt->defs[j][i] = ds;
              ValueTracking::Use srcUse(&(*I), j, i);
              for (DefSet::const_iterator J = ds.begin(); J != ds.end(); J++) {
                switch(J->kind) {
                  case ValueTracking::Def::INPUT:
                    {
                      ValueTracking::TupleDefUseChain& inputDu = 
                        intrack->inputUses[srcNode];
                      if(inputDu.empty()) {
                        inputDu.resize(srcNode->size());
                      }
                      inputDu[J->index].insert(srcUse);
                      break;
                    }
                  case ValueTracking::Def::STMT:
                    {
                      ValueTracking* ut = J->stmt->get_info<ValueTracking>();
                      if (!ut) {
                        ut = new ValueTracking(J->stmt);
                        J->stmt->add_info(ut);
                      }
                      ut->uses[J->index].insert(srcUse);
                      break;
                    }
                }
              }
            //} 
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

    // now find any output defs and add appropriate du chain to stmts 
    // @todo range - note that this still doesn't show inputs that are
    // immediately passed through to outputs, but that should be easy 
    // to store somewhere if needed
    if(node == m_exit) {
      for(DefMap::iterator D = defs.begin(); D != defs.end(); ++D) {
        ShVariableNodePtr node = D->first.node;
        ShBindingType kind = node->kind();
        int index = D->first.index;

        if(kind == SH_INOUT || kind == SH_OUTPUT) {
          DefSet& ds = D->second;

          ValueTracking::TupleUseDefChain& outDef = outtrack->outputDefs[node];
          if(outDef.empty()) {
            outDef.resize(node->size());
          }
          outDef[index] = ds;

          for(DefSet::iterator S = ds.begin(); S != ds.end(); ++S) {
            if(S->kind != ValueTracking::Def::STMT) continue; 
            ValueTracking* vt = S->stmt->get_info<ValueTracking>();
            SH_DEBUG_ASSERT(vt); // must have added it above somewhere
            ValueTracking::Use use(node, S->stmt->dest.swizzle()[S->index]);
            vt->uses[S->index].insert(use);
          }
        }
      }
    }
  }

  ReachingDefs& r;
  ShCtrlGraphNodePtr m_exit;
  ShProgramNodePtr p;
  InputValueTracking* intrack;
  OutputValueTracking* outtrack;
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
      SH_DEBUG_PRINT("Valuetracking for " << *I);
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        SH_DEBUG_PRINT("  src ud" << i << "\n" << vt->defs[i]);
      }
      SH_DEBUG_PRINT("  dest du" << vt->uses);
    }
  }

  void operator()(ShProgramNodePtr p) {
    InputValueTracking* ivt = p->get_info<InputValueTracking>();
    SH_DEBUG_ASSERT(ivt);
    SH_DEBUG_PRINT("Input Valuetracking:\n" << *ivt);

    OutputValueTracking* ovt = p->get_info<OutputValueTracking>();
    SH_DEBUG_ASSERT(ovt);
    SH_DEBUG_PRINT("Output Valuetracking:\n" << *ovt);
  }
};

}

namespace SH {

ValueTracking::ValueTracking(ShStatement* stmt)
  : uses(stmt->dest.node() ? stmt->dest.size() : 0),
    defs(stmt->src.size())
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

std::ostream& operator<<(std::ostream& out, const ValueTracking::Use& use) {
  if(use.kind == ValueTracking::Use::STMT) {
    out << "(" << *use.stmt << ").src" << use.source << "[" << use.index << "]";
  } else {
    out << "(OUTPUT " << use.node->name() << ")[" << use.index << "]"; 
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const ValueTracking::TupleUseDefChain& tud)
{
  int e = 0;
  for (ValueTracking::TupleUseDefChain::const_iterator E = tud.begin();
       E != tud.end(); ++E, ++e) {
    out << "[" << e << "] <- {";
    for (ValueTracking::UseDefChain::const_iterator J = E->begin(); J != E->end(); ++J) {
      if(J != E->begin()) out << ", ";
      out << *J;
    }
    out << "}\n";
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const ValueTracking::Def& def) {
  if(def.kind == ValueTracking::Def::STMT) {
    out << "(" << *def.stmt << ").dst" << "[" << def.index << "]";
  } else {
    out << "(INPUT " << def.node->name() << ")[" << def.index << "]"; 
  }
}

std::ostream& operator<<(std::ostream& out, const ValueTracking::TupleDefUseChain& tdu)
{
  int e = 0;
  for (ValueTracking::TupleDefUseChain::const_iterator E = tdu.begin();
       E != tdu.end(); ++E, ++e) {
    out << "[" << e << "] -> {";
    for (ValueTracking::DefUseChain::const_iterator J = E->begin(); J != E->end(); ++J) {
      if(J != E->begin()) out << ", ";
      out << *J;
    }
    out << "}\n";
  }
  return out;
}

ShInfo* InputValueTracking::clone() const
{
  return new InputValueTracking(*this);
}


std::ostream& operator<<(std::ostream& out, const InputValueTracking& ivt)
{
  InputValueTracking::InputTupleDefUseChain::const_iterator I;
  for(I = ivt.inputUses.begin(); I != ivt.inputUses.end(); ++I) {
    ShVariableNodePtr node = I->first;
    const ValueTracking::TupleDefUseChain& tdu = I->second;
    out << node->name();
    out << tdu; 
  }
  return out;
}


ShInfo* OutputValueTracking::clone() const
{
  return new OutputValueTracking(*this);
}

std::ostream& operator<<(std::ostream& out, const OutputValueTracking& ovt)
{
  OutputValueTracking::OutputTupleUseDefChain::const_iterator O;
  for(O = ovt.outputDefs.begin(); O != ovt.outputDefs.end(); ++O) {
    ShVariableNodePtr node = O->first;
    const ValueTracking::TupleUseDefChain& tud = O->second;
    out << node->name() << tud; 
  }
  return out;
}

void add_value_tracking(ShProgram& p)
{
  ReachingDefs r;

  ShCtrlGraphPtr graph = p.node()->ctrlGraph;
  
  DefFinder finder(r, graph->entry(), p.node()->inputs);
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
    SH_DEBUG_PRINT("  " << i << ": " << r.defs[i]);
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
  
  UdDuBuilder builder(r, p.node());
  graph->dfs(builder);

#ifdef SH_DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Uddu Dump");
  UdDuDumper dumper;
  graph->dfs(dumper);
  dumper(p.node());
#endif
}



}
