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

#include <map>
#include "ShTransformer.hpp"
#include "ShOptimizations.hpp"
#include "ShSectionUtil.hpp"
#include "ShInclusion.hpp"
#include "ShAaHier.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_AAHIER
#endif
#define SH_DBG_AAHIER

#ifdef SH_DBG_AAHIER
#define SH_DEBUG_PRINT_AH(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_AH(x) {}
#endif

namespace {
using namespace SH;

typedef std::map<ShCtrlGraphNodePtr, IntRange> CfgRangeMap;
typedef std::map<ValueTracking::Def, int> DefIndexMap; 
typedef std::map<ShCtrlGraphNodePtr, ShStructuralNodePtr> CfgSectionMap; 

typedef std::set<int> IndexSet;
typedef std::map<ShVariableNodePtr, IndexSet> VarIndexSet; 
typedef std::map<ShStructuralNodePtr, VarIndexSet> SectionVarIndexSet; 

// really just a non-modifying traversal, not really a transformer...

/** Gathers the postorder index of the containing cfg node for each definition point */
struct DefIndexGathererBase: public ShTransformerParent 
{
  void init(CfgRangeMap& cfgRange, DefIndexMap& defIndex)
  {
    m_cfgRange = &cfgRange;
    m_defIndex = &defIndex;
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStatement& stmt = *I;
    if(stmt.dest.null()) return false; 

    ValueTracking *vt = stmt.get_info<ValueTracking>();
    if(!vt) {
      SH_DEBUG_PRINT_AH("Stmt does not have val-tracking: " << stmt);
      return false;
    }

    int index = (*m_cfgRange)[node].second;
    for(int i = 0; i < stmt.dest.size(); ++i) {
      (*m_defIndex)[ValueTracking::Def(&stmt, i)] = index; 
    }
    return false;
  } 

  CfgRangeMap* m_cfgRange;
  DefIndexMap* m_defIndex;
};
typedef ShDefaultTransformer<DefIndexGathererBase> DefIndexGatherer;

/** Searches defuse info for defs that occur outside current section */
struct InEscFinderBase: public ShTransformerParent 
{
  // @todo range - the number of things being passed in here is ridiculous...
  void init(StructRangeMap& sectionRange, CfgSectionMap& cfgSection, 
      CfgRangeMap& cfgRange, DefIndexMap& defIndex,
      SectionParentMap& sectionParent, 
      SectionVarIndexSet &ins, SectionVarIndexSet &escs)
  {
    m_sectionRange = &sectionRange;
    m_cfgSection = &cfgSection;
    m_cfgRange = &cfgRange;
    m_defIndex = &defIndex;
    m_sectionParent = &sectionParent;
    m_in = &ins;
    m_esc = &escs;

    // set up m_section map
    for(StructRangeMap::const_iterator I = sectionRange.begin(); 
        I != sectionRange.end(); ++I) {
      ShStructuralNodePtr node = I->first;
      int index = I->second.second;
      while(node && node->type != ShStructuralNode::SECTION) {
        node = node->container;
      }
      m_section[index] = node; 
    }
  }

  bool outside(int idx, IntRange& range) {
    return idx < range.first || range.second < idx;
  }

  std::string nameOf(ShStructuralNodePtr section)
  {
    if(!section) return "Main Program"; 

    SH_DEBUG_ASSERT(section->type == ShStructuralNode::SECTION 
        && !section->structnodes.empty());
    ShStatement* start = section->structnodes.front()->secStart;
    SH_DEBUG_ASSERT(start);
    ShInfoComment* comment = start->get_info<ShInfoComment>();
    SH_DEBUG_ASSERT(comment);
    return comment->comment;
  }

