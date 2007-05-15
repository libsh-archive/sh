// Sh: A shGPU metaprogramming language.
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
#include <iterator>
#include <algorithm>
#include <fstream>
#include "Context.hpp"
#include "CfgBlock.hpp"
#include "BitSet.hpp"
#include "Attrib.hpp"
#include "VariableNode.hpp"
#include "Record.hpp"
#include "Inclusion.hpp"
#include "Internals.hpp"
#include "Syntax.hpp"
#include "Optimizations.hpp"
#include "RangeBranchFixer.hpp"
#include "TextureNode.hpp"
#include "Transformer.hpp"
#include "CtrlGraphWranglers.hpp"

namespace {

using namespace SH;

//#define SH_DEBUG_INCL

void dump(Program a, std::string name) {
#ifdef SH_DEBUG_INCL
    a.node()->dump(name);
#endif
}



struct IntervalInfo {
  static const char* Prefix; 

  static bool isType(ValueType value_type) 
  { return isInterval(value_type); }

  static ValueType toRangeType(ValueType value_type) 
  { return intervalValueType(value_type); }
};
const char* IntervalInfo::Prefix = "ia_";

struct AffineInfo {
  static const char* Prefix; 

  static bool isType(ValueType value_type) 
  { return isAffine(value_type); }

  static ValueType toRangeType(ValueType value_type) 
  { return affineValueType(value_type); }
};
const char* AffineInfo::Prefix = "aa_";

}

namespace SH  {

unsigned long long StmtIndex::cur_index = 0;

StmtIndex::StmtIndex()
  : m_index(cur_index++) 
{}

Info* StmtIndex::clone() const
{
  return new StmtIndex(*this);
}

long long StmtIndex::index() const
{
  return m_index;
}

bool StmtIndex::operator<(const StmtIndex& other) const
{
  return m_index < other.m_index;
}

struct StmtIndexAdderBase: public TransformerParent {
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* cfg_node)
  {
    if(!I->get_info<StmtIndex>()) {
      I->add_info(new StmtIndex());
    }
    return false;
  }
};
typedef DefaultTransformer<StmtIndexAdderBase> StmtIndexAdder;

void add_stmt_indices(Program a) {
  StmtIndexAdder sia;
  sia.transform(a.node());
}

struct StmtIndexGathererBase: public TransformerParent {
  IndexStmtMap* ismap;
  void init(IndexStmtMap& ism) {
    ismap = &ism;
  }
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* cfg_node)
  {
    StmtIndex* index = I->get_info<StmtIndex>();
    if(index) {
      (*ismap)[*index] = &*I;
    }
    return false;
  }
};
typedef DefaultTransformer<StmtIndexGathererBase> StmtIndexGatherer;

IndexStmtMap gather_indices(Program a) {
  IndexStmtMap result;
  StmtIndexGatherer sig;
  sig.init(result);
  sig.transform(a.node());
  return result;
}

/* isInterval detector
 * regular -> interval mapping
 * name prefix
 * texture class
 */

/** First step - Change all regular variables to interval variables,
 * Change all texture lookups to interval texture lookups or a constant range
 * from the texture.
 *
 * Updated to handle programs with some variables already bound
 */
template<typename F>
struct FloatToRangeBase: public TransformerParent {
  const Program* f;
  Program* rangef;

  FloatToRangeBase() 
    : prog(Context::current()->parsing())
  {
    prog->collectAll();
  }

  void setPrograms(const Program& f, Program& rangef) {
    this->f = &f;
    this->rangef = &rangef;
    rangef.clone_bindings_from(f);
  }

  void handleVarList(ProgramNode::VarList &varlist, BindingType type)
  {
    ProgramNode::VarList::iterator I = varlist.begin(); 
    if(type == SH_INPUT) { /* skip over bound variables */
      std::advance(I, this->rangef->binding_spec.size());
    } 
    for(;I != varlist.end();) {
      VariableNodePtr node = *I;
      if(handleVar(node)) {
        I = varlist.erase(I);
        varlist.insert(I, varmap[node]);
        m_changed = true;
      } else ++I;
    }
  }

