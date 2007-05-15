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
#include <sstream>
#include <fstream>
#include <string>
#include "Debug.hpp"
#include "Transformer.hpp"
#include "Structural.hpp"
#include "Section.hpp"
#include "Optimizations.hpp"
#include "Inclusion.hpp"
#include "AaHier.hpp"
#include "AaSymPlacer.hpp"
#include "RangeBranchFixer.hpp"

#ifdef DBG_AA
bool AaspDebugOn = true; 
#else
bool AaspDebugOn = false; 
#endif


#define SH_DEBUG_PRINT_ASP(x) { if (AaspDebugOn) {SH_DEBUG_PRINT(x);} }

using namespace std;



// @todo range
// - check if m_changed is always being set correctly 

namespace 
{
using namespace SH;

string dumpPrefix = ""; 

void dump(ProgramNodePtr p, string name) 
{
  if (AaspDebugOn) {
    p->dump(dumpPrefix + name);
  }
}

typedef queue<ValueTracking::Def> WorkList;
typedef list<CtrlGraphNode*> CfgWorkList; // @todo really should be something other than a basic queue (to reduce duplicate CFG nodes)
typedef list<Statement*> StmtWorkList; // @todo really should be something other than a basic queue (to reduce duplicate CFG nodes)
typedef vector<AaIndexSet> SymLevelVec; ///< used to hold symbols at each nesting level

/* Given an Associative Container a and Container c, inserts elements of c into a */
template<class A, class C>
void insert(A& a, C& c) {
  a.insert(c.begin(), c.end());
}

bool isEmpty(CtrlGraphNode* node) {
  return !node->block || node->block->empty();
}

/* Adds stmt syms as a comment to a statement if in debug mode*/
void symComment(Statement& stmt, AaStmtSyms* syms) {
  if(AaspDebugOn) {
    if(syms->empty()) return;
    ostringstream sout;
    sout << *syms; 
    stmt.destroy_info<InfoComment>();
    stmt.add_info(new InfoComment(sout.str()));
  }
}

/* Pulls out any interval definitions later used in affine ops so repeated uses end 
 * up with the same error symbols */ 
struct IntervalPullerBase: public TransformerParent 
{
  /* Gather syms for vars */
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;
    if(!isInterval(stmt.dest.valueType())) return false;
    SH_DEBUG_PRINT_ASP("IPB Handling statement " << stmt);

    ++I;

    ValueTracking* vt = stmt.get_info<ValueTracking>();
    Variable aaDup(0);
    for(size_t i = 0; i < stmt.dest.size(); ++i) {
      ValueTracking::DefUseChain::iterator U = vt->uses[i].begin();
      for(;U != vt->uses[i].end(); ++U) {
        if(U->kind != ValueTracking::Use::STMT) continue;
        bool aastmt = false;
        Statement* us = U->stmt;
        if(!us->dest.null()) {
          aastmt |= isAffine(us->dest.valueType());
        }
        for(size_t j = 0; us->src.size() < 3; ++j) {
          if(us->src[j].null()) break;
          aastmt |= isAffine(us->src[j].valueType());
        }
        if(aastmt) {
          SH_DEBUG_PRINT_ASP("  fixing use in stmt " << *us);
          if(aaDup.null()) {
            aaDup = Variable(stmt.dest.node()->clone(SH_TEMP, 0, affineValueType(stmt.dest.valueType())));
            aaDup.name(stmt.dest.node()->name() + "_i2aa_dup");
            node->block->insert(I, Statement(aaDup, OP_ASN, Variable(stmt.dest.node()))); 
          }
          Variable &use = us->src[U->source];
          use = Variable(aaDup.node(), use.swizzle(), use.neg());  
        } 
      }
    }


    return true;
  }
};
typedef DefaultTransformer<IntervalPullerBase> IntervalPuller;


/* Places symbols for inputs, intervals used as sources, and statement
 * destinations when a statement is non-affine.
 */
// @todo make this more efficient
// (either only make the changes necessary per component, or use per-statement
// worklists instead of per-def)
struct SymTransferBase: public TransformerParent 
{
  SymTransferBase()
    : psyms(new AaProgramSyms()), cur_index(0) 
  {}

