// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////

#include <queue>
#include "ShDebug.hpp"
#include "ShTransformer.hpp"
#include "ShOptimizations.hpp"
#include "ShAaSymPlacer.hpp"

#define SH_DBG_AASYMPLACER

#ifdef SH_DBG_AASYMPLACER
#define SH_DEBUG_PRINT_ASP(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASP(x) {}
#endif

namespace {
using namespace SH;

typedef std::queue<ValueTracking::Def> WorkList;

/* Looks at sources and initializes symbols for dest.
 * If dest is non-empty, adds the definition to the worklist 
 * @param stmtSyms The stmt error symbol assignment to update
 * @param worklist The worklist to add Definitions to
 * @param addAlways If true, then always adds a dest if it is not empty.
 *                  else only adds if it has changed */
void updateDest(ShAaStmtSyms *stmtSyms, WorkList &worklist, bool addAlways=false)
{
  ShStatement* stmt = stmtSyms->stmt;
  ShAaSyms &dest = stmtSyms->dest;
  ShAaStmtSyms::SymsVec &src = stmtSyms->src;

  std::vector<bool> destChanged(dest.size(), false);

  if(opInfo[stmt->op].affine_keep) {
    switch(opInfo[stmt->op].result_source) {
      case ShOperationInfo::IGNORE: 
        break;

      case ShOperationInfo::LINEAR:
      case ShOperationInfo::EXTERNAL: // assume component-wise error sym propagation 
        {
          SH_DEBUG_PRINT_ASP("    LINEAR/EXTERNAL"); 
          for(int i = 0; i < dest.size(); ++i) {
            int oldsize = dest[i].size();
            for(int j = 0; j < opInfo[stmt->op].arity; ++j) {
              bool scalarSrc = stmt->src[j].size() == 1;
              dest.merge(i, src[j][scalarSrc ? 0 : i]);
            }
            destChanged[i] = (oldsize != dest[i].size());
          }
          break;
        }
      case ShOperationInfo::ALL: // all src error syms may go to dest
        {
          ShAaIndexSet all;
          for(int i = 0; i < opInfo[stmt->op].arity; ++i) {
            for(int j = 0; j < src[i].size(); ++j) {
              all |= (src[i][j]);
            }
          }
          SH_DEBUG_PRINT_ASP("    ALL all=" << all);
          for(int i = 0; i < dest.size(); ++i) {
            int oldsize = dest[i].size();
            dest.merge(i, all);
            destChanged[i] = (oldsize != dest[i].size());
          }
          break;
        }
      default: 
        SH_DEBUG_PRINT_ASP("The king has left the building.  You should not be here either.");
    }
  }

  // Only propagate changes if the dest is affine.  Otherwise, it is interval
  // and the error syms disappear at this point
  // @todo range - think this through and make this make more sense...
  if(!shIsAffine(stmt->dest.valueType())) return;

  for(int i = 0; i < dest.size(); ++i) {
    if(destChanged[i] || (addAlways && !dest[i].empty())) {
      worklist.push(ValueTracking::Def(stmt, i));
    }
  }
  return; 
}

/* Places symbols for inputs, intervals used as sources, and statement
 * destinations when a statement is non-affine.
 */
struct SymInitBase: public ShTransformerParent {
  SymInitBase()
    : psyms(new ShAaProgramSyms()), cur_index(0) 
  {}

