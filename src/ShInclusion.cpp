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
#include <iterator>
#include <algorithm>
#include <fstream>
#include "ShContext.hpp"
#include "ShCfgBlock.hpp"
#include "ShBitSet.hpp"
#include "ShAttrib.hpp"
#include "ShVariableNode.hpp"
#include "ShRecord.hpp"
#include "ShInclusion.hpp"
#include "ShInternals.hpp"
#include "ShSyntax.hpp"
#include "ShOptimizations.hpp"
#include "ShRangeBranchFixer.hpp"
#include "ShTextureNode.hpp"
#include "ShTransformer.hpp"
#include "ShCtrlGraphWranglers.hpp"

namespace {

using namespace SH;

#define SH_DEBUG_INCL

void dump(ShProgram a, const char* name) {
#ifdef SH_DEBUG_INCL
    a.node()->dump(name);
#endif
}



struct IntervalInfo {
  static const char* Prefix; 

  static bool isType(ShValueType value_type) 
  { return shIsInterval(value_type); }

  static ShValueType toRangeType(ShValueType value_type) 
  { return shIntervalValueType(value_type); }
};
const char* IntervalInfo::Prefix = "ia_";

struct AffineInfo {
  static const char* Prefix; 

  static bool isType(ShValueType value_type) 
  { return shIsAffine(value_type); }

  static ShValueType toRangeType(ShValueType value_type) 
  { return shAffineValueType(value_type); }
};
const char* AffineInfo::Prefix = "aa_";

}

namespace SH  {

unsigned long long ShStmtIndex::cur_index = 0;

ShStmtIndex::ShStmtIndex()
  : m_index(cur_index++) 
{}

ShInfo* ShStmtIndex::clone() const
{
  return new ShStmtIndex(*this);
}

long long ShStmtIndex::index() const
{
  return m_index;
}

bool ShStmtIndex::operator<(const ShStmtIndex& other) const
{
  return m_index < other.m_index;
}

struct StmtIndexAdderBase: public ShTransformerParent {
  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr cfg_node)
  {
    if(!I->get_info<ShStmtIndex>()) {
      I->add_info(new ShStmtIndex());
    }
    return false;
  }
};
typedef ShDefaultTransformer<StmtIndexAdderBase> StmtIndexAdder;

void add_stmt_indices(ShProgram a) {
  StmtIndexAdder sia;
  sia.transform(a.node());
}

struct StmtIndexGathererBase: public ShTransformerParent {
  ShIndexStmtMap* ismap;
  void init(ShIndexStmtMap& ism) {
    ismap = &ism;
  }
  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr cfg_node)
  {
    ShStmtIndex* index = I->get_info<ShStmtIndex>();
    if(index) {
      (*ismap)[*index] = &*I;
    }
    return false;
  }
};
typedef ShDefaultTransformer<StmtIndexGathererBase> StmtIndexGatherer;

