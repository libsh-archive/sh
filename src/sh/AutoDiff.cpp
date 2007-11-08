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
#include "Attrib.hpp"
#include "BasicBlock.hpp"
#include "CtrlGraph.hpp"
#include "Optimizations.hpp"
#include "Transformer.hpp"

#include "AutoDiff.hpp"

namespace {

using namespace SH;
using namespace std;

ConstAttrib1f ZERO = ConstAttrib1f(0.0f);
ConstAttrib1f ONE = ConstAttrib1f(1.0f);

/* Automatic differentiation
 *
 * Pairs each variable with a temporary that holds its first-order derivative,
 * and performs the following transformation.
 *
 * 1) Initially, diff for all inputs = 0, except for input we're taking derivative against
 * 2) Augment each op with computation of derivative
 * 3) Replace outputs with paired derivates 
 */
struct AutoDiffBase: public TransformerParent {
  map<VariableNodePtr, VariableNodePtr> diff;
  Variable var;
  BasicBlock::StmtList pre, post; 

  /* initialize with the variable we are differentiating against */
  void init(const Variable& var) {
    this->var = var;
  }

 void handlePaletteList(ProgramNode::PaletteList &palettelist) {
   ProgramNode::VarList foo(palettelist.begin(), palettelist.end());
   handleVarList(foo, SH_PALETTE);
 }

 void handleTexList(ProgramNode::TexList &texlist) {
   ProgramNode::VarList foo(texlist.begin(), texlist.end());
   handleVarList(foo, SH_TEXTURE);
 }

  void handleVarList(ProgramNode::VarList &varlist, BindingType type) {
    /* Build variables and add initialization code (step 1)
     *
     * and assign outputs to derivatives (step 3) */
    for(ProgramNode::VarList::const_iterator I = varlist.begin();
        I != varlist.end(); ++I) {
      Variable copy;
      switch((*I)->kind()) {
        case SH_CONST:
          copy = Variable((*I)->clone());
          break;
        case SH_STREAM:
          SH_DEBUG_ASSERT(false);
          break;
        case SH_TEXTURE:
        case SH_PALETTE:
          cout << "copying tex/pal" << endl; 
        default:
          copy = Variable((*I)->clone(SH_TEMP));
          copy.name((*I)->name() + "'");
          break;
      }
      // uniforms are already zero
      if(!copy.uniform() && copy.node()->kind() != SH_CONST) {
        pre.push_back(Statement(copy, OP_ASN, ZERO.repeat(copy.size()))); 
        if((*I) == var.node()) {
          pre.push_back(Statement(copy(var.swizzle()[0]), OP_ASN, ONE));
        }

        if((*I)->kind() == SH_OUTPUT) {
          post.push_back(Statement(Variable(*I), OP_ASN, copy));
        }
      }
      diff[*I] = copy.node(); 
    }
  }

  void finish() {
    BasicBlockPtr entryBlock = m_program->ctrlGraph->prepend_entry()->block;
    entryBlock->splice(entryBlock->begin(), pre);

    BasicBlockPtr exitBlock = m_program->ctrlGraph->append_exit()->block; 
    exitBlock->splice(exitBlock->end(), post);
  }

