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
#include <fstream>
#include "ShDebug.hpp"
#include "ShTransformer.hpp"
#include "ShStructural.hpp"
#include "ShSection.hpp"
#include "ShOptimizations.hpp"
#include "ShAaHier.hpp"
#include "ShAaSymPlacer.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_AASYMPLACER
#endif
// #define SH_DBG_AASYMPLACER

#ifdef SH_DBG_AASYMPLACER
#define SH_DEBUG_PRINT_ASP(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASP(x) {}
#endif

// @todo range
// - check if m_changed is always being set correctly 

namespace {
using namespace SH;

typedef std::queue<ValueTracking::Def> WorkList;
typedef std::vector<ShAaIndexSet> SymLevelVec; ///< used to hold symbols at each nesting level

/* Looks at sources and initializes symbols for dest.
 * If dest is non-empty, adds the definition to the worklist 
 * @param stmtSyms The stmt error symbol assignment to update
 * @param worklist The worklist to add Definitions to
 * @param addAlways If true, then always adds a dest if it is not empty.
 *                  else only adds if it has changed */
void updateDest(ShAaStmtSyms *stmtSyms, const SymLevelVec& levelSyms, WorkList &worklist, bool addAlways=false)
{
  ShStatement* stmt = stmtSyms->stmt;
  ShAaSyms &dest = stmtSyms->dest;
  ShAaStmtSyms::SymsVec &src = stmtSyms->src;

  std::vector<bool> destChanged(dest.size(), false);

  if(stmt->op == SH_OP_ERRFROM) {
    SH_DEBUG_PRINT_ASP("    ERRFROM");
    ShAaIndexSet all; 

    for(int i = 0; i < src[1].size(); ++i) {
      all |= src[1][i];
    }

    SH_DEBUG_ASSERT(dest.size() == src[0].size());
    for(int i = 0; i < dest.size(); ++i) {
      int oldsize = dest[i].size();
      dest.merge(i, src[0][i] & all);
      destChanged[i] = (oldsize != dest[i].size());
    }
#if 0
  // @todo range - result of LASTERR is non-affine
  } else if(stmt->op == SH_OP_LASTERR) {
    SH_DEBUG_ASSERT(src[1].size() == 1);

    if(!src[1][0].empty()) {
      SH_DEBUG_ASSERT(dest.size() == src[0].size());
      for(int i = 0; i < dest.size(); ++i) {
        int oldsize = dest[i].size();
        dest[i] |= src[1][0].last();
        destChanged[i] = (oldsize != dest[i].size());
      }
    }
#endif
  } else if(stmt->op == SH_OP_ESCJOIN) {
    SH_DEBUG_PRINT_ASP("    ESCJOIN");
    SH_DEBUG_ASSERT(dest.size() == src[0].size());
    unsigned int myLevel = stmtSyms->level;
    if(myLevel < levelSyms.size()) {
      for(int i = 0; i < dest.size(); ++i) {
        int oldsize = dest[i].size();
        // get symbols added in level lower than current level
        // @todo range check that the set on the right cannot get smaller, hence
        // meaning we have to remove syms from dest... (that may not be
        // good...inf loops, nonconvergence)
        dest.merge(i, src[0][i] - levelSyms[myLevel]);
        destChanged[i] = (oldsize != dest[i].size());
      }
    }
  } else if(stmt->op == SH_OP_ESCSAV) {
    // only keep symbols generated at the current nesting level
    // We'll pick up the association between ESCSAV'd symbols and the new
    // symbols generated for ESCJOIN later in the processing.
    SH_DEBUG_PRINT_ASP("    ESCSAV"); 
    SH_DEBUG_ASSERT(dest.size() == src[0].size());
    unsigned int myLevel = stmtSyms->level;
    if(myLevel < levelSyms.size()) {
      for(int i = 0; i < dest.size(); ++i) {
        int oldsize = dest[i].size();
        dest.merge(i, src[0][i] & levelSyms[myLevel]);
        destChanged[i] = (oldsize != dest[i].size());
      }
    }
  } else if(opInfo[stmt->op].affine_keep) {
    ShOperationInfo::ResultSource result_source = opInfo[stmt->op].result_source;
    // special case certain EXTERNAL statements
    if(result_source == ShOperationInfo::EXTERNAL) {
      switch(stmt->op) {
        case SH_OP_TEX:
        case SH_OP_TEXI:
          result_source = ShOperationInfo::ALL;
          break;
        default:
          break;
      }
    }
    switch(result_source) {
      case ShOperationInfo::IGNORE: 
        break;

      case ShOperationInfo::EXTERNAL: // assume component-wise error sym propagation 
        SH_DEBUG_PRINT_ASP("    EXTERNAL");
        SH_DEBUG_PRINT_ASP("Did not deal with external op " << *stmt);
        break;


      case ShOperationInfo::LINEAR:
        {
          SH_DEBUG_PRINT_ASP("    LINEAR"); 
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
  void init(ShSectionTree* s, const ShAaVarSymsMap* inputs, WorkList *w) 
  {
    sectionTree = s; 

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

      psyms->inputs[node] = ShAaSyms(node->size(), false);
      insertSyms(0, psyms->inputs[node]);
      m_changed = true;

      SH_DEBUG_PRINT_ASP("Allocating error sym " << psyms->inputs[node] << " for input " << node->name() << " at level 0");
    }
  }

  void insertSyms(int level, const ShAaSyms& syms) {
    // resize level vec resize up to level if necessary
    if(levelSyms.size() <= (unsigned int) level) {
      levelSyms.resize(level + 1);
    }
    for(ShAaSyms::const_iterator I = syms.begin(); I != syms.end(); ++I) {
      levelSyms[level] |= *I;
    }
    SH_DEBUG_PRINT_ASP("Inserting level " << level << " syms=" << syms);
  }


  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false;

    // get node nesting level
    ShSectionNodePtr section = (*sectionTree)[node]; 
    int level = section->depth();

    ShAaStmtSyms *stmtSyms = new ShAaStmtSyms(level, &stmt); 
    stmt.destroy_info<ShAaStmtSyms>();
    stmt.add_info(stmtSyms);

    ValueTracking* vt = stmt.get_info<ValueTracking>();

    SH_DEBUG_PRINT_ASP("handleStmt stmt=" << stmt << " cur_index=" << cur_index);


    // @todo range - don't bother placing symbols if there are no affine sources
    // or dests in the statement.

    /* Handle inputs and IA sources */ 
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {

      if(psyms->inputs.find(stmt.src[i].node()) != psyms->inputs.end()) {
        if(!vt) {
          SH_DEBUG_PRINT("vt missing for stmt=" << stmt);
          stmtSyms->src[i].merge(stmt.src[i].swizzle(), psyms->inputs[stmt.src[i].node()]); 
        } else {
          ValueTracking::TupleUseDefChain& srcDefs = vt->defs[i]; 
          for(int j = 0; j < stmt.src[i].size(); ++j) {
            for(ValueTracking::UseDefChain::iterator J = srcDefs[j].begin();
                J != srcDefs[j].end(); ++J) {
              if(J->kind == ValueTracking::Def::INPUT) {
                stmtSyms->src[i][j] |= psyms->inputs[stmt.src[i].node()][stmt.src[i].swizzle()[j]];
              }
            }
          }
        }
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  fetched input sym " << stmtSyms->src[i] << " for src[" << i << "]"); 

      } else if(shIsInterval(stmt.src[i].valueType())) {
        // Assign new error symbols for IA source arguments
        stmtSyms->src[i] = ShAaSyms(stmt.src[i].swizzle());
        insertSyms(level, stmtSyms->src[i]);
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->src[i] << " for src[" << i << "] at level " << level); 
      }
    }

    // check if op adds a symbol to the output, if so, add it 
    if(opInfo[stmt.op].affine_add && shIsRange(stmt.dest.valueType())) { 
      // special cases where more than one sym need to be inserted
      switch(stmt.op) {
        case SH_OP_NORM:
          stmtSyms->newdest = ShAaSyms(ShSwizzle(ShSwizzle(1), stmt.dest.size()));
          stmtSyms->newdest |= ShAaSyms(stmt.dest.size(), false);
          break;
        default:
          stmtSyms->newdest = ShAaSyms(stmt.dest.size(), false);
          break;
      }
      stmtSyms->dest = stmtSyms->newdest;

      // insert ESCJOIN new syms up one level
      if(stmt.op == SH_OP_ESCJOIN) {
        insertSyms(level - 1, stmtSyms->newdest); 
      } else {
        insertSyms(level, stmtSyms->newdest); 
      }
      const ShStmtIndex* sidx = stmt.get_info<ShStmtIndex>();
      if(sidx) {
        psyms->stmts[*sidx] = stmtSyms->newdest;
      }
      m_changed = true;
      SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->dest << " for dest at level " << level); 
    }
    updateDest(stmtSyms, levelSyms, *worklist, true);
    return false;
  }


