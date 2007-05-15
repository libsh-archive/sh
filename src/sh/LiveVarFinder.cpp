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
#include "Context.hpp"
#include "ProgramNode.hpp"

/* This is very much based on the existing ValueTracking stuff */

namespace {
using namespace SH;


/* initializes use/def sets 
 * def = set of variable tuple elements defined before any use in B
 * use = set of variable tuple elements used before def in B */
struct InitUseDef {
  /* Adds variable (or elements in variable) that are not in check into the set s */ 
  void addCheck(const Variable& var, const LiveVars::ElementSet& check, LiveVars::ElementSet& s) {
    if(var.null()) return;

    BitSet result(var.node()->size());
    for(int i = 0; i < var.size(); ++i) result[var.swizzle()[i]] = true;

    /* cancel out any elements that are true in check */
    LiveVars::ElementSet::const_iterator V = check.find(var.node());
    if(V != check.end()) {
      result -= V->second; 
    } 

    if(!result.empty()) {
      if(s.find(var.node()) != s.end()) {
        s[var.node()] |= result;
      } else {
        s[var.node()] = result;
      }
    }
  }

  void operator()(CtrlGraphNode* node)
  {
    LiveVars* alive = new LiveVars(); 
    node->add_info(alive);

    if (!node) return;
    BasicBlockPtr block = node->block;

    if(block) {
      for(BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
        /* add srcs to use, if not yet defined */
        for(size_t i = 0; i < I->src.size(); ++i) {
          addCheck(I->src[i], alive->def, alive->use);
        }

        /* add dest to def, if not yet used */
        addCheck(I->dest, alive->use, alive->def);
      }
    }
  }
};

/* initializes out on the exit nodes for outputs */
void initOut(Program& p) {
  CtrlGraphNode* cfg_exit = p.node()->ctrlGraph->exit();
  LiveVars* alive = cfg_exit->get_info<LiveVars>();

  for(ProgramNode::VarList::const_iterator O = p.begin_outputs(); O != p.end_outputs(); ++O) {
    alive->out[*O] =  ~BitSet((*O)->size());
  }
}

/* iterates */ 
struct LiveIter {
  bool& changed;

  LiveIter(bool& changed): changed(changed) {}

  typedef LiveVars::ElementSet ElmSet; 

  /* Takes a union b and puts result in a, setting changed = true if a changes*/ 
  void unionIntoA(ElmSet& a, const ElmSet &b) {
    for(ElmSet::const_iterator B = b.begin(); B != b.end(); ++B) {
      if(a.find(B->first) != a.end()) {
        size_t oldcount = a[B->first].count(); 
        a[B->first] |= B->second;
        changed |= (a[B->first].count() != oldcount);
      } else {
        changed = true;
        a[B->first] = B->second;
      }
    }
  }

  /* Computes a union (b - c) and puts result in a, setting changed = true if a changes */
  void unionSub(ElmSet& a, const ElmSet &b, const ElmSet &c) {
    for(ElmSet::const_iterator B = b.begin(); B != b.end(); ++B) {
      BitSet bsc; 
      ElmSet::const_iterator C = c.find(B->first);
      if(C != c.end()) {
        bsc = B->second - C->second; 
      } else {
        bsc = B->second;
      }
      if(bsc.empty()) continue;

      if(a.find(B->first) != a.end()) {
        size_t oldcount = a[B->first].count(); 
        a[B->first] |= bsc; 
        changed |= (a[B->first].count() != oldcount);
      } else {
        changed = true;
        a[B->first] = bsc; 
      }
    }
  }

  void operator()(CtrlGraphNode* node)
  {
    LiveVars* alive = node->get_info<LiveVars>(); 
    for(CtrlGraphNode::SuccessorConstIt S = node->successors_begin(); S != node->successors_end(); ++S) {
      LiveVars* salive = S->node->get_info<LiveVars>();
      unionIntoA(alive->out, salive->in); 
    }
    if(node->follower()) {
      LiveVars* falive = node->follower()->get_info<LiveVars>();
      unionIntoA(alive->out, falive->in); 
    }
    if(alive->in.empty()) {
      alive->in = alive->use;
    }
    unionSub(alive->in, alive->out, alive->def);
  }
};

std::ostream& print(std::ostream& out, const SH::LiveVars::ElementSet& es) {
  for(LiveVars::ElementSet::const_iterator V = es.begin(); V != es.end(); ++V) {
    out << V->first->name() << " " << V->second << " addr=" << V->first.object() << std::endl;  
  }
  return out;
}


}

namespace SH {

Info* LiveVars::clone() const  
{
  return new LiveVars(*this); 
}


std::ostream& operator<<(std::ostream& out, const LiveVars& lvs)
{
  out << "in:" << std::endl;
  print(out, lvs.in) << std::endl;
  out << "use:" << std::endl; 
  print(out, lvs.use) << std::endl;
  out << "out:" << std::endl; 
  print(out, lvs.out) << std::endl;
  out << "def:" << std::endl; 
  print(out, lvs.def) << std::endl;
  return out;
}

void find_live_vars(Program& p)
{
  CtrlGraphPtr graph = p.node()->ctrlGraph;
  InitUseDef init;
  graph->dfs(init);

  bool changed;
  LiveIter li(changed);
  do {
    changed = false;
    graph->dfs(li);
  } while(changed);
}



}
