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
#include "ShRecStream.hpp"
#include "ShStream.hpp"
#include "ShInclusion.hpp"
#include "ShTransformer.hpp"
#include "ShAaSymPlacer.hpp"
#include "ShAaOpHandler.hpp"
#include "ShAaScanCore.hpp"

#ifdef SH_DBG_AA
#define SH_DBG_ASCCANCORE
#endif
#define SH_DBG_ASCCANCORE

#ifdef SH_DBG_ASCCANCORE
#define SH_DEBUG_PRINT_ASC(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_ASC(x) {}
#endif

namespace {
using namespace SH;

struct StmtIndexGathererBase: public ShTransformerParent {
  void init(ShAaScanCore::IndexStmtMap* idxStmt) {
    m_idxStmt = idxStmt;
  }

  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) 
  { 
    ShStmtIndex* idx = I->get_info<ShStmtIndex>();
    if(idx) {
      (*m_idxStmt)[*idx] = &*I;
    }
    return false;
  }
  ShAaScanCore::IndexStmtMap* m_idxStmt;
};
typedef ShDefaultTransformer<StmtIndexGathererBase> StmtIndexGatherer;

ShVariable makeTemp(ShVariableNodePtr node, const std::string& name) {
  ShVariable result(node->clone(SH_TEMP, 0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false)); 
  result.name(name);
  return result;
}

}
namespace SH {

ShAaScanCore::ShAaScanCore(ShProgram program)
  : m_program(program.node()->clone())
{
#ifdef SH_DBG_ASCCANCORE
  m_program.node()->dump("aascan_start");
#endif
  m_aa_program = affine_inclusion_syms(m_program);
#ifdef SH_DBG_ASCCANCORE
  m_aa_program.node()->dump("aascan_aa");
#endif
  placeAaSyms(m_aa_program.node());

  m_psyms = m_aa_program.node()->get_info<ShAaProgramSyms>();
  SH_DEBUG_ASSERT(m_psyms);
  SH_DEBUG_PRINT_ASC("Program Syms");
  SH_DEBUG_PRINT_ASC(*m_psyms);

  m_aa_opped = ShProgram(m_aa_program.node()->clone());
  handleAaOps(m_aa_opped.node());
  
#ifdef SH_DBG_ASCCANCORE
  m_aa_opped.node()->dump("aascan_op");
#endif

  // set up input map 
  ShProgramNode::VarList::const_iterator P, AAP;
  P = m_program.inputs_begin();
  AAP = m_aa_program.inputs_begin();
  for(; P != m_program.inputs_end(); ++P, ++AAP) {
    SH_DEBUG_ASSERT(AAP != m_aa_program.inputs_end());
    m_inputMap[*P] = *AAP;
  }

  // set up output map
  P = m_program.outputs_begin();
  AAP = m_aa_program.outputs_begin();
  for(; P != m_program.outputs_end(); ++P, ++AAP) {
    SH_DEBUG_ASSERT(AAP != m_aa_program.outputs_end());
    m_outputMap[*AAP] = *P;
  }

  // set up m_idxStmt
  StmtIndexGatherer sig;
  sig.init(&m_idxStmt);
  sig.transform(m_program.node());

  // set up symInput and symStmt maps (maps noise symbol idx to either input or
  // stmt that generates it)
  ShAaVarSymsMap::iterator I = m_psyms->inputs.begin();
  for(;I != m_psyms->inputs.end(); ++I) {
    ShVariableNodePtr node = I->first;
    const ShAaSyms& syms = I->second; 
    for(int i = 0; i < node->size(); ++i) {
      for(ShAaIndexSet::const_iterator S = syms[i].begin(); 
          S != syms[i].end(); ++S) {
        m_symInput[*S] = VarIntPair(node, i); 
      }
    }
  }

  ShAaStmtSymsMap::iterator J = m_psyms->stmts.begin();
  for(;J != m_psyms->stmts.end(); ++J) {
    const ShStmtIndex& stmtIndex = J->first;
    const ShAaSyms& newsyms = J->second;
    for(int i = 0; i < newsyms.size(); ++i) {
      for(ShAaIndexSet::const_iterator S = newsyms[i].begin(); 
          S != newsyms[i].end(); ++S) {
        SH_DEBUG_ASSERT(m_idxStmt[stmtIndex]);
        m_symStmt[*S] = StmtIntPair(m_idxStmt[stmtIndex], i);
      }
    }
  }

}


ShStream ShAaScanCore::doit(ShVariableNodePtr input, const ShStream& inputStream,
                          AaVarVec& pattern, ShRecord& record)
{
  SH_DEBUG_PRINT_ASC("parsing? " << ShContext::current()->parsing());

  // get input ranges from m_program
  pattern.clear();

  ShProgram m_aa_clone(m_aa_opped.node()->clone());

  // @todo - there's really two functions intertwined in here
  // one for input = 0 and one for input != 0...
  // should probably split this up
  ShRecord inputRecord; // just for refcounting purposes when input=0
  ShProgramNode::VarList::const_iterator I;
  for(I = m_program.inputs_begin(); I != m_program.inputs_end(); ++I) {
    const ShVariable &var = *I; 
    ShVariableNodePtr node = var.node();

    if(node == input) { // bind stream to input
      SH_DEBUG_PRINT("Using inputStream for input " << node->name());
      m_aa_clone = m_aa_clone(inputStream);
    } else { // take range meta info and bind to input 
      if(!node->hasRange()) {
        SH_DEBUG_PRINT_ASC("Warning: " << var.name() << " does not have range");
      }

      // assign ranges
      ShVariable lowBound(makeTemp(node, "low"));
      ShVariable highBound(makeTemp(node, "high"));
      lowBound.setVariant(var.lowBoundVariant());
      highBound.setVariant(var.highBoundVariant());

      ShVariable center(makeTemp(node, var.name() + "_center"));
      ShVariable err(makeTemp(node, var.name() + "_err"));

      shLRP(center, ShConstAttrib1f(0.5f), highBound, lowBound);
      shLRP(err, ShConstAttrib1f(0.5f), highBound, -lowBound);
      SH_DEBUG_PRINT_ASC("Using bounds for input " << var.name() << " bounds=[" << lowBound << ", " << highBound 
          << "] center=" << center << " err=" << err);

      ShRecord varRecord = center & err;

      if(input) {
        m_aa_clone = m_aa_clone(varRecord);
      } else {
        inputRecord.append(varRecord);
        ShStream varStream = toStream(varRecord);
        ShRecord foo(toRecord(varStream, 0));
        for(ShRecord::iterator I = foo.begin(); I != foo.end(); ++I) {
          SH_DEBUG_PRINT(*I);
        }
      SH_DEBUG_PRINT_ASC("Using bounds for input " << var.name() << " bounds=[" << lowBound << ", " << highBound 
          << "] center=" << center << " err=" << err);
        m_aa_clone = m_aa_clone(toStream(varRecord));
      }
    }
  }

  // set up return pattern/record 
  for(I = m_aa_program.outputs_begin(); I != m_aa_program.outputs_end(); ++I) {
    const ShVariable &var = *I; 
    ShVariableNodePtr aaNode = var.node(); 
    const ShAaSyms &syms = m_psyms->outputs[aaNode];
    SH_DEBUG_ASSERT(aaNode && !syms.empty());

    ShAaVariable aaOutput(new ShAaVariableNode(aaNode, syms)); 
    pattern.push_back(aaOutput);
    record.append(aaOutput.node()->record());
  }

  int length = input ? (*inputStream.begin())->count() : 1;
  ShStream resultStream = makeStream(record, length); 
  resultStream = m_aa_clone;
  return resultStream;
}

bool ShAaScanCore::isInputSym(int idx)
{
  return m_symInput.find(idx) != m_symInput.end();
}

bool ShAaScanCore::isStmtSym(int idx)
{
  return m_symStmt.find(idx) != m_symStmt.end();
}

ShAaScanCore::VarIntPair ShAaScanCore::getInput(int idx)
{
  SH_DEBUG_ASSERT(isInputSym(idx)); 
  return m_symInput[idx];
}

ShAaScanCore::StmtIntPair ShAaScanCore::getStmt(int idx)
{
  SH_DEBUG_ASSERT(isStmtSym(idx)); 
  return m_symStmt[idx];
}

}