  /* Gets the variable for the derivative corresponding to v */
  Variable d(const Variable& v, BasicBlock::StmtList &extras, bool copy=true) {
    SH_DEBUG_ASSERT_PRINT(diff.find(v.node()) != diff.end(), v.name());
    Variable dv(diff[v.node()], v.swizzle(), v.neg()); 
    if(copy) {
      Variable dv_copy(dv.node()->clone(SH_TEMP, dv.size()));
      dv_copy.name(dv.name() + "_copy");
      extras.push_back(Statement(dv_copy, OP_ASN, dv));
      return dv_copy;
    } else {
      return dv;
    }
  }

  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) {
    Statement& stmt = *I;
    BasicBlockPtr block = node->block;
    BasicBlock::StmtList extras;
    Variable dest = stmt.dest;
    if(dest.null()) return false;

    /* Get copies before modifying anything, in case dest == src[i] */
    Variable a = stmt.src[0];
    Variable da = stmt.src[0].null() ? Variable() : d(a, extras); 
    Variable b = stmt.src[1];
    Variable db = stmt.src[1].null() ? Variable() : d(b, extras); 
    Variable c = stmt.src[2];
    Variable dc = stmt.src[2].null() ? Variable() : d(c, extras); 
    Variable result = d(dest, extras, false);
    Variable one = ConstAttrib1f(1.0).repeat(result.size()); 
    Variable half = ConstAttrib1f(0.5).repeat(result.size()); 
    Variable zero = ConstAttrib1f(0.0).repeat(result.size()); 
    switch(stmt.op) {
      case OP_ASN: // a' = a'
      case OP_NEG: // (-a)' = -(a')
      case OP_FRAC: // frac'(a) = a' (ignoring discontinuities)
        extras.push_back(Statement(result, stmt.op, da));
        break;

      case OP_RND:
      case OP_FLR:
      case OP_CEIL: // derivative is 0 (ignoring discontinuities)
        extras.push_back(Statement(result, OP_ASN, ZERO.repeat(result.size())));
        break;

      case OP_ADD: // (a + b)' = a' + b'
        extras.push_back(Statement(result, OP_ADD, da, db));
        break;
      case OP_MUL: { // (ab)' = a'b + ab'
        extras.push_back(Statement(result, OP_MUL, da, b)); 
        extras.push_back(Statement(result, OP_MAD, a, db, result));  
        break;
      }

      case OP_SLT: 
      case OP_SLE:
      case OP_SGT:
      case OP_SGE:
      case OP_SEQ:
      case OP_SNE: { // @todo check this - actually discontinuous, but otherwise zero?
        extras.push_back(Statement(result, OP_ASN, ZERO.repeat(result.size())));
        break;
      }

      case OP_ABS: { // (|a|)' = a' sign(a) 
        extras.push_back(Statement(result, OP_SGN, a));
        extras.push_back(Statement(result, OP_MUL, da, result));
        break;
      }
      case OP_DIV: { // (a/b)' = (a'b - ab') / b^2
        Variable b2(result.node()->clone(SH_TEMP, result.size())); 
        extras.push_back(Statement(b2, OP_MUL, b, b)); 
        extras.push_back(Statement(result, OP_MUL, da, -b)); 
        extras.push_back(Statement(result, OP_MAD, a, db, result));  
        extras.push_back(Statement(result, OP_DIV, b2));
        break;
      }

      case OP_DOT: { // (a|b)' = (a' | b) + (a | b')  
        Variable adb(result.node()->clone(SH_TEMP, 1));
        Variable dab(result.node()->clone(SH_TEMP, 1));
        extras.push_back(Statement(dab, OP_DOT, da, b)); 
        extras.push_back(Statement(adb, OP_DOT, a, db)); 
        extras.push_back(Statement(result, adb, OP_ADD, dab));
        break;
      }
      case OP_MIN: { // min(a, b)' = a' * (a < b) + b' * (b < a), discontinuous at a==b (@todo or requires more thought)
        Variable altb(result.node()->clone(SH_TEMP, result.size())); 
        extras.push_back(Statement(altb, a, OP_SLT, b));
        extras.push_back(Statement(result, OP_COND, altb, da, db)); 
        break;
      }
      case OP_MAX: { // max(a, b)' = a' * (a > b) + b' * (b > a), discontinuous at a==b
        Variable agtb(result.node()->clone(SH_TEMP, result.size())); 
        extras.push_back(Statement(agtb, a, OP_SGT, b));
        extras.push_back(Statement(result, OP_COND, agtb, da, db)); 
        break;
      }
      case OP_NORM: { // norm(a) = a / sqrt(a|a)
        /* so norm'(a) = (a' sqrt(a|a) - a sqrt'(a|a)) / (a|a))
         *    sqrt'(a|a) = 0.5 (a|a)' / sqrt(a|a) 
         *    (a|a)' = 2a
         * All together we get
         * norm'(a) = (a' sqrt(a|a) - a * a / sqrt(a|a)) / (a|a)
         *          = (a' - a * a) / ((a|a) sqrt((a|a)))
         */
        Variable adota(result.node()->clone(SH_TEMP, 1)); 
        Variable rsq_adota(result.node()->clone(SH_TEMP, 1)); 
        extras.push_back(Statement(adota, a, OP_DOT, a));
        extras.push_back(Statement(rsq_adota, OP_RSQ, adota)); 

        extras.push_back(Statement(result, OP_MAD, -a, a, da)); 
        extras.push_back(Statement(result, OP_MUL, result, rsq_adota.repeat(result.size())));
        extras.push_back(Statement(result, OP_DIV, result, adota.repeat(result.size())));
        break;
      }
      case OP_SIN: { // sin'(a) = a'cos(a)
        extras.push_back(Statement(result, OP_COS, a));
        extras.push_back(Statement(result, OP_MUL, da, result));
        break;
      }
      case OP_COS: { // cos'(a) = -a'sin(a)
        extras.push_back(Statement(result, OP_SIN, a));
        extras.push_back(Statement(result, OP_MUL, -da, result));
        break;
      }
      case OP_TAN: { // tan'(a) = 1 / cos^2(a) 
        extras.push_back(Statement(result, OP_COS, a));
        extras.push_back(Statement(result, OP_MUL, result, result));
        extras.push_back(Statement(result, OP_RCP, result));
        break;
      }
      case OP_ASIN: { // asin'(a) = 1 / sqrt(1 - a^2) 
        extras.push_back(Statement(result, OP_MUL, a, a));
        extras.push_back(Statement(result, OP_ADD, one, -result)); 
        extras.push_back(Statement(result, OP_RSQ, result)); 
        break;
      }
      case OP_ACOS: { // acos'(a) = -1 / sqrt(1 - a^2) 
        extras.push_back(Statement(result, OP_MUL, a, a));
        extras.push_back(Statement(result, OP_ADD, one, -result)); 
        extras.push_back(Statement(result, OP_RSQ, result)); 
        extras.push_back(Statement(result, OP_ASN, -result));
        break;
      }
      case OP_ATAN: { // tan'(a) = 1 / (1 + a^2) 
        extras.push_back(Statement(result, OP_MUL, a, a));
        extras.push_back(Statement(result, OP_ADD, one, result)); 
        extras.push_back(Statement(result, OP_RCP, result));
        break;
      }
      case OP_EXP: { // exp'(a) = a'exp(a)
        extras.push_back(Statement(result, OP_EXP, a));
        extras.push_back(Statement(result, OP_MUL, da, result));
        break;
      }
      case OP_LOG: { // log'(a) = a' / a
        extras.push_back(Statement(result, OP_DIV, da, a));
        break;
      }
      case OP_LRP: { // (a * b + (1 - a) * c)' = a'b + b'a + c' - a'c + c'a 
        extras.push_back(Statement(result, OP_MAD, da, b, dc)); 
        extras.push_back(Statement(result, OP_MAD, db, a, result));
        extras.push_back(Statement(result, OP_MAD, da, c, result));
        extras.push_back(Statement(result, OP_MAD, dc, a, result));
        break;
      }
      case OP_MAD: { // (a * b + c)' = a'b + b'a + c'
        extras.push_back(Statement(result, OP_MAD, da, b, dc)); 
        extras.push_back(Statement(result, OP_MAD, a, db, result));  
        break;
      }
      case OP_RCP: { // (1/a)' = -a' / a^2 
        extras.push_back(Statement(result, OP_MUL, a, a));
        extras.push_back(Statement(result, OP_DIV, -da, result));
        break;
      }
      case OP_SQRT: { // sqrt'(a) = 0.5 a' / sqrt(a) 
        extras.push_back(Statement(result, OP_RSQ, a)); 
        extras.push_back(Statement(result, OP_MUL, result, half)); 
        extras.push_back(Statement(result, OP_MUL, result, da)); 
        break;
      }
      case OP_TEX: 
      case OP_TEXI: // @todo range make this work in general
                    // would need to generate one tex per texcoord to represent d tex / d coord_i
      case OP_PAL: { // derivative is zero or infinity at points of discontinuity (nearest-neighbor filtering) 
        extras.push_back(Statement(result, OP_ASN, zero)); 
        break;
      }
        
      default:
        SH_DEBUG_ASSERT_PRINT(false, "Unable to differntiate op " << opInfo[stmt.op].name);
    }
    block->splice(I, extras);
    return false;
  }
};
typedef DefaultTransformer<AutoDiffBase> AutoDiff;

}


namespace SH  {

Program differentiate(const Program &a, const Variable& var)
{
  Program result = a.clone();
  int oldOptimization = Context::current()->optimization();
  Context::current()->optimization(0);
  Context::current()->enter(result.node());
    AutoDiff ad;
    ad.init(var);
    ad.transform(result.node());
  Context::current()->exit();
  Context::current()->optimization(oldOptimization);
  optimize(result); 

  // Clone once again with variables replaced 
  // This prevents the same variables representing different things (especially important
  // for output variables, if at some point diff(a, x) and diff(a,y) are combined for example)
  result = result.clone(true);
  return result;
}


}
