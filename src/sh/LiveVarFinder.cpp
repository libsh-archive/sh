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
#include "Transformer.hpp"

/* This is very much based on the existing ValueTracking stuff */
using namespace SH;
using namespace std;

namespace {


/* initializes use/def sets 
 * def = set of variable tuple elements defined before any use in B
 * use = set of variable tuple elements used before def in B */
struct InitUseDef {
  ReachingDefs* rchDef;
  InitUseDef(ReachingDefs* rchDef): rchDef(rchDef) {}

  /* Adds variable (or elements in variable) that are not in check into the set s */ 
  static void addCheck(const Variable& var, const LiveVars::ElementSet& check, LiveVars::ElementSet& s) {
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
    node->destroy_info<LiveVars>();
    node->add_info(alive);

    /* initialize rchout */
    SH_DEBUG_ASSERT(rchDef->out.find(node) != rchDef->out.end());
    const BitSet& nodeOut = rchDef->out[node];
    for(size_t i = 0; i < rchDef->defs.size(); ++i) {
      ReachingDefs::Definition &d = rchDef->defs[i];
      if(alive->rchout.find(d.varnode) == alive->rchout.end()) alive->rchout[d.varnode] = BitSet(d.varnode->size());
      BitSet& bits = alive->rchout[d.varnode];
      for(int j = 0; j < d.size(); ++j) {
        if(nodeOut[d.off(j)]) {
          bits[j] = true;
        }
      }
      if(bits.empty()) alive->rchout.erase(d.varnode);
    }

    BasicBlockPtr block = node->block;
    if(!block) return;

    for(BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      /* add srcs to use, if not yet defined */
      for(size_t i = 0; i < I->src.size(); ++i) {
        addCheck(I->src[i], alive->def, alive->use);
      }

      /* add dest to def, if not yet used */
      addCheck(I->dest, alive->use, alive->def);
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

  /* Takes a = a union (b isct c) and puts result in a, setting changed = true if a changes*/ 
  void unionIsct(ElmSet& a, const ElmSet &b, const ElmSet& c) {
    for(ElmSet::const_iterator B = b.begin(); B != b.end(); ++B) {
      if(c.find(B->first) == c.end()) continue;
      BitSet bandc = B->second & c.find(B->first)->second;
      if(a.find(B->first) != a.end()) {
        size_t oldcount = a[B->first].count(); 
        a[B->first] |= bandc; 
        changed |= (a[B->first].count() != oldcount);
      } else {
        changed = true;
        a[B->first] = bandc; 
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
      unionIsct(alive->out, salive->in, alive->rchout); 
    }
    if(node->follower()) {
      LiveVars* falive = node->follower()->get_info<LiveVars>();
      unionIsct(alive->out, falive->in, alive->rchout); 
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

struct StatementLiveFinder {
  void operator()(CtrlGraphNode* node)
  {
    LiveVars* alive = node->get_info<LiveVars>(); 
    if(!node->block) return;
    BasicBlockPtr block = node->block;
    LiveVars* last_alive = 0;
    bool changed; /* doesn't actually matter */
    LiveIter li(changed);
    for(BasicBlock::StmtList::reverse_iterator I = block->rbegin(); I != block->rend(); ++I) {
      Statement& stmt = *I;
      LiveVars* stmt_alive = new LiveVars(); 
      stmt.destroy_info<LiveVars>();
      stmt.add_info(stmt_alive);

      if(I == block->rbegin()) { /* initialize from alive->out */
        stmt_alive->out = alive->out;
      } else { /* propagate from previous */
        stmt_alive->out = last_alive->in;
      }
      /* set up ->use, ->def */
      for(size_t i = 0; i < stmt.src.size(); ++i) {
        InitUseDef::addCheck(stmt.src[i], stmt_alive->def, stmt_alive->use);
      }
      InitUseDef::addCheck(stmt.dest, stmt_alive->use, stmt_alive->def);

      stmt_alive->in = stmt_alive->use;
      li.unionSub(stmt_alive->in, stmt_alive->out, stmt_alive->def);
      last_alive = stmt_alive;
    }
  }
};

}

namespace SH {

LiveVarCsvDataBase::LiveVarCsvDataBase() {
  total_live = scalar_total_live = max_live = max_scalar_live = stmt_count = 0;
}

void LiveVarCsvDataBase::finish() {
  Program p(m_program);
  insert_branch_instructions(p);
  find_statement_live_vars(p);
  remove_branch_instructions(p);
}

std::string LiveVarCsvDataBase::header() {
  return ",live_vars,live_scalar_vars";
}

std::string LiveVarCsvDataBase::operator()(const Statement& stmt) {
  const LiveVars* alive = stmt.get_info<LiveVars>();
  int live_vars = 0;
  int scalar_live_vars = 0;


  LiveVars::const_iterator I;
  for(I = alive->out.begin(); I != alive->out.end(); ++I) {
    switch(I->first->kind()) {
      case SH_CONST:
      case SH_TEXTURE:
      case SH_STREAM:
      case SH_PALETTE:
      case BINDINGTYPE_END:
        continue;
      case SH_TEMP:
        if(I->first->uniform()) continue;
      case SH_INPUT:
      case SH_OUTPUT:
      case SH_INOUT:
        break;
    }
    if(!I->second.empty()) {
      live_vars += 1;
      scalar_live_vars += I->second.count();
    }
  }

  ostringstream sout;
  sout << "," << live_vars << "," << scalar_live_vars; 
  scalar_total_live += scalar_live_vars;
  total_live += live_vars;
  max_live = std::max(max_live, live_vars);
  max_scalar_live = std::max(max_scalar_live, scalar_live_vars);
  stmt_count++;
  return sout.str();
}

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
  out << "rchout:" << std::endl; 
  print(out, lvs.rchout) << std::endl;
  return out;
}

void find_live_vars(Program& p)
{
  add_value_tracking(p);
  ReachingDefs* rchdef = p.node()->get_info<ReachingDefs>(); 

  CtrlGraphPtr graph = p.node()->ctrlGraph;

  InitUseDef init(rchdef);
  graph->dfs(init);

  bool changed;
  LiveIter li(changed);
  do {
    changed = false;
    graph->dfs(li);
  } while(changed);
}

void find_statement_live_vars(Program& p)
{
  CtrlGraphPtr graph = p.node()->ctrlGraph;
  find_live_vars(p);
  StatementLiveFinder slf;
  graph->dfs(slf);
}

/* Runs find_statement_live_Vars and tags statements with number of live variables */ 
SH_DLLEXPORT 
StmtCsvDataPtr getLiveVarCsvData(ProgramNodePtr p) {
  LiveVarCsvData* lvcd = new LiveVarCsvData();
  lvcd->transform(p);
  return lvcd;
}

}