  /* Handle predefined error symbols.  Start new error symbols from one more
   * than the maximum predefined error symbol 
   * Assigns the worklist to fill with w
   *
   * This must be called exactly once before each transform call */
  void init(const AaVarSymsMap* inputs) 
  {
    psyms->inputs = *inputs;
    int max_index = -1;
    for(AaVarSymsMap::const_iterator I = inputs->begin(); I != inputs->end(); ++I) {
      SH_DEBUG_PRINT_ASP("Allocating error sym " << I->second << " for input " << I->first->name() << " (predefined)"); 
      for(AaSyms::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {
        for(AaIndexSet::const_iterator K = J->begin(); K != J->end(); ++K) {
          max_index = max(max_index, *K);    
        }
      }
    }
    cur_index = max_index + 1;
    SH_DEBUG_PRINT_ASP("After init cur_index=" << cur_index);
  }

  // Initializes input error symbols
  void handleVarList(ProgramNode::VarList& varList, BindingType type)
  {
    if(type != SH_INPUT) return;
    ProgramNode::VarList::const_iterator V = varList.begin();

    for(; V != varList.end(); ++V) {
      VariableNodePtr node = *V;
      if(!isAffine(node->valueType())) continue; 
      else if(psyms->inputs.find(node) != psyms->inputs.end()) continue;

      AaSyms& as = psyms->inputs[node] = AaSyms(node->size(), false);
      for(int i = 0; i < as.size(); ++i) {
        input_symbols |= as[i];
      }
      insertSyms(0, psyms->inputs[node]);
      m_changed = true;

      SH_DEBUG_PRINT_ASP("Allocating error sym " << psyms->inputs[node] << " for input " << node->name() << " at level 0");
    }
  }

  void insertSyms(int level, const AaSyms& syms) 
  {
    // resize level vec resize up to level if necessary
    if(levelSyms.size() <= (unsigned int) level) {
      levelSyms.resize(level + 1);
    }
    for(AaSyms::const_iterator I = syms.begin(); I != syms.end(); ++I) {
      levelSyms[level] |= *I;
    }
    SH_DEBUG_PRINT_ASP("Inserting level " << level << " syms=" << syms);
  }


  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;

    // get node nesting level in structural tree
    StmtDepth* sd = I->get_info<StmtDepth>();
    int level = 0;
    if(sd) level = sd->depth();

    AaStmtSyms *stmtSyms = new AaStmtSyms(level, &stmt); 
    stmt.destroy_info<AaStmtSyms>();
    stmt.add_info(stmtSyms);

    ValueTracking* vt = stmt.get_info<ValueTracking>();
    SH_DEBUG_ASSERT(vt);

    SH_DEBUG_PRINT_ASP("handleStmt stmt=" << stmt << " cur_index=" << cur_index);


    // @todo range - don't bother placing symbols if there are no affine sources
    // or dests in the statement.

    /* Handle inputs and IA sources */ 
    bool aa_op = isAffine(stmt.dest.valueType()); /* whether the stmt is an affine arithmetic op */
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
      if(isAffine(stmt.src[i].valueType())) aa_op = true;
      if(psyms->inputs.find(stmt.src[i].node()) != psyms->inputs.end()) {
        if(!vt) {
          SH_DEBUG_PRINT("vt missing for stmt=" << stmt);
          stmtSyms->src[i].merge(stmt.src[i].swizzle(), psyms->inputs[stmt.src[i].node()]); 
        } else {
          ValueTracking::TupleUseDefChain& srcDefs = vt->defs[i]; 
          for(int j = 0; j < stmt.src[i].size(); ++j) {
            for(ValueTracking::UseDefChain::iterator J = srcDefs[j].begin();
                J != srcDefs[j].end(); ++J) {
              if(J->kind == ValueTracking::Def::SH_INPUT) {
                stmtSyms->src[i][j] |= psyms->inputs[stmt.src[i].node()][stmt.src[i].swizzle()[j]];
              }
            }
          }
        }
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  fetched input sym " << stmtSyms->src[i] << " for src[" << i << "]"); 

      } else if(isInterval(stmt.src[i].valueType())) {
        // Assign new error symbols for IA source arguments
        stmtSyms->src[i] = AaSyms(stmt.src[i].swizzle());
        insertSyms(level, stmtSyms->src[i]);
        m_changed = true;
        SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->src[i] << " for src[" << i << "] at level " << level); 
      }
    }

    // check if op adds a symbol to the output, if so, add it 
    if(aa_op && opInfo[stmt.op].affine_add && isRange(stmt.dest.valueType())) { 
      // special cases where more than one sym need to be inserted
      switch(stmt.op) {
        case OP_NORM:
          stmtSyms->newdest = AaSyms(Swizzle(Swizzle(1), stmt.dest.size()));
          stmtSyms->newdest |= AaSyms(stmt.dest.size(), false);
          break;
        default:
          stmtSyms->newdest = AaSyms(stmt.dest.size(), false);
          break;
      }
    // not necessar - handled in updateDest now
    //  stmtSyms->dest = stmtSyms->newdest;

      // insert ESCJOIN new syms up one level
      if(stmt.op == OP_ESCJOIN) {
        insertSyms(level - 1, stmtSyms->newdest); 
      } else {
        insertSyms(level, stmtSyms->newdest); 
      }
      const StmtIndex* sidx = stmt.get_info<StmtIndex>();
      if(sidx) {
        psyms->stmts[*sidx] = stmtSyms->newdest;
      }
      m_changed = true;
      SH_DEBUG_PRINT_ASP("  allocating error sym " << stmtSyms->dest << " for dest at level " << level); 
    }
    updateDest(stmtSyms, levelSyms, true);
    return false;
  }


  void finish()
  {
    m_program->destroy_info<AaProgramSyms>();
    psyms->maxSym = cur_index;
    m_program->add_info(psyms);
  }

  /* dataflow propagation of noise symbols through operations 
   * using the transfer function updateDest defined above 
   *
   * @param increasing - whether lattice values are increasing in size or
   * decreasing  */
  void propagate(bool increasing) {
    while(!worklist.empty()) {
      // data flow much the same as constprop, except we have a set of 
      // error symbols at each src/dest rather than const/uniform lattice values.
      ValueTracking::Def def = worklist.front(); worklist.pop();
      ValueTracking *vt = def.stmt->get_info<ValueTracking>();
      AaStmtSyms* defStmtSyms = def.stmt->get_info<AaStmtSyms>();

      SH_DEBUG_ASSERT(vt);
      SH_DEBUG_ASSERT(defStmtSyms);

      SH_DEBUG_PRINT_ASP("WorkList def index=" << def.index << " stmt=" << *def.stmt); 
      SH_DEBUG_PRINT_ASP("  def syms =" << *defStmtSyms);

      // 

      for (ValueTracking::DefUseChain::iterator use = vt->uses[def.index].begin();
           use != vt->uses[def.index].end(); ++use) {

        if(use->kind != ValueTracking::Use::STMT) continue;

        AaStmtSyms* useStmtSyms = use->stmt->get_info<AaStmtSyms>();
        if (!useStmtSyms) {
          SH_DEBUG_PRINT_ASP("Use " << *use->stmt << " does not have ESU information!");
          continue;
        }
        AaIndexSet& useIndexSet = useStmtSyms->src[use->source][use->index];
        if(increasing) {
          useIndexSet |= defStmtSyms->mergeDest[def.index];
        } else {
          ValueTracking *useVt = use->stmt->get_info<ValueTracking>();
          SH_DEBUG_ASSERT(useVt);

          useIndexSet.clear();
          for(ValueTracking::UseDefChain::iterator D = useVt->defs[use->source][use->index].begin();
              D != useVt->defs[use->source][use->index].end(); ++D) {
            AaIndexSet* defIndexSet = 0;
            switch(D->kind) {
              case ValueTracking::Def::SH_INPUT: {
                Variable src = use->stmt->src[use->source];
                defIndexSet = &psyms->inputs[src.node()][src.swizzle()[use->index]];
                break;
              }
              case ValueTracking::Def::STMT: {
                AaStmtSyms* DSyms = D->stmt->get_info<AaStmtSyms>();
                SH_DEBUG_ASSERT(DSyms);
                defIndexSet = &(DSyms->mergeDest[D->index]);
                break;
              }
            }
            useIndexSet |= *defIndexSet; 
          }
        }
        updateDest(useStmtSyms, levelSyms); 
        SH_DEBUG_PRINT_ASP("  use src=" << use->source << " index=" << use->index 
            << " stmt=" << *use->stmt); 
        SH_DEBUG_PRINT_ASP("    syms=" << *useStmtSyms);
      }
    }
  }

  // run an updatedest on stmt and add necessary defs to the worklist 
  // (should call propagate afterwards)
  void add(Statement* stmt) {
    /*
    for(int i = 0; i < stmt->dest.size(); ++i) {
      worklist.push(ValueTracking::Def(stmt, i));
    }
    */
    AaStmtSyms* stmtSyms = stmt->get_info<AaStmtSyms>();
    SH_DEBUG_ASSERT(stmtSyms);
    updateDest(stmtSyms, levelSyms);
  }

  SectionTree* sectionTree;
  AaProgramSyms* psyms;
  int cur_index;

  // levelSyms[i] represents the symbols inserted at nesting level i 
  SymLevelVec levelSyms;

  // input symbols 
  AaIndexSet input_symbols;

  private:
    /* Transfer function for noise symbol dataflow propagation.
     * Looks at sources and passes appropriate symbols to dest and mergedest.
     * If mergeDest changes, adds the definition to the worklist 
     *
     * (This only updates dest, during/after unique merging, mergeDest needs to
     * updated accordingly as well...)
     *
     * Note: This method only works in the case of a monotonic dataflow analysis
     * (i.e. either updated sets are always subsets or always supersets of original
     * sets).  For non-monotonic, change line marked NONMON to actually compare
     * sets instead of just sizes.
     *
     * Performs the following updates:
     * dest = newdest + src (if LINEAR or ALL)
     * mergeDest = dest - unique + mergeRep;
     *
     * @param stmtSyms The stmt error symbol assignment to update
     * @param addAlways If true, then always adds a dest if it is not empty.
     *                  else only adds if it has changed */
    void updateDest(AaStmtSyms *stmtSyms, const SymLevelVec& levelSyms, bool addAlways=false)
    {
      Statement* stmt = stmtSyms->stmt;
      AaSyms &dest = stmtSyms->dest;
      AaSyms &mergeDest = stmtSyms->mergeDest;
      AaStmtSyms::SymsVec &src = stmtSyms->src;
      int size = mergeDest.size();

      dest = stmtSyms->newdest;
      if(stmt->op == OP_ERRFROM) {
        SH_DEBUG_PRINT_ASP("    ERRFROM");
        AaIndexSet all; 

        for(int i = 0; i < src[1].size(); ++i) {
          all |= src[1][i];
        }

        SH_DEBUG_ASSERT(size == src[0].size());
        for(int i = 0; i < size; ++i) {
          dest.merge(i, src[0][i] & all);
        }
#if 0
      // @todo range - result of LASTERR is non-affine
      } else if(stmt->op == OP_LASTERR) {
        SH_DEBUG_ASSERT(src[1].size() == 1);

        if(!src[1][0].empty()) {
          SH_DEBUG_ASSERT(size == src[0].size());
          for(int i = 0; i < size; ++i) {
            int oldsize = dest[i].size();
            dest[i] |= src[1][0].last();
            destChanged[i] = (oldsize != dest[i].size());
          }
        }
#endif
      } else if(stmt->op == OP_ESCJOIN) {
        SH_DEBUG_PRINT_ASP("    ESCJOIN");
        SH_DEBUG_ASSERT(size == src[0].size());
        unsigned int myLevel = stmtSyms->level;
        if(myLevel < levelSyms.size()) {
          for(int i = 0; i < size; ++i) {
            dest.merge(i, src[0][i] - levelSyms[myLevel]);
          }
        }
      } else if(stmt->op == OP_ESCSAV) {
        // only keep symbols generated at the current nesting level
        // We'll pick up the association between ESCSAV'd symbols and the new
        // symbols generated for ESCJOIN later in the processing.
        SH_DEBUG_PRINT_ASP("    ESCSAV"); 
        SH_DEBUG_ASSERT(size == src[0].size());
        unsigned int myLevel = stmtSyms->level;
        if(myLevel < levelSyms.size()) {
          for(int i = 0; i < size; ++i) {
            dest.merge(i, src[0][i] & levelSyms[myLevel]);
          }
        }
      } else if(opInfo[stmt->op].affine_keep) {
        OperationInfo::ResultSource result_source = opInfo[stmt->op].result_source;
        // special case certain EXTERNAL statements
        if(result_source == OperationInfo::EXTERNAL) {
          switch(stmt->op) {
            case OP_TEX:
            case OP_TEXI:
              result_source = OperationInfo::ALL;
              break;
            default:
              break;
          }
        }
        switch(result_source) {
          case OperationInfo::IGNORE: 
            break;

          case OperationInfo::EXTERNAL: // assume component-wise error sym propagation 
            SH_DEBUG_PRINT_ASP("    EXTERNAL");
            SH_DEBUG_PRINT_ASP("Did not deal with external op " << *stmt);
            break;


          case OperationInfo::LINEAR:
            {
              SH_DEBUG_PRINT_ASP("    LINEAR"); 
              for(int i = 0; i < size; ++i) {
                for(int j = 0; j < opInfo[stmt->op].arity; ++j) {
                  bool scalarSrc = stmt->src[j].size() == 1;
                  dest.merge(i, src[j][scalarSrc ? 0 : i]);
                }
              }
              break;
            }
          case OperationInfo::ALL: // all src error syms may go to dest
            {
              AaIndexSet all;
              for(int i = 0; i < opInfo[stmt->op].arity; ++i) {
                for(int j = 0; j < src[i].size(); ++j) {
                  all |= (src[i][j]);
                }
              }
              SH_DEBUG_PRINT_ASP("    ALL all=" << all);
              for(int i = 0; i < size; ++i) {
                dest.merge(i, all);
              }
              break;
            }
          default: 
            SH_DEBUG_PRINT_ASP("The king has left the building.  You should not be here either.");
        }
      }

      // Only propagate changes from dest to mergeDest if the result variable is affine.  
      // Otherwise, it is interval and the error syms disappear at this point
      if(!isAffine(stmt->dest.valueType())) {
        symComment(*stmt, stmtSyms);
        return;
      }

      AaSyms tempMergeDest = (dest - stmtSyms->unique);
      tempMergeDest |= stmtSyms->mergeRep; 
      for(int i = 0; i < dest.size(); ++i) {
        if(addAlways || (tempMergeDest[i] != mergeDest[i])) {
          // @todo check if size check is enough above 
          mergeDest[i] = tempMergeDest[i];
          worklist.push(ValueTracking::Def(stmt, i));
        }
      }
      symComment(*stmt, stmtSyms);

      return; 
    }
    // current worklist
    WorkList worklist;

};
typedef DefaultTransformer<SymTransferBase> SymTransfer;