  // Adds the elm'th tuple element of var to the var index set 
  // for the given section and the section's ancestors until
  // either we reach program scope or the section contains the
  // given index
  //
  // @param idx postorder index of the  (-1 to ignore check) 
  void addVar(SectionVarIndexSet* svis, ShStructuralNodePtr section, 
      int idx, const ShVariable& var, int elm) {
    if(!section) {
      SH_DEBUG_PRINT_AH("    ignoring - in main program scope");
      return;
    }
    IntRange sectionRange = (*m_sectionRange)[section];

    if(idx == -1 || outside(idx, sectionRange)) {
      SH_DEBUG_PRINT_AH("    adding to section = " << nameOf(section));
      (*svis)[section][var.node()].insert(var.swizzle()[elm]);
      SH_DEBUG_PRINT_AH("    checking parent = " << (*m_sectionParent)[section].object());
      addVar(svis, (*m_sectionParent)[section], idx, var, elm);
    } else {
      SH_DEBUG_PRINT_AH("    ignoring - index is in section");
    }
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStatement& stmt = *I;
    ValueTracking *vt = stmt.get_info<ValueTracking>();

    IntRange nodeRange = (*m_cfgRange)[node];
    int nodeidx = nodeRange.second; 
    ShStructuralNodePtr nodeSection = m_section[nodeidx];

    if(!stmt.dest.null()) {
      ShBindingType kind = stmt.dest.node()->kind();
      // @todo range for here and below, need better def-use chains
      // that take into account input/output values
      // (or do an in/out conversion before coming through here) 
      if((kind == SH_OUTPUT || kind == SH_INOUT) && nodeSection) {
        SH_DEBUG_PRINT_AH("  adding output dest " << stmt.dest.name() 
            << " to escapes section=" << nameOf(nodeSection)); 
        for(int i = 0; i < stmt.dest.size(); ++i) {
          addVar(m_esc, nodeSection, -1, stmt.dest, i);
        }
      }
    }

    for(int i = 0; i < opInfo[stmt.op].arity; ++i) { 
      ShBindingType srcKind = stmt.src[i].node()->kind();
      if((srcKind == SH_INPUT || srcKind == SH_INOUT) && nodeSection) {
        SH_DEBUG_PRINT_AH("  adding input src " << stmt.src[i].name()
            << " to inputs section=" << nameOf(nodeSection));
        for(int j = 0; j < stmt.src[i].size(); ++j) {
          addVar(m_in, nodeSection, -1, stmt.src[i], j);
        }
      }

      if(!vt) continue;
      const ValueTracking::TupleUseDefChain& tud = vt->defs[i];
      for(unsigned int j = 0; j < tud.size(); ++j) { 
        const ValueTracking::UseDefChain& ud = tud[j];
        for(ValueTracking::UseDefChain::iterator U = ud.begin(); 
            U != ud.end(); ++U) {
          const ValueTracking::Def& def = *U;
          int defidx = (*m_defIndex)[def];

          // if def is outside of this section, add to inputs to this section 
          if((outside(defidx, nodeRange)) && nodeSection) { 
            SH_DEBUG_PRINT_AH("  checking src " << stmt.src[i].name()
              << "[" << j << "] deffed elsewhere for inputs section=" << nameOf(nodeSection));
            addVar(m_in, nodeSection, defidx, stmt.src[i], j);
          }

          // if use is outside of the def's section, add to the def section's 
          // escaped variables
          ShStructuralNodePtr defSection = m_section[defidx];
          if(defSection) {
            SH_DEBUG_PRINT_AH("  checking src " << stmt.src[i].name()
              << "[" << j << "] used elsewhere for escapes section=" << nameOf(defSection));
            addVar(m_esc, defSection, nodeidx, stmt.src[i], j);
          }
        }
      }
    }
    return false;
  } 

  StructRangeMap* m_sectionRange;
  CfgSectionMap* m_cfgSection;
  CfgRangeMap* m_cfgRange;
  DefIndexMap* m_defIndex;
  SectionParentMap* m_sectionParent;
  SectionVarIndexSet* m_in;
  SectionVarIndexSet* m_esc;

  typedef std::map<int, ShStructuralNodePtr> IndexSectionMap;
  IndexSectionMap m_section; ///< maps a postorder index to the enclosing section node (or 0 if in program scope)
};
typedef ShDefaultTransformer<InEscFinderBase> InEscFinder;

struct EscStmtInserterBase: public ShTransformerParent 
{
  void init(CfgSectionMap& cfgSection, SectionVarIndexSet& in, SectionVarIndexSet& esc)
  {
    m_cfgSection = &cfgSection;
    m_in = &in;
    m_esc = &esc;
  }

  ShVariable makeVar(ShVariableNodePtr node, const IndexSet& indices) {
    int* swizidx = new int[indices.size()];

    int i = 0;
    for(IndexSet::const_iterator I = indices.begin();
        I != indices.end(); ++I, ++i) {
      swizidx[i] = *I;
    }
    ShVariable result(node, ShSwizzle(node->size(), indices.size(), swizidx), false);
    delete[] swizidx;
    return result;
  }