  /* Handle predefined error symbols.  Start new error symbols from one more
   * than the maximum predefined error symbol 
   * Assigns the worklist to fill with w
   *
   * This must be called exactly once before each transform call */
  void init(const ShAaVarSymsMap* inputs, WorkList *w) 
  {
    worklist = w;

    psyms->inputs = *inputs;
    int max_index = -1;
    for(ShAaVarSymsMap::const_iterator I = inputs->begin(); I != inputs->end(); ++I) {
      SH_DEBUG_PRINT_ASP("Allocating error sym " << I->second << " for input " << I->first->name() << " (predefined)"); 
      for(ShAaSyms::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {
        for(ShAaIndexSet::const_iterator K = J->begin(); K != J->end(); ++K) {
          max_index = std::max(max_index, *K);    
        }
      }
    }
    cur_index = max_index + 1;
    SH_DEBUG_PRINT_ASP("After init cur_index=" << cur_index);
  }

  // Initializes input error symbols
  void handleVarList(ShProgramNode::VarList& varList, ShBindingType type)
  {
    if(type != SH_INPUT) return;
    ShProgramNode::VarList::const_iterator V = varList.begin();

    for(; V != varList.end(); ++V) {
      ShVariableNodePtr node = *V;
      if(!shIsAffine(node->valueType())) continue; 
      else if(psyms->inputs.find(node) != psyms->inputs.end()) continue;

      psyms->inputs[node] = ShAaSyms(node->size(), cur_index);
      m_changed = true;

      SH_DEBUG_PRINT_ASP("Allocating error sym " << psyms->inputs[node] << " for input " << node->name());
    }
  }


  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false;

    ShAaStmtSyms *stmtSyms = new ShAaStmtSyms(&stmt); 
    stmt.destroy_info<ShAaStmtSyms>();
    stmt.add_info(stmtSyms);

    SH_DEBUG_PRINT_ASP("handleStmt stmt=" << stmt << " cur_index=" << cur_index);

    // @todo range - don't bother placing symbols if there are no affine sources
    // or dests in the statement.

    /* Handle inputs and IA sources */ 
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {

      if(psyms->inputs.find(stmt.src[i].node()) != psyms->inputs.end()) {
        // affine inputs already have assigned error symbols
        // @todo range (being conservative here.  Should check if input value
        // actually reaches this point of the program) 
        stmtSyms->src[i].merge(stmt.src[i].swizzle(), psyms->inputs[stmt.src[i].node()]); 
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  fetched input sym " << stmtSyms->src[i] << " for src[" << i << "]"); 

      } else if(shIsInterval(stmt.src[i].valueType())) {
        // Assign new error symbols for IA source arguments
        stmtSyms->src[i] = ShAaSyms(stmt.src[i].swizzle(), cur_index);
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->src[i] << " for src[" << i << "]"); 
      }
    }

    // check if op adds a symbol to the output, if so, add it 
    if(opInfo[stmt.op].affine_add) { 
      stmtSyms->dest = stmtSyms->newdest = ShAaSyms(stmt.dest.size(), cur_index);
      m_changed = true;
      SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->dest << " for dest"); 
    }
    updateDest(stmtSyms, *worklist, true);
    return false;
  }


  void finish()
  {
    m_program->destroy_info<ShAaProgramSyms>();
    m_program->add_info(psyms);
  }

  ShAaProgramSyms* psyms;
  WorkList* worklist;
  int cur_index;
};
typedef ShDefaultTransformer<SymInitBase> SymInit;

struct SymGatherBase: public ShTransformerParent {
  /* Set inputs syms, initialize vars for gathering */
  void init(ShAaProgramSyms* ps)
  {
    psyms = ps; 
    psyms->vars = psyms->inputs;
    psyms->outputs.clear();
  }

  /* Gather syms for vars */
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    ShStatement& stmt = *I;
    const ShAaStmtSyms* stmtSyms = stmt.get_info<ShAaStmtSyms>();
    if(stmtSyms && !stmtSyms->dest.empty()) {
      if(psyms->vars.find(stmt.dest.node()) == psyms->vars.end()) {
        psyms->vars[stmt.dest.node()] = ShAaSyms(stmt.dest.node()->size());
      }
      psyms->vars[stmt.dest.node()].mask_merge(stmt.dest.swizzle(), stmtSyms->dest);
      SH_DEBUG_PRINT_ASP("  gathering sym " << psyms->vars[stmt.dest.node()]
          << " for stmt=" << stmt);
    }
    return false;
  }

  /* Set syms for outputs */ 
  void finish()
  {
    ShProgramNode::VarList::const_iterator V = m_program->outputs.begin();
    for(; V != m_program->outputs.end(); ++V) {
      ShVariableNodePtr node = *V;
      if(!shIsAffine(node->valueType())) continue; 

      if(psyms->vars.find(node) == psyms->vars.end()) { // make empty syms
        psyms->outputs[node] = ShAaSyms(node->size()); 
      } else {
        // @todo range - should not really do this.  Remove symbols that are not
        // active at the output 
        psyms->outputs[node] = psyms->vars[node];
      }
      SH_DEBUG_PRINT_ASP("  output sym " << node->name() <<  "=" << psyms->outputs[node]); 
      m_changed = true;
    }
  }

  ShAaProgramSyms* psyms;
};
typedef ShDefaultTransformer<SymGatherBase> SymGather;

struct SymRemoverBase: public ShTransformerParent {
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    I->destroy_info<ShAaStmtSyms>();
    return false;
  }

  void finish()
  {
    m_program->destroy_info<ShAaProgramSyms>();
  }
};
typedef ShDefaultTransformer<SymRemoverBase> SymRemover;

}