/* Structure used in unique merging to find live def ranges. 
 * A LiveDef represents the set of definitions that may be live 
 * upon entering and exiting a statement (it's a per-statement Info).
 *
 * (Based on typical live-variable analysis (see apel, aho sethi ullman),
 * use = any def used in a stmt src
 * def = any def resulting from this statement 
 *
 * dead = (def union in) - out 
 * (may also include def 
 *
 * rchout = reaching defs after this statement (as found in Valuetracking.cpp)
 *          (used only on last statement in a block to make live def analysis tighter))
 *
 * This alters the computation of out, which is normally
 *    out = union_{S in succs} in(S)
 * to be
 *    out' = out intersect rchout 
 * 
 * pred/succ are statements that come before after this statement 
 *    (0 may be used in the pred list if statement can be first 
 *     succ list if it can be last)
 *
 *
 *  
 */
typedef set<ValueTracking::Def> DefSet;
typedef set<Statement*> StmtSet;  
struct LiveDef: public Info 
{
  DefSet in, use, def, out, rchout;
  DefSet dead; 

  // @todo should be computing these from CFG, but don't  
  StmtSet pred, succ;

  Info* clone() const 
  {
    return new LiveDef(*this);
  }

  friend ostream& operator<<(ostream& out, const LiveDef& ld);
};