  void makeInfo(ShInEscInfo& info, VarIndexSet& in, VarIndexSet& esc)
  {
    VarIndexSet::const_iterator V;
    for(V = in.begin(); V != in.end(); ++V) {
      info.in.push_back(makeVar(V->first, V->second));
    }

    for(V = esc.begin(); V != esc.end(); ++V) {
      info.esc.push_back(makeVar(V->first, V->second));
    }
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStatement& stmt = *I;
    ShStructuralNodePtr section = (*m_cfgSection)[node];
    switch(stmt.op) {
      case SH_OP_STARTSEC:
        {
          ShInEscInfo* inesc = new ShInEscInfo();
          stmt.destroy_info<ShInEscInfo>();
          stmt.add_info(inesc);
          SH_DEBUG_ASSERT(section);
          makeInfo(*inesc, (*m_in)[section], (*m_esc)[section]);
          SH_DEBUG_PRINT_AH("Adding InEsc info to STARTSEC section=" << section.object());
          SH_DEBUG_PRINT_AH("    " << *inesc);
          break;
        }
      case SH_OP_ENDSEC:
        {
          ShInEscInfo* inesc = new ShInEscInfo();
          stmt.destroy_info<ShInEscInfo>();
          stmt.add_info(inesc);
          SH_DEBUG_ASSERT(section);
          makeInfo(*inesc, (*m_in)[section], (*m_esc)[section]);

          ShBasicBlock::ShStmtList escStmts; 
          for(ShInEscInfo::VarVec::iterator J = inesc->esc.begin();
              J != inesc->esc.end(); ++J) {

            ShStatement escj = ShStatement(*J, SH_OP_ESCJOIN, *J);
            ShStmtIndex* stmtIndex = stmt.get_info<ShStmtIndex>();
            if(stmtIndex) { 
              escj.add_info(stmtIndex->clone());
            }
            escStmts.push_back(escj);
          }
          node->block->splice(I, escStmts);
          break;
        }
      default:
        break;
    }
    return false;

    return false;
  }

  CfgSectionMap* m_cfgSection;
  SectionVarIndexSet* m_in;
  SectionVarIndexSet* m_esc;
};
typedef ShDefaultTransformer<EscStmtInserterBase> EscStmtInserter;

struct SectionInfoAdder {
  SectionInfoAdder(ShStructural &s)
    : m_cfgLevel(sectionNestingLevel(s)),
      m_cfgSection(gatherCfgSection(s)) 
  {}

  void operator()(ShCtrlGraphNodePtr node) 
  {
    SH_DEBUG_ASSERT(m_cfgLevel.find(node) != m_cfgLevel.end());

    ShSectionInfo* secInfo;
    unsigned int level = m_cfgLevel[node];
    if(level == 0) {
      secInfo = new ShSectionInfo();
    } else {
      SH_DEBUG_ASSERT(m_cfgSection[node]);
      ShStructuralNodePtr section = m_cfgSection[node];
      ShStatement *start = section->structnodes.front()->secStart;
      ShStatement *end = section->structnodes.back()->secEnd;
      SH_DEBUG_ASSERT(start && end);
      secInfo = new ShSectionInfo(start, end, level); 
    }
    node->destroy_info<ShSectionInfo>();
    node->add_info(secInfo);
  }

  NestingLevelMap m_cfgLevel; 
  CfgSectionMap m_cfgSection; 
};

}

namespace SH {

ShInfo* ShInEscInfo::clone() const
{
  return new ShInEscInfo(*this);
}

std::ostream& operator<<(std::ostream& out, const ShInEscInfo::VarVec& vv)
{
  for(ShInEscInfo::VarVec::const_iterator I = vv.begin();
      I != vv.end(); ++I) {
    if(I != vv.begin()) out << ",";
    out << I->name() << I->swizzle(); 
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const ShInEscInfo& iei)
{
  out << "<ins=" << iei.in << " | esc=" << iei.esc << ">";
  return out;
}

void inEscAnalysis(ShStructural &structural, ShProgramNodePtr p)
{
  StructRangeMap range;
  range = postorderRange(structural);

  CfgSectionMap cfgSection = gatherCfgSection(structural);

  // also map cfg nodes to indices
  CfgRangeMap cfgRange;
  for(StructRangeMap::const_iterator R = range.begin(); R != range.end(); ++R) {
    const ShStructuralNodePtr& snode = R->first;
    const IntRange& range = R->second;
    if(snode->cfg_node) {
      cfgRange[snode->cfg_node] = range;
    }
  }

  SectionParentMap sectionParent = findParents(structural);

  // insert value tracking
  p->ctrlGraph->computePredecessors();
  ShProgram program(p);
  insert_branch_instructions(program);
  add_value_tracking(program);
  remove_branch_instructions(program);

  // Make a def->index map
  DefIndexGatherer dig; 
  DefIndexMap defIndex;
  dig.init(cfgRange, defIndex);
  dig.transform(p);

  // Identify inputs, escapes 
  InEscFinder ief;
  SectionVarIndexSet ins, escs;
  ief.init(range, cfgSection, cfgRange, defIndex, sectionParent, ins, escs);
  ief.transform(p);

  // Add special statements
  EscStmtInserter esi;
  esi.init(cfgSection, ins, escs);
  esi.transform(p);
}

ShSectionInfo::ShSectionInfo()
  : level(0),
    start(0),
    end(0)
{}

ShSectionInfo::ShSectionInfo(ShStatement* start, ShStatement* end, int level)
  : level(level),
    start(start),
    end(end)
{}

ShInfo* ShSectionInfo::clone() const
{
  return new ShSectionInfo(*this);
}

void addSectionInfo(ShStructural &s, ShProgramNodePtr p)
{
  SectionInfoAdder sia(s);
  p->ctrlGraph->dfs(sia);
}


}