  bool handleVar(const VariableNodePtr& node)
  {
    if(varmap.count(node) > 0) return true;

    BindingType kind = node->kind();
    if(kind == SH_TEMP) {
      if(node->uniform() || !prog->hasDecl(node)) {
        return false;
      }
    } else if (kind == SH_CONST || kind == SH_TEXTURE || kind == SH_STREAM) return false;
    // @todo range decide what to do with palettes

    // todo do something more intelligent with boolean intervals 
    ValueType valType = node->valueType();
    if(F::isType(valType)) return false; /**/
    ValueType rangeType = F::toRangeType(valType); /**/

    if(rangeType == VALUETYPE_END) {
      SH_DEBUG_ERROR("Variable " << node->name() << " is of type " << valueTypeName(valType) << " which has no range equivalent");
    }

    varmap[node] = node->clone(BINDINGTYPE_END, 0, rangeType, SEMANTICTYPE_END, false, true);
    varmap[node]->name(F::Prefix + node->name());
#ifdef SH_DEBUG_INCL
    SH_DEBUG_PRINT("Mapped variable " << node->nameOfType() << " " << node->name() << " -> " << 
                                         varmap[node]->nameOfType() << " " << varmap[node]->name());
#endif
    return true;
  }

#if 0
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* cfg_node)
  {
    Statement &stmt = *I;

    // handle texture lookup/stream fetches
    // we're doing this in the AaOpHandler.cpp now...
    switch(stmt.op) {
      case OP_TEX:
      case OP_TEXI:
        {
          if(!isInterval(stmt.src[0].valueType())) break;

          TextureNodePtr node = shref_dynamic_cast<TextureNode>(stmt.src[0].node());
          SH_DEBUG_ASSERT(node);
          if(!F::rangeTexMap[node]) {
            F::rangeTexMap[node] = new typename F::TexClass(node);
          }
          typename F::TexClass *itex = F::rangeTexMap[node];

          int oldOpt = Context::current()->optimization();
          Context::current()->optimization(0);
          Program lookup_program = SH_BEGIN_PROGRAM() {
            if(stmt.op == OP_TEX) itex->lookup(stmt.dest, stmt.src[1]); 
            else if(stmt.op == OP_TEXI) itex->rect_lookup(stmt.dest, stmt.src[1]); 
          } SH_END;
          Context::current()->optimization(oldOpt);
          replaceStmt(cfg_node, I, lookup_program);
          I = cfg_node->block->end();
          return true;
        }
        break;
      case OP_TEXD: // @todo range prove that this works out with the interval textures
        {
          SH_DEBUG_ASSERT(0 && "Cannot handle this");
        }
        break;
      default:
        break;
    }
    return false;
  }
#endif

  void finish() {
    VariableReplacer vr(varmap);
    m_program->ctrlGraph->dfs(vr); 
  }

  VarMap varmap;
  ProgramNodePtr prog;
};
typedef DefaultTransformer<FloatToRangeBase<IntervalInfo> > FloatToIntervalConverter;
typedef DefaultTransformer<FloatToRangeBase<AffineInfo> > FloatToAffineConverter;

struct AffineInputToIntervalBase: public TransformerParent {
  void finish() 
  {
    VarMap aamap;
    
    Program inputMapper = SH_BEGIN_PROGRAM() {
      /* @todo fix this section for binding_spec */
      for(ProgramNode::VarList::iterator I = m_program->inputs.begin(); I != m_program->inputs.end();) {
        VariableNodePtr node = *I;
        ValueType valueType = node->valueType();
        if(!isAffine(valueType)) {
          ++I;
          continue;
        }

        ValueType regularType = regularValueType(valueType);
        ValueType iaType = intervalValueType(regularType);

        Variable iaInput(node->clone(SH_INPUT, 0, iaType, SEMANTICTYPE_END, 
              false, false));
        iaInput.name("ia_" + node->name());
        VariableNodePtr tempNode = node->clone(SH_TEMP);
        aamap[node] = tempNode; 
        Variable tempVar(tempNode);
        shASN(tempVar, iaInput);

        m_program->inputs.insert(I, iaInput.node());
        I = m_program->inputs.erase(I);
      }
    } SH_END;
    VariableReplacer vr(aamap);
    m_program->ctrlGraph->dfs(vr);
    m_program->ctrlGraph->prepend(std::auto_ptr<CtrlGraph>(inputMapper.node()->ctrlGraph->clone()));
  }
};
typedef DefaultTransformer<AffineInputToIntervalBase> AffineInputToInterval;