ShIndexStmtMap gather_indices(ShProgram a) {
  ShIndexStmtMap result;
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
 */
template<typename F>
struct FloatToRangeBase: public ShTransformerParent {
  FloatToRangeBase() 
    : prog(ShContext::current()->parsing())
  {}

  void handleVarList(ShProgramNode::VarList &varlist, ShBindingType type)
  {
    for(ShProgramNode::VarList::iterator I = varlist.begin(); I != varlist.end();) {
      ShVariableNodePtr node = *I;
      if(handleVar(node)) {
        I = varlist.erase(I);
        varlist.insert(I, varmap[node]);
        m_changed = true;
      } else ++I;
    }
  }

  bool handleVar(ShVariableNodePtr node)
  {
    if(varmap.count(node) > 0) return true;

    ShBindingType kind = node->kind();
    if(kind == SH_TEMP) {
      if(node->uniform() || !prog->hasDecl(node)) {
        SH_DEBUG_PRINT("Ignoring " << node->nameOfType() << node->name() << " in " << F::Prefix << " inclusion");
        return false;
      }
    } else if (kind == SH_CONST || kind == SH_TEXTURE || kind == SH_STREAM) return false;
    // @todo range decide what to do with palettes

    // todo do something more intelligent with boolean intervals 
    ShValueType valType = node->valueType();
    if(F::isType(valType)) return false; /**/
    ShValueType rangeType = F::toRangeType(valType); /**/

    if(rangeType == SH_VALUETYPE_END) {
      SH_DEBUG_ERROR("Variable " << node->name() << " is of type " << shValueTypeName(valType) << " which has no range equivalent");
    }

    varmap[node] = node->clone(SH_BINDINGTYPE_END, 0, rangeType, SH_SEMANTICTYPE_END, false, true);
    varmap[node]->name(F::Prefix + node->name());
#ifdef SH_DEBUG_INCL
    SH_DEBUG_PRINT("Mapped variable " << node->nameOfType() << " " << node->name() << " -> " << 
                                         varmap[node]->nameOfType() << " " << varmap[node]->name());
#endif
    return true;
  }

#if 0
  bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr cfg_node)
  {
    ShStatement &stmt = *I;

    // handle texture lookup/stream fetches
    // we're doing this in the ShAaOpHandler.cpp now...
    switch(stmt.op) {
      case SH_OP_TEX:
      case SH_OP_TEXI:
        {
          if(!shIsInterval(stmt.src[0].valueType())) break;

          ShTextureNodePtr node = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());
          SH_DEBUG_ASSERT(node);
          if(!F::rangeTexMap[node]) {
            F::rangeTexMap[node] = new typename F::TexClass(node);
          }
          typename F::TexClass *itex = F::rangeTexMap[node];

          int oldOpt = ShContext::current()->optimization();
          ShContext::current()->optimization(0);
          ShProgram lookup_program = SH_BEGIN_PROGRAM() {
            if(stmt.op == SH_OP_TEX) itex->lookup(stmt.dest, stmt.src[1]); 
            else if(stmt.op == SH_OP_TEXI) itex->rect_lookup(stmt.dest, stmt.src[1]); 
          } SH_END;
          ShContext::current()->optimization(oldOpt);
          replaceStmt(cfg_node, I, lookup_program);
          I = cfg_node->block->end();
          return true;
        }
        break;
      case SH_OP_TEXD: // @todo range prove that this works out with the interval textures
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
    ShVariableReplacer vr(varmap);
    m_program->ctrlGraph->dfs(vr); 
  }

  ShVarMap varmap;
  ShProgramNodePtr prog;
};
typedef ShDefaultTransformer<FloatToRangeBase<IntervalInfo> > FloatToIntervalConverter;
typedef ShDefaultTransformer<FloatToRangeBase<AffineInfo> > FloatToAffineConverter;

struct AffineInputToIntervalBase: public ShTransformerParent {
  void finish() 
  {
    ShVarMap aamap;
    
    ShProgram inputMapper = SH_BEGIN_PROGRAM() {
      for(ShProgramNode::VarList::iterator I = m_program->inputs.begin(); I != m_program->inputs.end();) {
        ShVariableNodePtr node = *I;
        ShValueType valueType = node->valueType();
        if(!shIsAffine(valueType)) {
          ++I;
          continue;
        }

        ShValueType regularType = shRegularValueType(valueType);
        ShValueType iaType = shIntervalValueType(regularType);

        ShVariable iaInput(node->clone(SH_INPUT, 0, iaType, SH_SEMANTICTYPE_END, 
              false, false));
        iaInput.name("ia_" + node->name());
        ShVariableNodePtr tempNode = node->clone(SH_TEMP);
        aamap[node] = tempNode; 
        ShVariable tempVar(tempNode);
        shASN(tempVar, iaInput);

        m_program->inputs.insert(I, iaInput.node());
        I = m_program->inputs.erase(I);
      }
    } SH_END;
    ShVariableReplacer vr(aamap);
    m_program->ctrlGraph->dfs(vr);
    m_program->ctrlGraph->prepend(inputMapper.node()->ctrlGraph);
  }
};
typedef ShDefaultTransformer<AffineInputToIntervalBase> AffineInputToInterval;