ostream& operator<<(ostream& out, const DefSet& ds) {
  int lastIndex = -1;
  for(DefSet::const_iterator D = ds.begin(); D != ds.end(); ++D) {
    if(D->kind == ValueTracking::Def::SH_INPUT) {
      out << "in[" << D->node->name() << "](" << D->index << ")";
      continue;
    }

    StmtIndex* stmtIdx = D->stmt->get_info<StmtIndex>();
    SH_DEBUG_ASSERT(stmtIdx);
    if(lastIndex != stmtIdx->index()) {
      if(D != ds.begin()) {
        out << ", ";
      }
      lastIndex = stmtIdx->index();
      out << "#" << lastIndex;
    } 
    out << "(" << D->index << ")";
  }
  return out;
}

ostream& operator<<(ostream& out, const StmtSet& ss) {
  for(StmtSet::const_iterator S = ss.begin(); S != ss.end(); ++S) {
    if(S != ss.begin()) {
      out << ", ";
    }
    if((*S) == 0) {
      out << "NULL"; 
      continue;
    }
    StmtIndex* idx = (*S)->get_info<StmtIndex>();
    SH_DEBUG_ASSERT(idx);
    out << "#" << idx->index();
  }
  return out;
}

ostream& operator<<(ostream& out, const LiveDef& ld) {
  out << "<LiveDef in: " << ld.in << " use: " << ld.use << " def:" << ld.def
      << " out:" << ld.out << " rchout: " << ld.rchout << " dead:" << ld.dead 
      << " pred: " << ld.pred << " succ:" << ld.succ << ">";
  return out;
}

/* Statements that may come before or after a CFG node */ 
struct AdjStmt: public Info {
  StmtSet pred, succ;

  Info* clone() const 
  {
    return new AdjStmt(*this);
  }
};

/* Structure used to compute pred/succ in statements
 * This does it per CFG node by propagating across empty nodes 
 */
struct StmtClosureBase: TransformerParent {
  CfgWorkList worklist;

  void start(ProgramNodePtr program) {
    TransformerParent::start(program);
  }

  // @todo think about and simplify this...
  void handleNode(CtrlGraphNode* node) {
    worklist.push_back(node);
    AdjStmt* adj = new AdjStmt();
    node->add_info(adj);

    SH_DEBUG_PRINT_ASP("StmtClosure Node ");

    if(node == m_program->ctrlGraph->entry()) {
      adj->pred.insert(0);
      SH_DEBUG_PRINT_ASP("  Pred Entry");
    }

    if(node == m_program->ctrlGraph->exit()) {
      adj->succ.insert(0);
      SH_DEBUG_PRINT_ASP("  Succ Exit");
    }

    CtrlGraphNode::PredecessorIt P = node->predecessors_begin();
    for(;P != node->predecessors_end(); ++P) {
      if(!isEmpty(*P)) {
        adj->pred.insert(&*(*P)->block->rbegin());
        SH_DEBUG_PRINT_ASP("  Pred " << *(*P)->block->rbegin());
      }
    }

    CtrlGraphNode::SuccessorConstIt S = node->successors_begin();
    for(; S != node->successors_end(); ++S) {
      if(!isEmpty(S->node)) {
        adj->succ.insert(&*(S->node)->block->begin());
        SH_DEBUG_PRINT_ASP("  Succ " << *(S->node)->block->begin());
      }
    }
    if(node->follower() && !isEmpty(node->follower())) {
      adj->succ.insert(&*node->follower()->block->begin());
      SH_DEBUG_PRINT_ASP("  Succ " << *node->follower()->block->begin());
    }
  }

  AdjStmt* getAdj(CtrlGraphNode* node) {
    AdjStmt* result = node->get_info<AdjStmt>();
    if(!result) {
      SH_DEBUG_PRINT_ASP("Null node adjstmt!");
    }
    SH_DEBUG_ASSERT(result);
    return result;
  }

  void finish() {
    while(!worklist.empty()) {
      CtrlGraphNode* node = worklist.front(); worklist.pop_front();
      AdjStmt* adj = getAdj(node); 

      size_t predSize = adj->pred.size();
      size_t succSize = adj->succ.size();

      CtrlGraphNode::PredecessorIt P = node->predecessors_begin();
      for(;P != node->predecessors_end(); ++P) {
        if(isEmpty(*P)) {
          insert(adj->pred, getAdj(*P)->pred);
        }
      }

      CtrlGraphNode::SuccessorConstIt S = node->successors_begin();
      for(; S != node->successors_end(); ++S) {
        if(isEmpty(S->node)) {
          insert(adj->succ, getAdj(S->node)->succ);
        }
      }
      if(node->follower() && isEmpty(node->follower())) {
        insert(adj->succ, getAdj(node->follower())->succ);
      }

      if(isEmpty(node)) {
        if(succSize != adj->succ.size()) {
          worklist.insert(worklist.end(), node->predecessors_begin(), node->predecessors_end());
        }

        if(predSize != adj->pred.size()) {
          for(CtrlGraphNode::SuccessorIt S = node->successors_begin();
              S != node->successors_end(); ++S) {
            worklist.push_back(S->node);
          }
          if(node->follower()) {
            worklist.push_back(node->follower());
          }
        }
      }
    }
  }
};
typedef DefaultTransformer<StmtClosureBase> StmtClosure;


// Initializes use, def sets and pred, succ sets 
struct LiveDefFinderBase: TransformerParent {
  StmtWorkList worklist;
  ReachingDefs* rchDef;

