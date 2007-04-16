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
#include <cassert>
//#include "RecStream.hpp"
#include "Stream.hpp"
#include "Inclusion.hpp"
#include "Transformer.hpp"
#include "AaSymPlacer.hpp"
#include "AaOpHandler.hpp"
#include "AaScanCore.hpp"

#ifdef DBG_AA
#define DBG_ASCCANCORE
#endif
#define DBG_ASCCANCORE

#ifdef DBG_ASCCANCORE
#define SH_DEBUG_PRINT_ASC(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASC(x) {}
#endif

namespace {
using namespace SH;

struct StmtIndexGathererBase: public TransformerParent {
  void init(AaScanCore::IndexStmtMap* idxStmt) {
    m_idxStmt = idxStmt;
  }

  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) 
  { 
    StmtIndex* idx = I->get_info<StmtIndex>();
    if(idx) {
      (*m_idxStmt)[*idx] = &*I;
    }
    return false;
  }
  AaScanCore::IndexStmtMap* m_idxStmt;
};
typedef DefaultTransformer<StmtIndexGathererBase> StmtIndexGatherer;

Variable makeTemp(VariableNodePtr node, const std::string& name) {
  Variable result(node->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false)); 
  result.name(name);
  return result;
}

}
namespace SH {

AaScanCore::AaScanCore(Program program)
  : m_program(program)
{
#ifdef DBG_ASCCANCORE
  m_program.node()->dump("aascan_start");
#endif
  m_aa_program = affine_inclusion_syms(m_program);
#ifdef DBG_ASCCANCORE
  m_aa_program.node()->dump("aascan_aa");
#endif
  placeAaSyms(m_aa_program.node(), true);

  m_psyms = m_aa_program.node()->get_info<AaProgramSyms>();
  SH_DEBUG_ASSERT(m_psyms);
  SH_DEBUG_PRINT_ASC("Program Syms");
  SH_DEBUG_PRINT_ASC(*m_psyms);

  m_aa_opped = Program(m_aa_program.node()->clone());
  handleAaOps(m_aa_opped.node());
  
#ifdef DBG_ASCCANCORE
  m_aa_opped.node()->dump("aascan_op");
#endif

  // set up input map 
  ProgramNode::VarList::const_iterator P, AAP;
  P = m_program.begin_inputs();
  AAP = m_aa_program.begin_inputs();
  for(; P != m_program.end_inputs(); ++P, ++AAP) {
    SH_DEBUG_ASSERT(AAP != m_aa_program.end_inputs());
    m_inputMap[*P] = *AAP;
  }

  // set up output map
  P = m_program.begin_outputs();
  AAP = m_aa_program.begin_outputs();
  for(; P != m_program.end_outputs(); ++P, ++AAP) {
    SH_DEBUG_ASSERT(AAP != m_aa_program.end_outputs());
    m_outputMap[*AAP] = *P;
  }

  // set up m_idxStmt
  StmtIndexGatherer sig;
  sig.init(&m_idxStmt);
  sig.transform(m_program.node());

  // set up symInput and symStmt maps (maps noise symbol idx to either input or
  // stmt that generates it)
  AaVarSymsMap::iterator I = m_psyms->inputs.begin();
  for(;I != m_psyms->inputs.end(); ++I) {
    VariableNodePtr node = I->first;
    const AaSyms& syms = I->second; 
    for(int i = 0; i < node->size(); ++i) {
      for(AaIndexSet::const_iterator S = syms[i].begin(); 
          S != syms[i].end(); ++S) {
        m_symInput[*S] = VarIntPair(node, i); 
      }
    }
  }

  AaStmtSymsMap::iterator J = m_psyms->stmts.begin();
  for(;J != m_psyms->stmts.end(); ++J) {
    const StmtIndex& stmtIndex = J->first;
    const AaSyms& newsyms = J->second;
    for(int i = 0; i < newsyms.size(); ++i) {
      for(AaIndexSet::const_iterator S = newsyms[i].begin(); 
          S != newsyms[i].end(); ++S) {
        SH_DEBUG_ASSERT(m_idxStmt[stmtIndex]);
        m_symStmt[*S] = StmtIntPair(m_idxStmt[stmtIndex], i);
      }
    }
  }

}


Stream AaScanCore::doit(VariableNodePtr input, const Stream& inputStream,
                          AaVarVec& pattern, Record& record)
{
  SH_DEBUG_PRINT_ASC("parsing? " << Context::current()->parsing());

  // get input ranges from m_program
  pattern.clear();

  Program m_aa_clone(m_aa_opped.node()->clone());

  // @todo - there's really two functions intertwined in here
  // one for input = 0 and one for input != 0...
  // should probably split this up
  Record inputRecord; // just for refcounting purposes when input=0
  ProgramNode::VarList::const_iterator I;
  for(I = m_program.begin_inputs(); I != m_program.end_inputs(); ++I) {
    const Variable &var = *I; 
    VariableNodePtr node = var.node();

    if(node == input) { // bind stream to input
      SH_DEBUG_PRINT("Using inputStream for input " << node->name());
      m_aa_clone = m_aa_clone(inputStream);
    } else { // take range meta info and bind to input 
      if(!node->hasRange()) {
        SH_DEBUG_PRINT_ASC("Warning: " << var.name() << " does not have range");
      }

      // assign ranges
      Variable lowBound(makeTemp(node, "low"));
      Variable highBound(makeTemp(node, "high"));
      lowBound.setVariant(var.lowBoundVariant());
      highBound.setVariant(var.highBoundVariant());

      Variable center(makeTemp(node, var.name() + "_center"));
      Variable err(makeTemp(node, var.name() + "_err"));

      shLRP(center, ConstAttrib1f(0.5f), highBound, lowBound);
      shLRP(err, ConstAttrib1f(0.5f), highBound, -lowBound);
      SH_DEBUG_PRINT_ASC("Using bounds for input " << var.name() << " bounds=[" << lowBound << ", " << highBound 
          << "] center=" << center << " err=" << err);

      Record varRecord = center & err;

      if(input) {
        m_aa_clone = m_aa_clone(varRecord);
      } else {
        inputRecord.append(varRecord);
        Stream varStream = toStream(varRecord);
        Record foo(toRecord(varStream, 0));
        for(Record::iterator I = foo.begin(); I != foo.end(); ++I) {
          SH_DEBUG_PRINT(*I);
        }
      SH_DEBUG_PRINT_ASC("Using bounds for input " << var.name() << " bounds=[" << lowBound << ", " << highBound 
          << "] center=" << center << " err=" << err);
        m_aa_clone = m_aa_clone(toStream(varRecord));
      }
    }
  }

  // set up return pattern/record 
  for(I = m_aa_program.begin_outputs(); I != m_aa_program.end_outputs(); ++I) {
    const Variable &var = *I; 
    VariableNodePtr aaNode = var.node(); 
    const AaSyms &syms = m_psyms->outputs[aaNode];
    SH_DEBUG_ASSERT(aaNode && !syms.empty());

    AaVariable aaOutput(new AaVariableNode(aaNode, syms)); 
    pattern.push_back(aaOutput);
    record.append(aaOutput.node()->record());
  }

  //@todo range - totally broken here
  assert(false);
#if 0
  int length = input ? inputStream.begin()->get_count() : 1;
  Stream resultStream = makeStream(record, length); 
  resultStream = m_aa_clone;
  return resultStream;
#endif
  return Stream();
}

bool AaScanCore::isInputSym(int idx)
{
  return m_symInput.find(idx) != m_symInput.end();
}

bool AaScanCore::isStmtSym(int idx)
{
  return m_symStmt.find(idx) != m_symStmt.end();
}

AaScanCore::VarIntPair AaScanCore::getInput(int idx)
{
  SH_DEBUG_ASSERT(isInputSym(idx)); 
  return m_symInput[idx];
}

AaScanCore::StmtIntPair AaScanCore::getStmt(int idx)
{
  SH_DEBUG_ASSERT(isStmtSym(idx)); 
  return m_symStmt[idx];
}

}
