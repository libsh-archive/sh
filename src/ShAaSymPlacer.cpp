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
#include "ShDebug.hpp"
#include "ShTransformer.hpp"
#include "ShStructural.hpp"
#include "ShSection.hpp"
#include "ShOptimizations.hpp"
#include "ShInclusion.hpp"
#include "ShAaHier.hpp"
#include "ShAaSymPlacer.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_AASYMPLACER
#endif
//#define SH_DBG_AASYMPLACER

#ifdef SH_DBG_AASYMPLACER
#define SH_DEBUG_PRINT_ASP(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASP(x) {}
#endif

// @todo range
// - check if m_changed is always being set correctly 

namespace 
{
using namespace SH;

typedef std::queue<ValueTracking::Def> WorkList;
typedef std::list<ShCtrlGraphNodePtr> CfgWorkList; // @todo really should be something other than a basic queue (to reduce duplicate CFG nodes)
typedef std::list<ShStatement*> StmtWorkList; // @todo really should be something other than a basic queue (to reduce duplicate CFG nodes)
typedef std::vector<ShAaIndexSet> SymLevelVec; ///< used to hold symbols at each nesting level

/* Given a Sequence s and Container c, inserts elements of c into s */ 
template<class S, class C>
void end_insert(S& s, C& c) {
  s.insert(s.end(), c.begin(), c.end());
}

/* Given an Associative Container a and Container c, inserts elements of c into a */
template<class A, class C>
void insert(A& a, C& c) {
  a.insert(c.begin(), c.end());
}

bool isEmpty(ShCtrlGraphNodePtr node) {
  return !node->block || node->block->empty();
}

/* Adds stmt syms as a comment to a statement if in debug mode*/
void symComment(ShStatement& stmt, ShAaStmtSyms* syms) {
#ifdef SH_DBG_AASYMPLACER
    if(syms->empty()) return;
    std::ostringstream sout;
    sout << *syms; 
    stmt.destroy_info<ShInfoComment>();
    stmt.add_info(new ShInfoComment(sout.str()));
#endif
}


/* Places symbols for inputs, intervals used as sources, and statement
 * destinations when a statement is non-affine.
 */
// @todo make this more efficient
// (either only make the changes necessary per component, or use per-statement
// worklists instead of per-def)
struct SymTransferBase: public ShTransformerParent 
{
  SymTransferBase()
    : psyms(new ShAaProgramSyms()), cur_index(0) 
  {}