  void finish()
  {
    m_program->destroy_info<ShAaProgramSyms>();
    m_program->add_info(psyms);
  }

  ShSectionTree* sectionTree;
  ShAaProgramSyms* psyms;
  WorkList* worklist;
  int cur_index;

  // levelSyms[i] represents the symbols inserted at nesting level i 
  SymLevelVec levelSyms;
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
    if(stmt.dest.null()) return false;
    ValueTracking* vt = stmt.get_info<ValueTracking>();

    const ShAaStmtSyms* stmtSyms = stmt.get_info<ShAaStmtSyms>();
    if(stmtSyms) {
      SH_DEBUG_PRINT_ASP("  gathering " << stmt);
      SH_DEBUG_PRINT_ASP("     syms= " << *stmtSyms);
    }
    if(stmtSyms && !stmtSyms->dest.empty()) {
      if(psyms->vars.find(stmt.dest.node()) == psyms->vars.end()) {
        psyms->vars[stmt.dest.node()] = ShAaSyms(stmt.dest.node()->size());
      }
      psyms->vars[stmt.dest.node()].mask_merge(stmt.dest.swizzle(), stmtSyms->dest);

      // attach a comment
      std::ostringstream sout;
      sout << *stmtSyms; 
      stmt.destroy_info<ShInfoComment>();
      stmt.add_info(new ShInfoComment(sout.str()));

      // update outputs
      ShBindingType kind = stmt.dest.node()->kind();
      if(vt && (kind == SH_OUTPUT || kind == SH_INOUT) && 
          shIsAffine(stmt.dest.valueType())) { 
        for(int i = 0; i < stmt.dest.size(); ++i) {
          ValueTracking::DefUseChain::iterator I;
          for(I = vt->uses[i].begin(); I != vt->uses[i].end(); ++I) {
            if(I->kind == ValueTracking::Use::OUTPUT) {
              ShAaSyms& outSyms = psyms->outputs[stmt.dest.node()];
              if(outSyms.empty()) {
                outSyms.resize(stmt.dest.size());
              }
              outSyms[stmt.dest.swizzle()[i]] |= stmtSyms->dest[i];
            }
          }
        }
      }
    }
    return false;
  }

  // make sure all outputs have syms (in case there's an output never asn'd to)
  void finish() {
    for(ShProgramNode::VarList::iterator O = m_program->outputs.begin();
        O != m_program->outputs.end(); ++O) {
      if(!shIsAffine((*O)->valueType())) continue;
      if(psyms->outputs.find(*O) == psyms->outputs.end()) {
        psyms->outputs[*O].resize((*O)->size());
      }
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

ShAaStmtSyms::ShAaStmtSyms(int level, ShStatement* stmt)
  : level(level), 
    stmt(stmt), 
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
  out << "StmtSyms { ";
  out << " level=" << stmtSyms.level;
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
    if(i > 0) out << ", ";
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

std::ostream& operator<<(std::ostream& out, const ShAaStmtSymsMap& ssmap)
{
  out << "(( ";
  for(ShAaStmtSymsMap::const_iterator I = ssmap.begin(); I != ssmap.end(); ++I) {
    out << " " << I->first.index() << "=" << I->second; 
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

void placeAaSyms(ShProgramNodePtr programNode)
{
  ShAaVarSymsMap foo;
  placeAaSyms(programNode, foo);
}

void placeAaSyms(ShProgramNodePtr programNode, const ShAaVarSymsMap& inputs)
{
  WorkList worklist;
  ShProgram program(programNode);

  SH_DEBUG_PRINT_ASP("Clearing any old syms");
  clearAaSyms(programNode);

  SH_DEBUG_PRINT_ASP("Hierarchical Initialization"); 
  ShStructural programStruct(programNode->ctrlGraph);
  ShSectionTree sectionTree(programStruct);

  std::ofstream fout("sap_stree.dot");
  sectionTree.dump(fout);
  fout.close();
  std::string cmd = std::string("dot -Tps < sap_stree.dot > sap_stree.ps");
  system(cmd.c_str());


  inEscAnalysis(sectionTree, programNode);
#ifdef SH_DBG_AASYMPLACER 
  programNode->dump("sap_inesc");
#endif

  SH_DEBUG_PRINT_ASP("Adding Value Tracking");
  programNode->ctrlGraph->computePredecessors();
  insert_branch_instructions(program);
  add_value_tracking(program);
  remove_branch_instructions(program);

  // Place error symbols where they're needed,
  // and add defs to the worklist if they insert an extra error symbol
  SH_DEBUG_PRINT_ASP("Running error symbol initialization");
  SymInit si; 
  si.init(&sectionTree, &inputs, &worklist);
  si.transform(programNode);
  ShAaProgramSyms* psyms = si.psyms;

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

      if(use->kind != ValueTracking::Use::STMT) continue;

      ShAaStmtSyms* useStmtSyms = use->stmt->get_info<ShAaStmtSyms>();
      if (!useStmtSyms) {
        SH_DEBUG_PRINT_ASP("Use " << *use->stmt << " does not have ESU information!");
        continue;
      }
      useStmtSyms->src[use->source][use->index] |= defStmtSyms->dest[def.index];
      updateDest(useStmtSyms, si.levelSyms, worklist); 
      SH_DEBUG_PRINT_ASP("  use src=" << use->source << " index=" << use->index 
          << " stmt=" << *use->stmt); 
      SH_DEBUG_PRINT_ASP("    syms=" << *useStmtSyms);
    }
  }

  // Gather syms for vars, outputs
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
