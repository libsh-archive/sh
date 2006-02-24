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
#include "ShSection.hpp"
#include "ShInclusion.hpp"
#include "ShAaHier.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_AAHIER
#endif
// #define SH_DBG_AAHIER

#ifdef SH_DBG_AAHIER
#define SH_DEBUG_PRINT_AH(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_AH(x) {}
#endif

namespace {
using namespace SH;

/** Maps a definition to the cfg node where it was defined,
 * or 0 if it's an input */
typedef std::map<ValueTracking::Def, ShCtrlGraphNodePtr> DefNodeMap; 

/** Stores the tuple elements that are inputs/escapees from a section */
typedef std::set<int> IndexSet;
typedef std::map<ShVariableNodePtr, IndexSet> VarIndexSet; 
typedef std::map<ShSectionNodePtr, VarIndexSet> SectionVarIndexSet; 

// really just a non-modifying traversal, not really a transformer...

/** Gathers the postorder index of the containing cfg node for each definition point */
struct DefNodeGathererBase: public ShTransformerParent 
{
  DefNodeMap* m_defNode;
  void init(DefNodeMap& defNode)
  {
    m_defNode = &defNode;
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

    for(int i = 0; i < stmt.dest.size(); ++i) {
      (*m_defNode)[ValueTracking::Def(&stmt, i)] = node;
    }
    return false;
  } 
};
typedef ShDefaultTransformer<DefNodeGathererBase> DefNodeGatherer;

/** Searches defuse info for defs that occur outside current section */
struct InEscFinderBase: public ShTransformerParent 
{
  ShSectionTree* m_sectionTree;
  DefNodeMap* m_defNode;
  SectionVarIndexSet m_in;
  SectionVarIndexSet m_esc;

  // @todo range - the number of things being passed in here is ridiculous...
  void init(ShSectionTree& sectionTree, DefNodeMap& defNode)
  {
    m_sectionTree = &sectionTree;
    m_defNode = &defNode;
  }

  // Adds the elm'th tuple element of var to the var index set 
  // for the given section and the section's ancestors until
  // either we reach program scope or the section contains the
  // given cfg node 
  //
  // @param def CfgNode where definition occurs. Set to 0 if we
  //            should add var/elm regardless of containment in section.
  void addVar(SectionVarIndexSet& svis, ShSectionNodePtr section, 
      ShCtrlGraphNodePtr defCfg, const ShVariable& var, int elm) {
    if(!section || section->isRoot()) return;

    if(!defCfg || !m_sectionTree->contains(section, defCfg)) { 
      SH_DEBUG_PRINT_AH("    adding to section = " << section->name()); 
      svis[section][var.node()].insert(var.swizzle()[elm]);
      addVar(svis, section->parent, defCfg, var, elm);
    } 
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStatement& stmt = *I;
    ValueTracking *vt = stmt.get_info<ValueTracking>();
    if(!vt) {
      if(!stmt.dest.null()) {
        SH_DEBUG_PRINT("No valuetracking on " << stmt);
      }
      return false;
    }
      

    ShSectionNodePtr section = (*m_sectionTree)[node];
    SH_DEBUG_ASSERT(section); 

    // Handle escaping outputs
    if(!stmt.dest.null()) {
      for(int i = 0; i < stmt.dest.size(); ++i) {
        for(ValueTracking::DefUseChain::iterator I = vt->uses[i].begin();
            I != vt->uses[i].end(); ++I) {
          if(I->kind == ValueTracking::Use::OUTPUT) {
            SH_DEBUG_PRINT_AH("  adding output dest " << stmt.dest.name() 
                << " to escapes section=" << section->name()); 
            addVar(m_esc, section, 0, stmt.dest, i);
            break;
          }
        }
      }
    }

    for(int i = 0; i < opInfo[stmt.op].arity; ++i) { 
      const ValueTracking::TupleUseDefChain& tud = vt->defs[i];

      for(unsigned int j = 0; j < tud.size(); ++j) { 
        const ValueTracking::UseDefChain& ud = tud[j];

        for(ValueTracking::UseDefChain::iterator U = ud.begin(); 
            U != ud.end(); ++U) {
          const ValueTracking::Def& def = *U;
          ShCtrlGraphNodePtr defCfg = (*m_defNode)[def];

          // if def is outside of this section, add to inputs to this section 
          SH_DEBUG_PRINT_AH("  checking src " << stmt.src[i].name()
            << "[" << j << "] deffed elsewhere for inputs section=" << section->name()); 
          addVar(m_in, section, defCfg, stmt.src[i], j);

          // if use is outside of the def's section, add to the def section's 
          // escaped variables
          if(defCfg) {
            ShSectionNodePtr defSection = (*m_sectionTree)[defCfg];
            SH_DEBUG_ASSERT(defSection);
            SH_DEBUG_PRINT_AH("  checking src " << stmt.src[i].name()
              << "[" << j << "] used elsewhere for escapes section=" << defSection->name()); 
              addVar(m_esc, defSection, node, stmt.src[i], j);
          }
        }
      }
    }
    return false;
  } 
};
typedef ShDefaultTransformer<InEscFinderBase> InEscFinder;

struct EscStmtInserterBase: public ShTransformerParent 
{
  ShSectionTree* m_sectionTree;
  SectionVarIndexSet* m_in;
  SectionVarIndexSet* m_esc;

  void init(ShSectionTree& sectionTree, InEscFinder& ief)  
  {
    m_sectionTree = &sectionTree;
    m_in = &ief.m_in;
    m_esc = &ief.m_esc;
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
    ShSectionNodePtr section = (*m_sectionTree)[node];
    switch(stmt.op) {
      case SH_OP_STARTSEC:
        {
          ShInEscInfo* inesc = new ShInEscInfo();
          stmt.destroy_info<ShInEscInfo>();
          stmt.add_info(inesc);
          SH_DEBUG_ASSERT(section);
          makeInfo(*inesc, (*m_in)[section], (*m_esc)[section]);
          SH_DEBUG_PRINT_AH("Adding InEsc info to section=" << section->name());
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
#if 0 // @todo range - add these in later
            ShVariable jOutput(J->node.clone(SH_OUTPUT, J->size()));
            ShStatement escSave = ShStatement(jOutput, SH_OP_ESCSAV, *J); 
            escStmts.push_back(escSave);
#endif
            if(!shIsAffine(J->valueType())) continue;

            ShStatement escJoin = ShStatement(*J, SH_OP_ESCJOIN, *J);
            ShStmtIndex* stmtIndex = stmt.get_info<ShStmtIndex>();
            if(stmtIndex) { 
              escJoin.add_info(stmtIndex->clone());
            }
            escStmts.push_back(escJoin);
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

};
typedef ShDefaultTransformer<EscStmtInserterBase> EscStmtInserter;

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

void inEscAnalysis(ShSectionTree& sectionTree, ShProgramNodePtr p)
{
  // insert value tracking
  p->ctrlGraph->computePredecessors();
  ShProgram program(p);
  insert_branch_instructions(program);
  add_value_tracking(program);
  remove_branch_instructions(program);

  // Make a def->index map
  DefNodeGatherer dng; 
  DefNodeMap defNode;
  dng.init(defNode);
  dng.transform(p);

  // Identify inputs, escapes 
  InEscFinder ief;
  ief.init(sectionTree, defNode);
  ief.transform(p);

  // Add special statements, and extra outputs.
  ShContext::current()->enter(p);
  EscStmtInserter esi;
  esi.init(sectionTree, ief);
  esi.transform(p);
  ShContext::current()->exit();
}

}