  /* Handle predefined error symbols.  Start new error symbols from one more
   * than the maximum predefined error symbol 
   * Assigns the worklist to fill with w
   *
   * This must be called exactly once before each transform call */
  void init(ShSectionTree* s, const ShAaVarSymsMap* inputs) 
  {
    sectionTree = s; 

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

  void insertSyms(int level, const ShAaSyms& syms) 
  {
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
    SH_DEBUG_ASSERT(vt);

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
    // not necessar - handled in updateDest now
    //  stmtSyms->dest = stmtSyms->newdest;

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
    updateDest(stmtSyms, levelSyms, true);
    return false;
  }


  void finish()
  {
    m_program->destroy_info<ShAaProgramSyms>();
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
      ShAaStmtSyms* defStmtSyms = def.stmt->get_info<ShAaStmtSyms>();

      SH_DEBUG_ASSERT(vt);
      SH_DEBUG_ASSERT(defStmtSyms);

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
        ShAaIndexSet& useIndexSet = useStmtSyms->src[use->source][use->index];
        if(increasing) {
          useIndexSet |= defStmtSyms->mergeDest[def.index];
        } else {
          ValueTracking *useVt = use->stmt->get_info<ValueTracking>();
          SH_DEBUG_ASSERT(useVt);

          useIndexSet.clear();
          for(ValueTracking::UseDefChain::iterator D = useVt->defs[use->source][use->index].begin();
              D != useVt->defs[use->source][use->index].end(); ++D) {
            ShAaIndexSet* defIndexSet = 0;
            switch(D->kind) {
              case ValueTracking::Def::INPUT: {
                ShVariable src = use->stmt->src[use->source];
                defIndexSet = &psyms->inputs[src.node()][src.swizzle()[use->index]];
                break;
              }
              case ValueTracking::Def::STMT: {
                ShAaStmtSyms* DSyms = D->stmt->get_info<ShAaStmtSyms>();
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
  void add(ShStatement* stmt) {
    /*
    for(int i = 0; i < stmt->dest.size(); ++i) {
      worklist.push(ValueTracking::Def(stmt, i));
    }
    */
    ShAaStmtSyms* stmtSyms = stmt->get_info<ShAaStmtSyms>();
    SH_DEBUG_ASSERT(stmtSyms);
    updateDest(stmtSyms, levelSyms);
  }

  ShSectionTree* sectionTree;
  ShAaProgramSyms* psyms;
  int cur_index;

  // levelSyms[i] represents the symbols inserted at nesting level i 
  SymLevelVec levelSyms;

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
    void updateDest(ShAaStmtSyms *stmtSyms, const SymLevelVec& levelSyms, bool addAlways=false)
    {
      ShStatement* stmt = stmtSyms->stmt;
      ShAaSyms &dest = stmtSyms->dest;
      ShAaSyms &mergeDest = stmtSyms->mergeDest;
      ShAaStmtSyms::SymsVec &src = stmtSyms->src;
      int size = mergeDest.size();

      dest = stmtSyms->newdest;
      if(stmt->op == SH_OP_ERRFROM) {
        SH_DEBUG_PRINT_ASP("    ERRFROM");
        ShAaIndexSet all; 

        for(int i = 0; i < src[1].size(); ++i) {
          all |= src[1][i];
        }

        SH_DEBUG_ASSERT(size == src[0].size());
        for(int i = 0; i < size; ++i) {
          dest.merge(i, src[0][i] & all);
        }
#if 0
      // @todo range - result of LASTERR is non-affine
      } else if(stmt->op == SH_OP_LASTERR) {
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
      } else if(stmt->op == SH_OP_ESCJOIN) {
        SH_DEBUG_PRINT_ASP("    ESCJOIN");
        SH_DEBUG_ASSERT(size == src[0].size());
        unsigned int myLevel = stmtSyms->level;
        if(myLevel < levelSyms.size()) {
          for(int i = 0; i < size; ++i) {
            dest.merge(i, src[0][i] - levelSyms[myLevel]);
          }
        }
      } else if(stmt->op == SH_OP_ESCSAV) {
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
              for(int i = 0; i < size; ++i) {
                for(int j = 0; j < opInfo[stmt->op].arity; ++j) {
                  bool scalarSrc = stmt->src[j].size() == 1;
                  dest.merge(i, src[j][scalarSrc ? 0 : i]);
                }
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
      if(!shIsAffine(stmt->dest.valueType())) {
        symComment(*stmt, stmtSyms);
        return;
      }

      ShAaSyms tempMergeDest = (dest - stmtSyms->unique);
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
typedef ShDefaultTransformer<SymTransferBase> SymTransfer;




/* Structure used in unique merging to find live def ranges. 
 * A LiveDef represents the set of definitions that may be live 
 * upon entering and exiting a statement (it's a per-statement ShInfo).
 *
 * (Based on typical live-variable analysis (see apel, aho sethi ullman),
 * use = any def used in a stmt src
 * def = any def resulting from this statement 
 *
 * dead = (def union in) - out 
 * (may also include def 
 * 
 * pred/succ are statements that come before after this statement 
 *    (0 may be used in the pred list if statement can be first 
 *     succ list if it can be last)
 *
 *
 *  
 */
typedef std::set<ValueTracking::Def> DefSet;
typedef std::set<ShStatement*> StmtSet;  
struct LiveDef: public ShInfo 
{
  DefSet in, use, def, out;
  DefSet dead; 

  // @todo should be computing these from CFG, but don't  
  StmtSet pred, succ;

  ShInfo* clone() const 
  {
    return new LiveDef(*this);
  }

  friend std::ostream& operator<<(std::ostream& out, const LiveDef& ld);
};

std::ostream& operator<<(std::ostream& out, const DefSet& ds) {
  int lastIndex = -1;
  for(DefSet::const_iterator D = ds.begin(); D != ds.end(); ++D) {
    ShStmtIndex* stmtIdx = D->stmt->get_info<ShStmtIndex>();
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

std::ostream& operator<<(std::ostream& out, const StmtSet& ss) {
  for(StmtSet::const_iterator S = ss.begin(); S != ss.end(); ++S) {
    if(S != ss.begin()) {
      out << ", ";
    }
    if((*S) == 0) {
      out << "NULL"; 
      continue;
    }
    ShStmtIndex* idx = (*S)->get_info<ShStmtIndex>();
    SH_DEBUG_ASSERT(idx);
    out << "#" << idx->index();
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const LiveDef& ld) {
  out << "<LiveDef in: " << ld.in << " use: " << ld.use << " def:" << ld.def
      << " out:" << ld.out << " dead:" << ld.dead 
      << " pred: " << ld.pred << " succ:" << ld.succ << ">";
  return out;
}

/* Statements that may come before or after a CFG node */ 
struct AdjStmt: public ShInfo {
  StmtSet pred, succ;

  ShInfo* clone() const 
  {
    return new AdjStmt(*this);
  }
};

/* Structure used to compute pred/succ in statements
 * This does it per CFG node by propagating across empty nodes 
 */
struct StmtClosureBase: ShTransformerParent {
  CfgWorkList worklist;

  void start(ShProgramNodePtr program) {
    ShTransformerParent::start(program);

    program->ctrlGraph->computePredecessors();
  }

  // @todo think about and simplify this...
  void handleNode(ShCtrlGraphNodePtr node) {
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

    ShCtrlGraphNode::ShPredList::const_iterator P = node->predecessors.begin();
    for(;P != node->predecessors.end(); ++P) {
      if(!isEmpty(*P)) {
        adj->pred.insert(&*(*P)->block->rbegin());
        SH_DEBUG_PRINT_ASP("  Pred " << *(*P)->block->rbegin());
      }
    }

    ShCtrlGraphNode::SuccessorList::const_iterator S = node->successors.begin();
    for(; S != node->successors.end(); ++S) {
      if(!isEmpty(S->node)) {
        adj->succ.insert(&*(S->node)->block->begin());
        SH_DEBUG_PRINT_ASP("  Succ " << *(S->node)->block->begin());
      }
    }
    if(node->follower && !isEmpty(node->follower)) {
      adj->succ.insert(&*node->follower->block->begin());
      SH_DEBUG_PRINT_ASP("  Succ " << *node->follower->block->begin());
    }
  }

  AdjStmt* getAdj(ShCtrlGraphNodePtr node) {
    AdjStmt* result = node->get_info<AdjStmt>();
    if(!result) {
      SH_DEBUG_PRINT_ASP("Null node adjstmt!");
    }
    SH_DEBUG_ASSERT(result);
    return result;
  }

  void finish() {
    while(!worklist.empty()) {
      ShCtrlGraphNodePtr node = worklist.front(); worklist.pop_front();
      AdjStmt* adj = getAdj(node); 

      size_t predSize = adj->pred.size();
      size_t succSize = adj->succ.size();

      ShCtrlGraphNode::ShPredList::const_iterator P = node->predecessors.begin();
      for(;P != node->predecessors.end(); ++P) {
        if(isEmpty(*P)) {
          insert(adj->pred, getAdj(*P)->pred);
        }
      }

      ShCtrlGraphNode::SuccessorList::const_iterator S = node->successors.begin();
      for(; S != node->successors.end(); ++S) {
        if(isEmpty(S->node)) {
          insert(adj->succ, getAdj(S->node)->succ);
        }
      }
      if(node->follower && isEmpty(node->follower)) {
        insert(adj->succ, getAdj(node->follower)->succ);
      }

      if(isEmpty(node)) {
        if(succSize != adj->succ.size()) {
          end_insert(worklist, node->predecessors);
        }

        if(predSize != adj->pred.size()) {
          for(ShCtrlGraphNode::SuccessorList::iterator S = node->successors.begin();
              S != node->successors.end(); ++S) {
            worklist.push_back(S->node);
          }
          if(node->follower) {
            worklist.push_back(node->follower);
          }
        }
      }
    }
  }
};
typedef ShDefaultTransformer<StmtClosureBase> StmtClosure;


// Initializes use, def sets and pred, succ sets 
struct LiveDefFinderBase: ShTransformerParent {
  StmtWorkList worklist;

  void start(ShProgramNodePtr program) {
    ShTransformerParent::start(program);

    StmtClosure sc;
    sc.transform(program);
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStatement& stmt = *I;

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
            if(L->kind == ValueTracking::Def::STMT) {
              insert(liveDef->use, *K);
            }
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
      ShBasicBlock::ShStmtList::iterator J = I;
      --J;
      liveDef->pred.insert(&*J);
    }

    if(&*I == &*node->block->rbegin()) {
      insert(liveDef->succ, nodeAdj->succ); 
    } else {
      ShBasicBlock::ShStmtList::iterator J = I;
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

    ShCtrlGraphNodePtr exitNode = m_program->ctrlGraph->exit(); 
    if(!isEmpty(exitNode)) {
      ShStatement& last = *exitNode->block->rbegin();
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
      ShStatement* stmt = worklist.front(); worklist.pop_front();
      if(!stmt) continue; // ignore pred starts

      LiveDef* ld = stmt->get_info<LiveDef>();

      // update out(N) = union in(S), S is in succ(N)
      StmtSet::iterator S = ld->succ.begin();
      for(; S != ld->succ.end(); ++S) {
        if(!*S) { // don't need to consider exit...it's alread considered
          continue;
        }
        LiveDef* Sld = (*S)->get_info<LiveDef>();
        insert(ld->out, Sld->in);
      }

      size_t inCount = ld->in.size();

      // update in(N) = use(N) union (out(N) - def(N))
      DefSet outLessDef;
      std::insert_iterator<DefSet> I(outLessDef, outLessDef.begin()); 
      set_difference(ld->out.begin(), ld->out.end(), ld->def.begin(), ld->def.end(), I);

      insert(ld->in, ld->use);
      insert(ld->in, outLessDef);
      if(ld->in.size() > inCount) { // add preds to worklist
        end_insert(worklist, ld->pred); 
      }

      SH_DEBUG_PRINT_ASP("LiveDef - Prop - " << *stmt << " = " << *ld << ", out - def = " << outLessDef);
    }
  }
};
typedef ShDefaultTransformer<LiveDefFinderBase> LiveDefFinder;

/* Collects LiveDef->dead information 
 * dead = (in union def) - out = (in - out) union (def - out) */

struct ReaperBase: ShTransformerParent {
  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) {
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false;

    LiveDef* liveDef = stmt.get_info<LiveDef>();

    liveDef->dead.clear();
    std::insert_iterator<DefSet> D(liveDef->dead, liveDef->dead.begin());

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
typedef ShDefaultTransformer<ReaperBase> Reaper;



/* Structure for unique merging that holds noise symbol counts for each cfg node 
 * (The second dataflow step) */
struct SymCount
{
  // count[i] = number of times noise symbol
  typedef std::map<int, int> SymCountMap;
  typedef SymCountMap::iterator iterator;
  typedef SymCountMap::const_iterator const_iterator;
  SymCountMap count; 

  // per tuple element counts
  typedef std::pair<ShVariableNodePtr, int> Element;  
  typedef std::map<Element, SymCountMap> VarCountMap;
  VarCountMap varCount;
  

  // Re-initializes count from a set of live definitions 
  void init(const DefSet& defs) 
  {
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

  ShAaIndexSet unique() const {
    ShAaIndexSet result;
    for(SymCountMap::const_iterator I = count.begin(); I != count.end(); ++I) {
      if(I->second == 1) {
        result |= I->first;
      }
    }
    return result;
  }

  iterator begin() { return count.begin(); }
  iterator end() { return count.end(); }

  const_iterator begin() const { return count.begin(); }
  const_iterator end() const { return count.end(); }

  private:
    template<int DELTA>
    void update(const ValueTracking::Def& def) 
    {
      ShStatement& stmt = *def.stmt;
      SH_DEBUG_ASSERT(def.kind == ValueTracking::Def::STMT);
      int index = def.index;

      ShVariableNodePtr defNode = stmt.dest.node();
      int rawIndex = stmt.dest.swizzle()[index]; 
      SymCountMap& defVarCount = varCount[Element(defNode, rawIndex)];

      ShAaStmtSyms* stmtSyms = stmt.get_info<ShAaStmtSyms>();
      SH_DEBUG_ASSERT(stmtSyms);
      ShAaIndexSet& indexSet = stmtSyms->mergeDest[index];

      ShAaIndexSet::const_iterator I = indexSet.begin();
      for(;I != indexSet.end(); ++I) {
        int oldCount = defVarCount[*I];
        int newCount = (defVarCount[*I] += DELTA);
        int& counti = count[*I];

        if(oldCount == 0 && newCount > 0) {
          counti++;
        } else if (oldCount > 0 && newCount == 0) {
          counti--;
        }
        if(newCount < 0 || counti < 0) {
          SH_DEBUG_PRINT("*I " << *I << " oldCount" << oldCount << " newCount " << 
              newCount << " counti " << counti);
        }
        SH_DEBUG_ASSERT(newCount >= 0 && counti >= 0);
      }
    }

  friend std::ostream& operator<<(std::ostream& out, const SymCount& sc);
};

std::ostream& operator<<(std::ostream& out, const SymCount& sc) {
  // @todo 
  out << "<SymCount count=";
  for(SymCount::const_iterator I = sc.begin(); I != sc.end(); ++I) {
    out << I->first << ":" << I->second << " ";
  }
  
  out << "unique=" << sc.unique() << ">"; 
  return out;
}

// This works accelerated on a per-node basis
// (only  
//
// At a node, the sym counts are initialized based on the first statement.
// Then the iteration goes through each statement
//  -increments counts for definitions
//  -decrements counts for deaths
//
//
struct UniqueMergerBase: ShTransformerParent 
{
  SymCount symCount;
  SymTransfer* symTransfer;

  void init(SymTransfer* st) {
    symTransfer = st;
  }

  // Identify unique syms in a node
  //
  // Note that in merging, live defs are never alterred, only the
  // set of symbols present in each def.
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node) 
  {
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false;

    LiveDef* liveDef = stmt.get_info<LiveDef>();
    ShAaStmtSyms* syms = stmt.get_info<ShAaStmtSyms>();

    SH_DEBUG_ASSERT(liveDef);

    if(I == node->block->begin()) {
      symCount.init(liveDef->in);
    }

    symCount.inc(liveDef->def);
    symCount.dec(liveDef->dead);

    SH_DEBUG_PRINT_ASP(stmt);
    SH_DEBUG_PRINT_ASP("  " << *syms);
    SH_DEBUG_PRINT_ASP("  " << *liveDef);
    SH_DEBUG_PRINT_ASP("  " << symCount);

    symComment(stmt, syms);

    // @todo actually implement merging

    // get unique symbol set in mergeDest
    ShAaIndexSet unique = symCount.unique();
    SH_DEBUG_PRINT_ASP("  Unique Set " << unique);

    // filter against each element in dest, and update unique/mergeReps
    // (if no mergeRep, pick one)
    bool change = false;
    for(int i = 0; i < stmt.dest.size(); ++i) {
      ShAaIndexSet iUnique = unique & syms->mergeDest[i]; 
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
typedef ShDefaultTransformer<UniqueMergerBase> UniqueMerger; 

/** adds unique merge information **/ 
void addUniqueMerge(ShProgramNodePtr programNode, SymTransfer& st) 
{
  // Step 1: Compute per statement LiveDef information 

  LiveDefFinder ldf;
  ldf.transform(programNode);

  Reaper r;
  r.transform(programNode);

  // Step 2: Identify unique symbols, propagate merged symbols through
  //         the live definition range, and repeat until no further improvement
  ShAaProgramSyms* progSyms = programNode->get_info<ShAaProgramSyms>(); 
  SH_DEBUG_ASSERT(progSyms);

  UniqueMerger umb;
  umb.init(&st);
  while(umb.transform(programNode));
#ifdef SH_DBG_AASYMPLACER
  programNode->dump(programNode->name() + "_asp-3-uniq_merge");
#endif
}

/* Set inputs syms, and given completed ShAaStmtsyms info at each statement,
 * gathers syms required for each variable */
struct SymGatherBase: public ShTransformerParent 
{
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
    if(stmtSyms && !stmtSyms->mergeDest.empty()) {
      if(psyms->vars.find(stmt.dest.node()) == psyms->vars.end()) {
        psyms->vars[stmt.dest.node()] = ShAaSyms(stmt.dest.node()->size());
      }
      psyms->vars[stmt.dest.node()].mask_merge(stmt.dest.swizzle(), stmtSyms->mergeDest);

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

/* Removes all the ShInfo information added by the various symbol placement
 * operations. */ 
struct SymRemoverBase: public ShTransformerParent {
// @todo make sure this removes everthing

  void handleNode(ShCtrlGraphNodePtr node) 
  {
    node->destroy_info<AdjStmt>();
    node->destroy_info<SymCount>();
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    I->destroy_info<LiveDef>();
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

ShInfo* ShAaStmtSyms::clone() const {
  return new ShAaStmtSyms(*this);
}

bool ShAaStmtSyms::empty() const {
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

std::ostream& operator<<(std::ostream& out, const ShAaStmtSyms& stmtSyms)
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
  bool disableHier = programNode->meta("aa_disable_hier") == "true";
  bool disableUniqMerge = programNode->meta("aa_disable_uniqmerge") == "true";

  std::string name = programNode->name();
  ShProgram program(programNode);

  SH_DEBUG_PRINT_ASP("Clearing any old syms");
  clearAaSyms(programNode);

  SH_DEBUG_PRINT_ASP("Adding Value Tracking, compute preds");
  programNode->ctrlGraph->computePredecessors();
  add_value_tracking(program);


  SH_DEBUG_PRINT_ASP("Hierarchical Initialization");  
  ShStructural programStruct(programNode->ctrlGraph);
  ShSectionTree sectionTree(programStruct);

#ifdef SH_DBG_AASYMPLACER 
  std::ostringstream sout;
  sectionTree.dump(sout);
  shDotGen(sout.str(), name + "_asp-0-stree");
#endif

  if(!disableHier) {
    SH_DEBUG_PRINT_ASP("Hierarchical disabled. Skipping escape analysis");
    inEscAnalysis(sectionTree, programNode);
#ifdef SH_DBG_AASYMPLACER 
  programNode->dump(name + "_asp-1-inesc");
#endif
  }

  SH_DEBUG_PRINT_ASP("Adding Value Tracking");
  programNode->ctrlGraph->computePredecessors();
  insert_branch_instructions(program);
  add_value_tracking(program);
  remove_branch_instructions(program);


  SH_DEBUG_PRINT_ASP("Filling in empty stmt indices");
  add_stmt_indices(program);

  // Place error symbols where they're needed,
  // and add defs to the worklist if they insert an extra error symbol
  SH_DEBUG_PRINT_ASP("Running error symbol initialization");
  SymTransfer st; 
  st.init(&sectionTree, &inputs);
  st.transform(programNode);
  ShAaProgramSyms* psyms = st.psyms;

  // Propagate error symbols over control graph using current worklist
  SH_DEBUG_PRINT_ASP("Propagating syms");
  st.propagate(true);

#ifdef SH_DBG_AASYMPLACER 
  programNode->dump(name + "_asp-2-syms");
#endif

  if(disableUniqMerge) {
    SH_DEBUG_PRINT_ASP("Unique Merging disabled.");
  } else {
    SH_DEBUG_PRINT_ASP("Computing unique merge information");
    addUniqueMerge(programNode, st);
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


