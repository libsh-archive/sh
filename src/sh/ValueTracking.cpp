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
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Optimizations.hpp"
#include "BitSet.hpp"
#include "CtrlGraph.hpp"
#include "Debug.hpp"
#include "Evaluate.hpp"
#include "Context.hpp"
#include "Syntax.hpp"
#include "ProgramNode.hpp"

// Uncomment to enable use-def chain debugging 
// #define DEBUG_VALUETRACK

#ifdef DEBUG_OPTIMIZER
#ifndef DEBUG_VALUETRACK
#define DEBUG_VALUETRACK
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
    Definition(Statement* stmt,
               const CtrlGraphNodePtr& node,
               int offset,
               BitSet disable_mask)
      : varnode(stmt->dest.node()), stmt(stmt), node(node), offset(offset),
        disable_mask(disable_mask)
    {}

    Definition(const VariableNodePtr& varnode,
               const CtrlGraphNodePtr& node,
               int offset,
               BitSet disable_mask)
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
    
    VariableNodePtr varnode;
    Statement* stmt;
    CtrlGraphNodePtr node;
    int offset;
    
    // Sometimes we want to ignore some of the elements, when
    // constructing gen, because they are overwritten by later
    // statements. In that case we mark the ignored components here.
    // unswizzled disable mask
    BitSet disable_mask;
  };

  void addDefinition(const Definition& d)
  {
    // Otherwise, add this definition
    defs.push_back(d);
    defsize += d.size(); 
  }

  typedef std::map<CtrlGraphNodePtr, int> SizeMap;
  typedef std::map<CtrlGraphNodePtr, BitSet> ReachingMap;
  
  std::vector<Definition> defs;
  int defsize; ///< current defsize, offset for next added Definition

  ReachingMap gen, prsv;
  ReachingMap rchin;
};

struct DefFinder {
  DefFinder(ReachingDefs& r, const CtrlGraphNodePtr& entry, const ProgramNode::VarList& inputs)
    : entry(entry), inputs(inputs), r(r), offset(0)
  {
  }

  // assignment operator could not be generated: declaration only
  DefFinder& operator=(DefFinder const&);