  void start(ProgramNodePtr program) {
    TransformerParent::start(program);

    StmtClosure sc;
    sc.transform(program);

    rchDef = program->get_info<ReachingDefs>();
    SH_DEBUG_ASSERT(rchDef);
  }

  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) 
  { 
    Statement& stmt = *I;

    AdjStmt* nodeAdj = node->get_info<AdjStmt>();

    LiveDef* liveDef = new LiveDef(); 
    stmt.add_info(liveDef);
    worklist.push_back(&stmt);
    SH_DEBUG_ASSERT(nodeAdj);

    if(!stmt.dest.null()) {
      ValueTracking* vt = stmt.get_info<ValueTracking>();
      SH_DEBUG_ASSERT(vt);

      // add items to use(n) 
      ValueTracking::TupleUseDefChainVec::const_iterator J;
      ValueTracking::TupleUseDefChain::const_iterator K;
      ValueTracking::UseDefChain::const_iterator L;
      for(J = vt->defs.begin(); J != vt->defs.end(); ++J) {
        for(K = J->begin(); K != J->end(); ++K) {
          for(L = K->begin(); L != K->end(); ++L) {
            insert(liveDef->use, *K);
          }
        }
      }

      // set up def(n) by adding any definitions that happen before use
      // @todo check if this works in loops...
      for(int i = 0; i < stmt.dest.size(); ++i) {
        liveDef->def.insert(ValueTracking::Def(&stmt, i));
      }
    }

    // set up pred/succ
    if(I == node->block->begin()) {
      insert(liveDef->pred, nodeAdj->pred);
    } else {
      BasicBlock::StmtList::iterator J = I;
      --J;
      liveDef->pred.insert(&*J);
    }

    if(&*I == &*node->block->rbegin()) {
      insert(liveDef->succ, nodeAdj->succ); 
      // also set up rchout
      SH_DEBUG_ASSERT(rchDef->out.find(node) != rchDef->out.end());
      const BitSet& nodeOut = rchDef->out[node];
      for(int i = 0; i < rchDef->defs.size(); ++i) {
        ReachingDefs::Definition &d = rchDef->defs[i];
        for(int j = 0; j < d.size(); ++j) {
          if(nodeOut[d.off(j)]) {
            liveDef->rchout.insert(d.toDef(j));
          }
        }
      }
    } else {
      BasicBlock::StmtList::iterator J = I;
      ++J;
      liveDef->succ.insert(&*J);
    }

    SH_DEBUG_PRINT_ASP("LiveDef - Init - " << stmt << " = " << *liveDef);

    return false;
  }

  // finishes initialization (add defs that are used in outputs to the exit node)
  // then runs dataflow propagation 
  void finish() 
  {
    DefSet result;
    OutputValueTracking* ovt = m_program->get_info<OutputValueTracking>();
    SH_DEBUG_ASSERT(ovt);

    // add items to use(exit)
    OutputValueTracking::OutputTupleUseDefChain::const_iterator I;
    ValueTracking::TupleUseDefChain::const_iterator J; 
    for(I = ovt->outputDefs.begin(); I != ovt->outputDefs.end(); ++I) {
      for(J = I->second.begin(); J != I->second.end(); ++J) {
        insert(result, *J);
      }
    }

    CtrlGraphNode* exitNode = m_program->ctrlGraph->exit(); 
    if(!isEmpty(exitNode)) {
      Statement& last = *exitNode->block->rbegin();
      LiveDef* liveDef = last.get_info<LiveDef>();
      SH_DEBUG_ASSERT(liveDef);

      insert(liveDef->out, result);
    } else {
      AdjStmt* exitAdj = exitNode->get_info<AdjStmt>();
      SH_DEBUG_ASSERT(exitAdj);
      for(StmtSet::iterator S = exitAdj->pred.begin();
          S != exitAdj->pred.end(); ++S) {
        if(!*S) { // don't need to propagate back to entry
          continue;
        }
        LiveDef* liveDef = (*S)->get_info<LiveDef>();
        SH_DEBUG_ASSERT(liveDef);

        insert(liveDef->out, result);
      }
    }

    // done init

    // propagate and finally compute LiveDef->dead sets
    propagate();
  }

  // run dataflow
  void propagate()
  {
    while(!worklist.empty()) {
      Statement* stmt = worklist.front(); worklist.pop_front();
      if(!stmt) continue; // ignore pred starts

      LiveDef* ld = stmt->get_info<LiveDef>();

      // update out(N) = (union in(S), S is in succ(N)) intersect rchout(N)
      // (ignore rchout part if stmt is not last) 
      StmtSet::iterator S = ld->succ.begin();
      for(; S != ld->succ.end(); ++S) {
        if(!*S) { // don't need to consider exit...it's alread considered
          continue;
        }
        LiveDef* Sld = (*S)->get_info<LiveDef>();
        if(ld->rchout.empty()) {
          insert(ld->out, Sld->in);
        } else {
          set_intersection(Sld->in.begin(), Sld->in.end(), ld->rchout.begin(), ld->rchout.end(),
                           inserter(ld->out, ld->out.begin()));
        }
      }

      size_t inCount = ld->in.size();

      // update in(N) = use(N) union (out(N) - def(N))
      DefSet outLessDef;
      insert_iterator<DefSet> I(outLessDef, outLessDef.begin()); 
      set_difference(ld->out.begin(), ld->out.end(), ld->def.begin(), ld->def.end(), I);

      insert(ld->in, ld->use);
      insert(ld->in, outLessDef);
      if(ld->in.size() > inCount) { // add preds to worklist
        worklist.insert(worklist.end(), ld->pred.begin(), ld->pred.end()); 
      }

      SH_DEBUG_PRINT_ASP("LiveDef - Prop - " << *stmt << " = " << *ld << ", out - def = " << outLessDef);
      /*
      ostringstream sout;
      sout << *ld;
      stmt->destroy_info<InfoComment>();
      stmt->add_info(new InfoComment(sout.str()));
      */
    }
  }
};
typedef DefaultTransformer<LiveDefFinderBase> LiveDefFinder;

/* Collects LiveDef->dead information 
 * dead = (in union def) - out = (in - out) union (def - out) */

struct ReaperBase: TransformerParent {
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;

    LiveDef* liveDef = stmt.get_info<LiveDef>();

    liveDef->dead.clear();
    insert_iterator<DefSet> D(liveDef->dead, liveDef->dead.begin());

    set_difference(liveDef->in.begin(), liveDef->in.end(),
                   liveDef->out.begin(), liveDef->out.end(),
                   D);

    set_difference(liveDef->def.begin(), liveDef->def.end(),
                   liveDef->out.begin(), liveDef->out.end(),
                   D);
    SH_DEBUG_PRINT_ASP("LiveDef - Reap - " << stmt << " = " << *liveDef);
    return false; 
  }
};
typedef DefaultTransformer<ReaperBase> Reaper;



/* Structure for unique merging that holds noise symbol counts for each cfg node 
 * (The second dataflow step) */
struct SymCount
{
  // count[i] = number of times noise symbol
  typedef map<int, int> SymCountMap;
  typedef SymCountMap::iterator iterator;
  typedef SymCountMap::const_iterator const_iterator;
  SymCountMap count; 

  // per tuple element counts
  typedef pair<VariableNodePtr, int> Element;  
  typedef map<Element, SymCountMap> VarCountMap;
  VarCountMap varCount;
  AaProgramSyms* psyms;

  SymCount(): psyms(0) {}
  