struct AffineOutputToIntervalBase: public TransformerParent {
  void finish() 
  {
    VarMap aamap;
    
    Program outputMapper = SH_BEGIN_PROGRAM() {
      for(ProgramNode::VarList::iterator I = m_program->outputs.begin(); I != m_program->outputs.end();) {
        VariableNodePtr node = *I;
        ValueType valueType = node->valueType();
        if(!isAffine(valueType)) {
          ++I;
          continue;
        }

        ValueType regularType = regularValueType(valueType);
        ValueType iaType = intervalValueType(regularType);

        Variable iaOutput(node->clone(SH_OUTPUT, 0, iaType, SEMANTICTYPE_END, 
              false, false));
        iaOutput.name("ia_" + node->name());
        VariableNodePtr tempNode = node->clone(SH_TEMP);
        aamap[node] = tempNode; 
        shASN(iaOutput, Variable(tempNode));

        m_program->outputs.insert(I, iaOutput.node());
        I = m_program->outputs.erase(I);
      }
    } SH_END;
    VariableReplacer vr(aamap);
    m_program->ctrlGraph->dfs(vr);
    m_program->ctrlGraph->append(std::auto_ptr<CtrlGraph>(outputMapper.node()->ctrlGraph->clone()));
  }
};
typedef DefaultTransformer<AffineOutputToIntervalBase> AffineOutputToInterval;

// expects to Context::current()->parsing() == p.  Applies
// FloatToIntervalConverter to p 
Program inclusion(Program a)
{
  add_stmt_indices(a);
  Program result(a.node()->clone()); 
  result.name(a.name() + "_ia_incl");
  dump(result, a.name() + "_inclusion_start");

  Context::current()->enter(result.node());
    FloatToIntervalConverter ftic;
    ftic.setPrograms(a, result);
    ftic.transform(result.node());
    fixRangeBranches(result);
    optimize(result);
  Context::current()->exit();

  dump(result, a.name() + "_inclusion_done");
  return result;
}

Program affine_inclusion(Program a)
{
  Program result = affine_inclusion_syms(a);
  affine_to_interval_inputs(result);
  affine_to_interval_outputs(result);
  return result;
}

Program affine_inclusion_syms(Program a)
{
  add_stmt_indices(a);
  Program result(a.node()->clone());
  result.name(a.name() + "_ia_incl");
  dump(result, a.name() + "_aaincl_start");

  Context::current()->enter(result.node());
    FloatToAffineConverter ftac;
    ftac.setPrograms(a, result);
    ftac.transform(result.node());
  dump(result, a.name() + "_aaincl_ftac");
    fixRangeBranches(result);
    optimize(result);
  Context::current()->exit();

  dump(result, a.name() + "_aaincl_done");
  return result;
}

void affine_to_interval_inputs(Program a)
{
  dump(a, a.name() + "_atoi_inputs_start");

  int oldOptimization = Context::current()->optimization();
  Context::current()->optimization(0);
  Context::current()->enter(a.node());
    AffineInputToInterval aii;
    aii.transform(a.node());
  Context::current()->exit();
  Context::current()->optimization(oldOptimization);
  dump(a, a.name() + "_atoi_inputs_done");
}

void affine_to_interval_outputs(Program a)
{
  dump(a, a.name() + "_atoi_outputs_start");
  int oldOptimization = Context::current()->optimization();
  Context::current()->optimization(0);
  Context::current()->enter(a.node());
    AffineOutputToInterval aii;
    aii.transform(a.node());
  Context::current()->exit();
  Context::current()->optimization(oldOptimization);
  dump(a, a.name() + "_atoi_outputs_done");
}

// @todo range - add something after affine_inclusion to 
//  a) change aa inputs to intervals and insert appropriate conversion code
//  b) change aa outputs to 
//      b.1) intervals 
//      b.2) aa with only error symbols from inputs

}