namespace SH {

ShAaStmtSyms::ShAaStmtSyms(ShStatement* stmt)
  : stmt(stmt), 
    newdest(stmt->dest.size()),
    dest(stmt->dest.size()),
    src(opInfo[stmt->op].arity)
{
  for(int i = 0; i < opInfo[stmt->op].arity; ++i) {
    src[i].resize(stmt->src[i].size());
  }
}

ShInfo* ShAaStmtSyms::clone() const {
  return new ShAaStmtSyms(*this);
}

std::ostream& operator<<(std::ostream& out, const ShAaStmtSyms& stmtSyms)
{
  out << "AaStmtSyms { " 
      << " stmt=" << *(stmtSyms.stmt); 
  if(!stmtSyms.newdest.empty()) {
    out << " new=" << stmtSyms.newdest;
  }
  out << " dest=("; 
  for(int i = 0; i < stmtSyms.dest.size(); ++i) {
    out << stmtSyms.dest[i];
  }
  out << " )";
  out << " src=[";
  for(int i = 0; i < opInfo[stmtSyms.stmt->op].arity; ++i) {
    out << " " << i << "->";
    for(int j = 0; j < stmtSyms.src[i].size(); ++j) {
      out << stmtSyms.src[i][j];
    }
  }
  out << " ]";
  out << " }";
  return out;
}

std::ostream& operator<<(std::ostream& out, const ShAaVarSymsMap& vsmap)
{
  out << "(( ";
  for(ShAaVarSymsMap::const_iterator I = vsmap.begin(); I != vsmap.end(); ++I) {
    out << " " << (I->first->name()) << "=" << I->second; 
  }
  out << "))";
  return out;
}

ShInfo* ShAaProgramSyms::clone() const {
  return new ShAaProgramSyms(*this);
}

std::ostream& operator<<(std::ostream& out, const ShAaProgramSyms& progSyms)
{
  out << "AaProgramSyms { " 
      << " inputs=" << progSyms.inputs
      << " vars=" << progSyms.vars 
      << " outputs=" << progSyms.outputs
      << " }";
  return out;
}

void placeAaSyms(ShProgramNodePtr programNode, const ShAaVarSymsMap& inputs)
{
  WorkList worklist;
  ShProgram program(programNode);

  SH_DEBUG_PRINT_ASP("Adding Value Tracking");
  programNode->ctrlGraph->computePredecessors();
  insert_branch_instructions(program);
  add_value_tracking(program);
  remove_branch_instructions(program);

  // Place error symbols where they're needed,
  // and add defs to the worklist if they insert an extra error symbol
  SH_DEBUG_PRINT_ASP("Running error symbol initialization");
  SymInit si; 
  si.init(&inputs, &worklist);
  si.transform(programNode);

  // Propagate error symbols over control graph 
  SH_DEBUG_PRINT_ASP("Propagating syms");
  while(!worklist.empty()) {
    // data flow much the same as constprop, except we have a set of 
    // error symbols at each src/dest rather than const/uniform lattice values.
    ValueTracking::Def def = worklist.front(); worklist.pop();
    ValueTracking *vt = def.stmt->get_info<ValueTracking>();
    ShAaStmtSyms* defStmtSyms = def.stmt->get_info<ShAaStmtSyms>();

    if (!vt) {
      SH_DEBUG_PRINT_ASP(*def.stmt << " on worklist missing VT information?");
      continue;
    }
    SH_DEBUG_PRINT_ASP("WorkList def index=" << def.index << " stmt=" << *def.stmt); 
    SH_DEBUG_PRINT_ASP("  def syms =" << *defStmtSyms);

    // 

    for (ValueTracking::DefUseChain::iterator use = vt->uses[def.index].begin();
         use != vt->uses[def.index].end(); ++use) {
      ShAaStmtSyms* useStmtSyms = use->stmt->get_info<ShAaStmtSyms>();
      if (!useStmtSyms) {
        SH_DEBUG_PRINT_ASP("Use " << *use->stmt << " does not have ESU information!");
        continue;
      }
      SH_DEBUG_PRINT_ASP("  use src=" << use->source << " index=" << use->index 
          << " stmt=" << *use->stmt); 
      SH_DEBUG_PRINT_ASP("    syms=" << *useStmtSyms);
      useStmtSyms->src[use->source][use->index] |= defStmtSyms->dest[def.index];
      updateDest(useStmtSyms, worklist);
    }
  }

  // Gather syms for vars, outputs
  ShAaProgramSyms* psyms = si.psyms;
  SymGather sg;
  sg.init(psyms);
  sg.transform(programNode);
}

void clearAaSyms(ShProgramNodePtr programNode) 
{
  SymRemover sr;
  sr.transform(programNode);
}



}