  // Re-initializes count from a set of live definitions 
  void init(const DefSet& defs, AaProgramSyms* psyms) 
  {
    this->psyms = psyms;
    count.clear();
    varCount.clear();
    inc(defs);
  }

  void inc(const DefSet& defs) {
    for (DefSet::const_iterator I = defs.begin(); I != defs.end(); ++I) {
      inc(*I);
    }
  }

  void dec(const DefSet& defs) {
    for (DefSet::const_iterator I = defs.begin(); I != defs.end(); ++I) {
      dec(*I);
    }
  }

  void inc(const ValueTracking::Def& def) {
    update<1>(def);
  }

  void dec(const ValueTracking::Def& def) {
    update<-1>(def);
  }

  /* Finds all unique symbols, excluding special symbols */
  AaIndexSet unique(const AaIndexSet& special) const {
    AaIndexSet result;
    for(SymCountMap::const_iterator I = count.begin(); I != count.end(); ++I) {
      if(I->second == 1 && !special.contains(I->first)) {
        result |= I->first;
      }
    }
    return result;
  }

  iterator begin() { return count.begin(); }
  iterator end() { return count.end(); }

  const_iterator begin() const { return count.begin(); }
  const_iterator end() const { return count.end(); }

  string tocsv() {
    ostringstream sout;
    for(const_iterator I = begin(); I != end(); ++I) {
      if (I != begin()) sout << ",";
      sout << I->first << ":" << I->second; 
    }
    return sout.str();
  }

  private:
    template<int DELTA>
    void update(const ValueTracking::Def& def) 
    {
      VariableNodePtr defNode; 
      int rawIndex = def.absIndex();
      AaIndexSet* indexSet;
      switch(def.kind) {
        case ValueTracking::Def::STMT: {
          Statement& stmt = *def.stmt;
          SH_DEBUG_ASSERT(def.kind == ValueTracking::Def::STMT);
          int index = def.index;

          defNode = stmt.dest.node();

          AaStmtSyms* stmtSyms = stmt.get_info<AaStmtSyms>();
          SH_DEBUG_ASSERT(stmtSyms);
          indexSet = &stmtSyms->mergeDest[index];
          break;
        }
        case ValueTracking::Def::SH_INPUT: {
          defNode = def.node;
          if(psyms->inputs.find(defNode) == psyms->inputs.end()) { // not AA, no need to update 
            return;
          }
          indexSet = &psyms->inputs[defNode][rawIndex]; 
          break;
        }
      } 
      SymCountMap& defVarCount = varCount[Element(defNode, rawIndex)];
      AaIndexSet::const_iterator I, is_end; 
      I = indexSet->begin(); 
      is_end = indexSet->end();

      for(;I != is_end; ++I) {
        int oldCount = defVarCount[*I];
        int newCount = (defVarCount[*I] += DELTA);
        int& counti = count[*I];

        if(oldCount == 0 && newCount > 0) {
          counti++;
        } else if (oldCount > 0 && newCount == 0) {
          counti--;
        }
        if(newCount < 0 || counti < 0) {
          SH_DEBUG_PRINT("bad bad bad...*I " << *I << " oldCount" << oldCount << " newCount " << 
              newCount << " counti " << counti);
        }
        SH_DEBUG_ASSERT(newCount >= 0 && counti >= 0);
      }
    }

  friend ostream& operator<<(ostream& out, const SymCount& sc);

};

ostream& operator<<(ostream& out, const SymCount& sc) {
  // @todo 
  out << "<SymCount count=";
  for(SymCount::const_iterator I = sc.begin(); I != sc.end(); ++I) {
    out << I->first << ":" << I->second << " ";
  }
  
  out << "unique=" << sc.unique(AaIndexSet()) << ">"; 
  return out;
}

/* Collects special symbols (those that we should not merge) */
struct SpecialFinderBase: TransformerParent 
{
  AaIndexSet special;
  void init(const AaIndexSet& input_symbols) {
    special = input_symbols;
  }

  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node) 
  {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;
    if(stmt.op != OP_ERRFROM && stmt.op != OP_LASTERR) return false;

    AaStmtSyms* syms = stmt.get_info<AaStmtSyms>();
    for(int i = 0; i < syms->src[1].size(); ++i) {
      special |= syms->src[1][i];
    }
    return false;
  }

  void finish() {
  }
};
typedef DefaultTransformer<SpecialFinderBase> SpecialFinder; 

// This works accelerated on a per-node basis
// (only  
//
// At a node, the sym counts are initialized based on the first statement.
// Then the iteration goes through each statement
//  -increments counts for definitions
//  -decrements counts for deaths
//
//  dumpStats toggles dumping of per-statement statistics
//  disableMerge toggles disabling of any actual merging (only useful with dumpStats for
//    dumping live symbols without merging) 
//
struct UniqueMergerBase: TransformerParent 
{
  SymCount symCount;
  SymTransfer* symTransfer;
  bool dumpStats, disableMerge, csvSyms;
  string symfile; 
  ofstream sout;
  size_t iteration;
  AaIndexSet special;

  void init(SymTransfer* st, const AaIndexSet& special, bool dumpStats, bool disableMerge, const string& symfile)
  {
    iteration = 0;
    this->special = special;
    this->dumpStats = dumpStats; 
    this->disableMerge = disableMerge;
    symTransfer = st;
    csvSyms = symfile != "";
    this->symfile = symfile;
  }


  void start(ProgramNodePtr program) {
    TransformerParent::start(program);
    iteration += 1;
    SH_DEBUG_PRINT_ASP("Unique merging iteration" << iteration << " changed=" << m_changed);
    if(csvSyms) {
      ostringstream idout;
      idout << symfile;
      idout << "_it" << iteration;
      sout.open(idout.str().c_str());
      sout << "um=" << !disableMerge << endl;  
    }
  }