  void operator()(const CtrlGraphNodePtr& node)
  {
    if (!node) return;
    BasicBlockPtr block = node->block;

    // Use this data structure to mark assignments to node elements,
    // so that we can ignore earlier assignments to them in this block.
    std::map<VariableNodePtr, BitSet> disable_map;

    if(block) {
      // Iterate backwards over the list of statements...
      BasicBlock::StmtList::iterator I = block->end();
      while (1) {
        if (I == block->begin()) break;
        --I;
        if (!I->dest.null() && I->op != OP_KIL && I->op != OP_OPTBRA /*&& I->dest.node()->kind() == SH_TEMP*/) {
          // Construct a disable map if this node has not yet been
          // assigned to in a later statement in this block.
          if (disable_map.find(I->dest.node()) == disable_map.end()) {
            disable_map[I->dest.node()] = BitSet(I->dest.node()->size());
          }

          // Construct a bitset containing all elements which this
          // definition affects.
          BitSet defn_map(I->dest.node()->size());
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
    for(ProgramNode::VarList::const_iterator I = inputs.begin(); 
        I != inputs.end(); ++I) {
      if(disable_map.find(*I) == disable_map.end()) {
        disable_map[*I] = BitSet((*I)->size());
      }

      // check whether input does not reach end of block
      if(disable_map[*I].full()) continue;

      r.addDefinition(ReachingDefs::Definition(*I, node, r.defsize,
            disable_map[*I]));
    }
  }

  CtrlGraphNodePtr entry; // @todo range - this is a touch clumsy
  const ProgramNode::VarList& inputs;
  ReachingDefs& r;
  int offset;
};

struct InitRch {
  InitRch(ReachingDefs& r)
    : r(r)
  {
  }

  // assignment operator could not be generated: declaration only
  InitRch& operator=(InitRch const&);

  void operator()(const CtrlGraphNodePtr& node)
  {
    if (!node) return;

    r.rchin[node] = BitSet(r.defsize);
    r.gen[node] = BitSet(r.defsize);
    r.prsv[node] = ~BitSet(r.defsize);
      
    BasicBlockPtr block = node->block;

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
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if (I->dest.null() 
          || I->op == OP_KIL
          || I->op == OP_OPTBRA
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

  // assignment operator could not be generated: declaration only
  IterateRch& operator=(IterateRch const&);

  void operator()(const CtrlGraphNodePtr& node)
  {
    if (!node) return;
    SH_DEBUG_ASSERT(r.rchin.find(node) != r.rchin.end());
    BitSet newRchIn(r.defsize);
    
    for (CtrlGraphNode::PredList::iterator I = node->predecessors.begin();
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
  UdDuBuilder(ReachingDefs& r, const ProgramNodePtr& p) 
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
    TupleElement(const VariableNodePtr& node, int index)
      : node(node), index(index)
    {
    }

    bool operator<(const TupleElement& other) const
    {
      if (node < other.node) return true;
      if (node == other.node) return index < other.index;
      return false;
    }
    
    VariableNodePtr node;
    int index;
  };

  // assignment operator could not be generated: declaration only
  UdDuBuilder& operator=(UdDuBuilder const&);

  void operator()(const CtrlGraphNodePtr& node) {
    typedef std::set<ValueTracking::Def> DefSet;
    typedef std::map<TupleElement, DefSet> DefMap;

    // defs contains all of the possible contributors to the key's
    // definition at the current point.
    DefMap defs;
    
    if (!node) return;
    BasicBlockPtr block = node->block;

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
      for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
        // Compute the ud chains for the statement's source variables,
        // and contribute to the du chains of the source variables' definitions.
        for (int j = 0; j < opInfo[I->op].arity; j++) {
          //if (I->src[j].node()->kind() == SH_TEMP) {
            ValueTracking* vt = I->get_info<ValueTracking>();
            if (!vt) {
              vt = new ValueTracking(&(*I));
              I->add_info(vt);
            }
            VariableNodePtr srcNode = I->src[j].node();
            for (int i = 0; i < I->src[j].size(); i++) {
              const DefSet& ds = defs[TupleElement(srcNode, I->src[j].swizzle()[i])];

              vt->defs[j][i] = ds;
              ValueTracking::Use srcUse(&(*I), j, i);
              for (DefSet::const_iterator J = ds.begin(); J != ds.end(); J++) {
                switch(J->kind) {
                  case ValueTracking::Def::SH_INPUT:
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
        VariableNodePtr node = D->first.node;
        BindingType kind = node->kind();
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
            if (!vt) {
              vt = new ValueTracking(S->stmt);
              S->stmt->add_info(vt);
            }
            SH_DEBUG_ASSERT(vt); // must have added it above somewhere
            ValueTracking::Use use(node, S->stmt->dest.swizzle()[S->index]);
            vt->uses[S->index].insert(use);
          }
        }
      }
    }
  }

  ReachingDefs& r;
  CtrlGraphNodePtr m_exit;
  ProgramNodePtr p;
  InputValueTracking* intrack;
  OutputValueTracking* outtrack;
};

struct UdDuClearer {
  void operator()(const CtrlGraphNodePtr& node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      I->destroy_info<ValueTracking>();
    }
  }
};

struct UdDuDumper {
  void operator()(const CtrlGraphNodePtr& node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;

    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
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

  void operator()(const ProgramNodePtr& p) {
#ifdef DEBUG
    InputValueTracking* ivt = p->get_info<InputValueTracking>();
    SH_DEBUG_ASSERT(ivt);
    SH_DEBUG_PRINT("Input Valuetracking:\n" << *ivt);

    OutputValueTracking* ovt = p->get_info<OutputValueTracking>();
    SH_DEBUG_ASSERT(ovt);
    SH_DEBUG_PRINT("Output Valuetracking:\n" << *ovt);
#endif
  }
};

}

namespace SH {

ValueTracking::ValueTracking(Statement* stmt)
  : uses(stmt->dest.node() ? stmt->dest.size() : 0),
    defs(stmt->src.size())
{
#ifdef DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Adding value tracking to " << *stmt);
#endif
  for (int i = 0; i < opInfo[stmt->op].arity; i++) {
    for (int j = 0; j < (stmt->src[i].node() ? stmt->src[i].size() : 0); j++) {
      defs[i].push_back(std::set<Def>());
    }
  }
}

Info* ValueTracking::clone() const
{
  return new ValueTracking(*this);
}

std::ostream& operator<<(std::ostream& out, const ValueTracking::Use& use) {
  if(use.kind == ValueTracking::Use::STMT) {
    out << "(" << *use.stmt << ").src" << use.source << "[" << use.index << "]";
  } else {
    out << "(SH_OUTPUT " << use.node->name() << ")[" << use.index << "]"; 
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
    out << "(SH_INPUT " << def.node->name() << ")[" << def.index << "]"; 
  }
  return out;
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

Info* InputValueTracking::clone() const
{
  return new InputValueTracking(*this);
}


std::ostream& operator<<(std::ostream& out, const InputValueTracking& ivt)
{
  InputValueTracking::InputTupleDefUseChain::const_iterator I;
  for(I = ivt.inputUses.begin(); I != ivt.inputUses.end(); ++I) {
    VariableNodePtr node = I->first;
    const ValueTracking::TupleDefUseChain& tdu = I->second;
    out << node->name();
    out << tdu; 
  }
  return out;
}


Info* OutputValueTracking::clone() const
{
  return new OutputValueTracking(*this);
}

std::ostream& operator<<(std::ostream& out, const OutputValueTracking& ovt)
{
  OutputValueTracking::OutputTupleUseDefChain::const_iterator O;
  for(O = ovt.outputDefs.begin(); O != ovt.outputDefs.end(); ++O) {
    VariableNodePtr node = O->first;
    const ValueTracking::TupleUseDefChain& tud = O->second;
    out << node->name() << tud; 
  }
  return out;
}

void add_value_tracking(Program& p)
{
  ReachingDefs r;

  CtrlGraphPtr graph = p.node()->ctrlGraph;
  
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

#ifdef DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Dumping Reaching Defs");
  SH_DEBUG_PRINT("defsize = " << r.defsize);
  SH_DEBUG_PRINT("defs.size() = " << r.defs.size());
  for(unsigned int i = 0; i < r.defs.size(); ++i) {
    SH_DEBUG_PRINT("  " << i << ": " << r.defs[i]);
  }
  std::cerr << std::endl;

  for (ReachingDefs::ReachingMap::const_iterator I = r.rchin.begin(); I != r.rchin.end(); ++I) {
    CtrlGraphNodePtr node = I->first;
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

#ifdef DEBUG_VALUETRACK
  SH_DEBUG_PRINT("Uddu Dump");
  UdDuDumper dumper;
  graph->dfs(dumper);
  dumper(p.node());
#endif
}



}