struct AffineOutputToIntervalBase: public ShTransformerParent {
  void finish() 
  {
    ShVarMap aamap;
    
    ShProgram outputMapper = SH_BEGIN_PROGRAM() {
      for(ShProgramNode::VarList::iterator I = m_program->outputs.begin(); I != m_program->outputs.end();) {
        ShVariableNodePtr node = *I;
        ShValueType valueType = node->valueType();
        if(!shIsAffine(valueType)) {
          ++I;
          continue;
        }

        ShValueType regularType = shRegularValueType(valueType);
        ShValueType iaType = shIntervalValueType(regularType);

        ShVariable iaOutput(node->clone(SH_OUTPUT, 0, iaType, SH_SEMANTICTYPE_END, 
              false, false));
        iaOutput.name("ia_" + node->name());
        ShVariableNodePtr tempNode = node->clone(SH_TEMP);
        aamap[node] = tempNode; 
        shASN(iaOutput, ShVariable(tempNode));

        m_program->outputs.insert(I, iaOutput.node());
        I = m_program->outputs.erase(I);
      }
    } SH_END;
    ShVariableReplacer vr(aamap);
    m_program->ctrlGraph->dfs(vr);
    m_program->ctrlGraph->append(outputMapper.node()->ctrlGraph);
  }
};
typedef ShDefaultTransformer<AffineOutputToIntervalBase> AffineOutputToInterval;

// expects to ShContext::current()->parsing() == p.  Applies
// FloatToIntervalConverter to p 
ShProgram inclusion(ShProgram a)
{
  add_stmt_indices(a);
  ShProgram result(a.node()->clone());
  dump(result,"inclusion_start");

  ShContext::current()->enter(result.node());
    FloatToIntervalConverter ftic;
    ftic.transform(result.node());
    fixRangeBranches(result);
    optimize(result);
  ShContext::current()->exit();

  dump(result, "inclusion_done");
  return result;
}

ShProgram affine_inclusion(ShProgram a)
{
  ShProgram result = affine_inclusion_syms(a);
  affine_to_interval_inputs(result);
  affine_to_interval_outputs(result);
  return result;
}

ShProgram affine_inclusion_syms(ShProgram a)
{
  add_stmt_indices(a);
  ShProgram result(a.node()->clone());
  dump(result, "aaincl_start");

  ShContext::current()->enter(result.node());
    FloatToAffineConverter ftac;
    ftac.transform(result.node());
  dump(result, "aaincl_ftac");
    //fixRangeBranches(result);
    optimize(result);
  ShContext::current()->exit();

  dump(result, "aaincl_done");
  return result;
}

void affine_to_interval_inputs(ShProgram a)
{
  dump(a, "atoi_inputs_start");

  int oldOptimization = ShContext::current()->optimization();
  ShContext::current()->optimization(0);
  ShContext::current()->enter(a.node());
    AffineInputToInterval aii;
    aii.transform(a.node());
  ShContext::current()->exit();
  ShContext::current()->optimization(oldOptimization);
  dump(a, "atoi_inputs_done");
}

void affine_to_interval_outputs(ShProgram a)
{
  dump(a, "atoi_outputs_start");
  int oldOptimization = ShContext::current()->optimization();
  ShContext::current()->optimization(0);
  ShContext::current()->enter(a.node());
    AffineOutputToInterval aii;
    aii.transform(a.node());
  ShContext::current()->exit();
  ShContext::current()->optimization(oldOptimization);
  dump(a, "atoi_outputs_done");
}

// @todo range - add something after affine_inclusion to 
//  a) change aa inputs to intervals and insert appropriate conversion code
//  b) change aa outputs to 
//      b.1) intervals 
//      b.2) aa with only error symbols from inputs

}