  void finish() {
    TransformerParent::finish();
    if(csvSyms) {
      sout.close();
    }
  }
  // Identify unique syms in a node
  //
  // Note that in merging, live defs are never alterred, only the
  // set of symbols present in each def.
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node) 
  {
    Statement& stmt = *I;
    if(stmt.dest.null()) return false;

    LiveDef* liveDef = stmt.get_info<LiveDef>();
    AaStmtSyms* syms = stmt.get_info<AaStmtSyms>();

    SH_DEBUG_ASSERT(liveDef);

    if(I == node->block->begin()) {
      symCount.init(liveDef->in, symTransfer->psyms);
    }

    symCount.inc(liveDef->def);
    symCount.dec(liveDef->dead);

    SH_DEBUG_PRINT_ASP(stmt);
    SH_DEBUG_PRINT_ASP("  " << *syms);
    SH_DEBUG_PRINT_ASP("  " << *liveDef);
    SH_DEBUG_PRINT_ASP("  " << symCount);

    if(csvSyms) {
      sout << stmt; 
      for(SymCount::iterator S = symCount.begin(); S != symCount.end(); ++S) {
        if(S->second == 0) continue;
        sout << "," << S->first << ":" << S->second;
      }
      sout << endl;
    }

    symComment(stmt, syms);
    if (dumpStats && AaspDebugOn) {
      SH_DEBUG_PRINT("aasyms," << stmt << "," << symCount.tocsv()); 
    }
    if (disableMerge) {
      return false;
    }

    // @todo actually implement merging

    // get unique symbol set in mergeDest
    AaIndexSet unique = symCount.unique(special);
    SH_DEBUG_PRINT_ASP("  Unique Set " << unique);

    // filter against each element in dest, and update unique/mergeReps
    // (if no mergeRep, pick one)
    bool change = false;
    for(int i = 0; i < stmt.dest.size(); ++i) {
      AaIndexSet iUnique = unique & syms->mergeDest[i]; 
      SH_DEBUG_PRINT_ASP("  i " << i << " iUnique " << iUnique);

      // at least 2 uniques remaining 
      if(iUnique.size() >= 2) { 
        change = true;
        syms->unique[i] |= iUnique; // %todo - is this correct (or should be |=) 
        if(syms->mergeRep[i].empty()) {
          // @todo may want to evaluate impact of using last here as a tweak?
          // (I don't think it matters, as both are unique, but there may be 
          // fragmentation issues leading to more swizzling?)
          syms->mergeRep[i] |= syms->unique[i].first();
        }
        SH_DEBUG_PRINT_ASP("  Merging " << syms->unique[i] << " representative " << syms->mergeRep[i] << " stmt " << stmt);
      } 

      // deal with case where there are no more unique --> remove mergeRep
      // @todo also case where mergeRep == unique 
      if(!syms->mergeRep[i].empty() && (syms->unique[i] & syms->dest[i]).empty()) { 
        change = true;
        SH_DEBUG_PRINT_ASP("  No more unique.  Removing representative " << syms->mergeRep[i] << " stmt " << stmt);
        syms->unique[i].clear(); // this is okay - why? because mergeDest = dest - unique
        syms->mergeRep[i].clear();
      }

      // Either (syms->unique[i] & syms->dest[i]) has at least one element and
      // syms->mergeRep has one element
      // Or else syms->mergeRep is empty
    }

    if(change) {
      // propagate changes (decreasing dataflow)
      symTransfer->add(&stmt);
      symTransfer->propagate(false);
      m_changed = true;
    }

    return false;
  }

};
typedef DefaultTransformer<UniqueMergerBase> UniqueMerger; 

/** adds unique merge information **/ 
void addUniqueMerge(ProgramNodePtr programNode, SymTransfer& st, bool dumpStats, bool disableMerge, const string& symfile) 
{
  if (!dumpStats && disableMerge) return;

  // Step 1: Compute per statement LiveDef information 

  LiveDefFinder ldf;
  ldf.transform(programNode);
  dump(programNode, "_asp-3.0-livedef");

  Reaper r;
  r.transform(programNode);

  // Step 2: Identify unique symbols, propagate merged symbols through
  //         the live definition range, and repeat until no further improvement
  AaProgramSyms* progSyms = programNode->get_info<AaProgramSyms>(); 
  SH_DEBUG_ASSERT(progSyms);

  SpecialFinder sf;
  sf.init(st.input_symbols);
  sf.transform(programNode);

  UniqueMerger umb;
  umb.init(&st, sf.special, dumpStats, disableMerge, symfile);
  while(umb.transform(programNode));
  dump(programNode, "_asp-3-uniq_merge");
}

/* Set inputs syms, and given completed AaStmtsyms info at each statement,
 * gathers syms required for each variable */
struct SymGatherBase: public TransformerParent 
{
  void init(AaProgramSyms* ps)
  {
    psyms = ps; 
    psyms->vars = psyms->inputs;
    psyms->outputs.clear();
  }

  /* Gather syms for vars */
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    Statement& stmt = *I;

    if(stmt.dest.null()) return false;

    ValueTracking* vt = stmt.get_info<ValueTracking>();

    const AaStmtSyms* stmtSyms = stmt.get_info<AaStmtSyms>();
    if(stmtSyms) {
      SH_DEBUG_PRINT_ASP("  gathering " << stmt);
      SH_DEBUG_PRINT_ASP("     syms= " << *stmtSyms);
    }
    // @todo range - want to create something even for vars without syms */
    if(stmtSyms /*&& !stmtSyms->mergeDest.empty()*/) {
      if(psyms->vars.find(stmt.dest.node()) == psyms->vars.end()) {
        psyms->vars[stmt.dest.node()] = AaSyms(stmt.dest.node()->size());
      }
      psyms->vars[stmt.dest.node()].mask_merge(stmt.dest.swizzle(), stmtSyms->mergeDest);

      // attach a comment
      ostringstream sout;
      sout << *stmtSyms; 
      stmt.destroy_info<InfoComment>();
      stmt.add_info(new InfoComment(sout.str()));

      // update outputs
      BindingType kind = stmt.dest.node()->kind();
      if(vt && (kind == SH_OUTPUT || kind == SH_INOUT) && 
          isAffine(stmt.dest.valueType())) { 
        for(int i = 0; i < stmt.dest.size(); ++i) {
          ValueTracking::DefUseChain::iterator I;
          for(I = vt->uses[i].begin(); I != vt->uses[i].end(); ++I) {
            if(I->kind == ValueTracking::Use::SH_OUTPUT) {
              AaSyms& outSyms = psyms->outputs[stmt.dest.node()];
              if(outSyms.empty()) {
                outSyms.resize(stmt.dest.size());
              }
              outSyms[stmt.dest.swizzle()[i]] |= stmtSyms->mergeDest[i];
            }
          }
        }
      }
    }
    return false;
  }

  // make sure all outputs have syms (in case there's an output never asn'd to)
  void finish() 
  {
    for(ProgramNode::VarList::iterator O = m_program->outputs.begin();
        O != m_program->outputs.end(); ++O) {
      if(!isAffine((*O)->valueType())) continue;
      if(psyms->outputs.find(*O) == psyms->outputs.end()) {
        psyms->outputs[*O].resize((*O)->size());
      }
    }
  }

  AaProgramSyms* psyms;
};
typedef DefaultTransformer<SymGatherBase> SymGather;

/* Removes all the Info information added by the various symbol placement
 * operations. */ 
struct SymRemoverBase: public TransformerParent {
// @todo make sure this removes everthing

  void handleNode(CtrlGraphNode* node) 
  {
    node->destroy_info<AdjStmt>();
    node->destroy_info<SymCount>();
  }

  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNode* node)
  {
    I->destroy_info<LiveDef>();
    I->destroy_info<AaStmtSyms>();
    return false;
  }

  void finish()
  {
    m_program->destroy_info<AaProgramSyms>();
  }
};
typedef DefaultTransformer<SymRemoverBase> SymRemover;


}

namespace SH {


AaStmtSyms::AaStmtSyms(int level, Statement* stmt)
  : level(level), 
    stmt(stmt), 
    unique(stmt->dest.size()),
    mergeRep(stmt->dest.size()),
    newdest(stmt->dest.size()),
    dest(stmt->dest.size()),
    mergeDest(stmt->dest.size()),
    src(opInfo[stmt->op].arity)
{
  for(int i = 0; i < opInfo[stmt->op].arity; ++i) {
    src[i].resize(stmt->src[i].size());
  }
}

Info* AaStmtSyms::clone() const {
  return new AaStmtSyms(*this);
}

bool AaStmtSyms::empty() const {
  if(!(unique.empty() && mergeRep.empty() && newdest.empty() && dest.empty() && mergeDest.empty())) {
      return false;
  }

  for(SymsVec::const_iterator S = src.begin(); S != src.end(); ++S) {
    if(!S->empty()) {
      return false;
    }
  }
  return true;
}

ostream& operator<<(ostream& out, const AaStmtSyms& stmtSyms)
{
  out << "<StmtSyms ";
  out << " level=" << stmtSyms.level;

  if(!stmtSyms.unique.empty()) {
    out << " unique=" << stmtSyms.unique;
  }

  if(!stmtSyms.unique.empty()) {
    out << " mergeRep=" << stmtSyms.mergeRep;
  }

  if(!stmtSyms.newdest.empty()) {
    out << " new=" << stmtSyms.newdest;
  }

  if(!(stmtSyms.dest - stmtSyms.mergeDest).empty()) {
    out << " dest=("; 
    for(int i = 0; i < stmtSyms.dest.size(); ++i) {
      out << stmtSyms.dest[i];
    }
    out << " )";
  }

  out << " mergeDest=("; 
  for(int i = 0; i < stmtSyms.dest.size(); ++i) {
    out << stmtSyms.mergeDest[i];
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
  out << " >";
  return out;
}

ostream& operator<<(ostream& out, const AaVarSymsMap& vsmap)
{
  out << "(( ";
  for(AaVarSymsMap::const_iterator I = vsmap.begin(); I != vsmap.end(); ++I) {
    out << " " << (I->first->name()) << "=" << I->second; 
  }
  out << "))";
  return out;
}

ostream& operator<<(ostream& out, const AaStmtSymsMap& ssmap)
{
  out << "(( ";
  for(AaStmtSymsMap::const_iterator I = ssmap.begin(); I != ssmap.end(); ++I) {
    out << " " << I->first.index() << "=" << I->second; 
  }
  out << "))";
  return out;
}

Info* AaProgramSyms::clone() const {
  return new AaProgramSyms(*this);
}

ostream& operator<<(ostream& out, const AaProgramSyms& progSyms)
{
  out << "AaProgramSyms { " 
      << " inputs=" << progSyms.inputs
      << " vars=" << progSyms.vars 
      << " outputs=" << progSyms.outputs
      << " }";
  return out;
}

void placeAaSyms(ProgramNodePtr programNode, bool showStats)
{
  AaVarSymsMap foo;
  placeAaSyms(programNode, foo, showStats);
}

void placeAaSyms(ProgramNodePtr programNode, const AaVarSymsMap& inputs, bool showStats)
{
  bool disableHier = !Context::current()->get_flag("aa_enable_hier"); 
  bool disableUniqMerge = !Context::current()->get_flag("aa_enable_um"); 
  AaspDebugOn = !Context::current()->get_flag("aa_disable_debug"); 
  SH_DEBUG_PRINT_ASP("placeAaSyms disableHier=" << disableHier << " disableUM=" << disableUniqMerge); 

  string name = programNode->name();
  Program program(programNode);

  ostringstream idout;
  idout << programNode->name() /*<< "_" << programNode.object()*/;
  if(disableHier) idout << "_nohier";
  if(disableUniqMerge) idout << "_noum";
  idout << "_";
  dumpPrefix = idout.str();

  string symfile = ""; 
  if(programNode->has_meta("aa_symfile")) symfile = programNode->meta("aa_symfile");
  else if(AaspDebugOn) { 
    symfile = dumpPrefix + "_syms"; 
  }

  SH_DEBUG_PRINT_ASP("Clearing any old syms");
  clearAaSyms(programNode);

  SH_DEBUG_PRINT_ASP("Fixing branches");
  fixRangeBranches(program);

  SH_DEBUG_PRINT_ASP("Adding Value Tracking, compute preds");
  add_value_tracking(program);

  SH_DEBUG_PRINT_ASP("Pulling intervals");
  IntervalPuller ip;
  ip.transform(programNode);

  SH_DEBUG_PRINT_ASP("Adding Value Tracking, compute preds");
  add_value_tracking(program);

  SH_DEBUG_PRINT_ASP("Hierarchical Initialization");  
  Structural programStruct(programNode->ctrlGraph);
  /*
  SectionTree sectionTree(programStruct);
  if(AaspDebugOn) {
    ostringstream sout;
    sectionTree.dump(sout);
    dotGen(sout.str(), dumpPrefix + "_asp-0-stree");
  }
  */

  if(!disableHier) {
    SH_DEBUG_PRINT_ASP("Hierarchical disabled. Skipping escape analysis");
    liveVarAnalysis(programStruct, program);
    dump(programNode, "_asp-1-inesc");
  }

  SH_DEBUG_PRINT_ASP("Adding Value Tracking");
  insert_branch_instructions(program);
  add_value_tracking(program);


  SH_DEBUG_PRINT_ASP("Filling in empty stmt indices");
  add_stmt_indices(program);

  // Place error symbols where they're needed,
  // and add defs to the worklist if they insert an extra error symbol
  SH_DEBUG_PRINT_ASP("Running error symbol initialization");
  SymTransfer st; 
  st.init(&inputs);
  st.transform(programNode);
  AaProgramSyms* psyms = st.psyms;

  // Propagate error symbols over control graph using current worklist
  SH_DEBUG_PRINT_ASP("Propagating syms");
  st.propagate(true);

  dump(programNode, "_asp-2-syms");

  addUniqueMerge(programNode, st, showStats, disableUniqMerge, symfile);

  // Gather syms for vars, outputs
  SymGather sg;
  sg.init(psyms);
  sg.transform(programNode);

  remove_branch_instructions(program);
}


void clearAaSyms(ProgramNodePtr programNode) 
{
  SymRemover sr;
  sr.transform(programNode);
}

}


