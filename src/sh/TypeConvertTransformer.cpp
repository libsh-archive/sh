// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <map>
#include <list>
#include "Syntax.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "TypeInfo.hpp"
#include "VariableNode.hpp"
#include "Internals.hpp"
#include "Instructions.hpp"
#include "Eval.hpp"
#include "Transformer.hpp"
#include "BaseTexture.hpp"

// #define SH_DEBUG_TYPECONVERT

// @file TypeConvertTransformer.cpp
//
// Currently does something naive.  Assumes all unsupported types in the backend
// turn into floats, so it is often too conservative.
// (i.e. in NV for fixed-point registers, may not need to do some of the
// clamping of operation results, etc.)
//
// @todo type may want think about leaving fi and fui types for NV backend to indicate 
// using _SAT _SSAT modifiers
//
// @todo type probably want to use a different set of available operators (or
// expand on host operator set) for different backends

namespace SH {

// algorithm
// Converts non-float types into floats (for standard Sh types)
//
// Algorithm: 
// 1) Identify variables of the following types that are not float  
//   SH_INPUT = 0,
//   SH_OUTPUT = 1,
//   SH_INOUT = 2,
//   SH_TEMP = 3,
//   SH_CONST = 4,
//   
//   Add a replacement to an VarMap
//
// This pass should be done by collecting the variables then using the
// FloatConverter as a functor on the variables lists.
//
// 2) Where an input argument to an operation does not match the 
//    input type expected there insert code before the current 
//    statement for a conversion. 
//
//    This code should operate on the float versions of variables,
//    not the original if they have been converted
//
// 3) Generate any code required for conversion of the output
//    and insert code after current statement.
//
//    Output conversion may be required for two reasons
//    a) if the operation done in floating point gives a different
//       answer than the original type of the operation
//    b) if the destination variable or operation's destination type
//       need to be converted
//
//    This should also operate on the float variables.
//
// Conversions in 2&3 should only happen if the type being converted from
// or to is a key in m_valueTypeMap 
//
// (If neither is the case, then assume that the hardware natively 
// supports the types and does the appropriate conversions automatically)
//
// @todo check that this works properly.  This is probably too conservative
// in some cases where a conversion may not be necessary.
//
// 5) Run a VariableReplacer to replace old non-float variables
//    with float ones.
//    (Run it on both the control graph, input list, and output list)
//    
//    
// Assumed floating point representations
//  int - use  
//  uint -   
//  frac - use float, but clamped to [-1,1]
//  ufrac - use float, but clamped to [0,1]
//  
//    Desired Type -->    float   int   uint      frac      ufrac
//    Existing Type      
//    float               X       flr   max0,flr  max_1,1   max0,1 
//    int                 X       X     max0      max_1,1   max0,1 
//    uint                X       X     X         max1      max1 
//    frac                X       flr   max0,flr  X         max0 
//    ufrac               X       flr   flr       X         X
//
// 
// @todo handle textures/streams later
// since they do not need to be converted to float
// and have storage formats that vary more with
// the backend. 
//
//   SH_TEXTURE = 5,
//  
//   SH_STREAM = 
struct FloatConverter {
  FloatConverter(Transformer::ValueTypeMap &valueTypeMap, VarMap &converts, bool preserve_casts)
    : m_valueTypeMap(valueTypeMap), m_converts(converts), m_eval(Eval::instance()), m_preserve_casts(preserve_casts)
  {
  }

  // assignment operator could not be generated: declaration only
  FloatConverter& operator=(FloatConverter const&);

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    
    BasicBlock::StmtList::iterator I;

#ifdef SH_DEBUG_TYPECONVERT
    for(I = block->begin(); I != block->end(); ++I) {
      std::cout << "FloatConv Before [" << *I << "]" << std::endl;
      if (! I->dest.null()) {
        std::cout << "  DestType: " << typeInfo(I->dest.valueType())->name() << std::endl;
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        std::cout << "  SrcType[" << i << "]: " << typeInfo(I->src[i].valueType())->name() << std::endl;
      }
    }
#endif

    for(I = block->begin(); I != block->end(); ++I) {
      fixStatement(block->m_statements, I);  
    }

#ifdef SH_DEBUG_TYPECONVERT
    for(I = block->begin(); I != block->end(); ++I) {
      std::cout << "FloatConv After [" << *I << "]" << std::endl;
      if (! I->dest.null()) {
        std::cout << "  DestType: " << typeInfo(I->dest.valueType())->name() << std::endl;
      }
      for (int i = 0; i < opInfo[I->op].arity; i++) {
        std::cout << "  SrcType[" << i << "]: " << typeInfo(I->src[i].valueType())->name() << std::endl;
      }
    }
#endif
  }

  // @todo might want to make these more global (perhaps even
  // put it into the TypeInfo for standard types?)
  bool is_float(ValueType valueType) {
    return isFloat(valueType) && isRegularValueType(valueType);
  }

  bool is_int(ValueType valueType) {
    return isInteger(valueType) && isSigned(valueType);
  }

  // @todo not implemented yet 
  bool is_uint(ValueType valueType) {
    return isInteger(valueType) && !isSigned(valueType);
  }

  // @todo not implemented yet 
  bool is_frac(ValueType valueType) {
    return isFraction(valueType) && isSigned(valueType);
  }

  bool is_ufrac(ValueType valueType) {
    return isFraction(valueType) && !isSigned(valueType);
  }

  // flags to apply following operations (in order) 
  static const unsigned int APPLY_MAX_1  = 0x01; // clamp >= -1
  static const unsigned int APPLY_MAX0   = 0x02; // clamp >= 0
  static const unsigned int APPLY_MIN1   = 0x04; // clamp <= 1
  static const unsigned int APPLY_FLR    = 0x08; // take floor

  // inserts conversion code before m_I in m_curList that takes the given var 
  // and converts type from fromType to toType and puts this in
  // result Variable             
  //
  // Instead of using var's variable node in these operations,
  // it uses newvarNode (this may in fact be the same as var->node()) 
  //
  // var and result should have the converted types already,
  // but fromType and toType are the original types requested for the statement.
  //
  void insertConversion(BasicBlock::StmtList &stmtList, const BasicBlock::StmtList::iterator &I,
      const Variable &var, ValueType fromType, const Variable &result, ValueType toType,
      unsigned int forced = 0)
  {
    unsigned int operations = forced;
    if(is_float(toType)) {
    } else if(is_int(toType)) {
      if(!(is_int(fromType) || is_uint(fromType))) {
        operations |= APPLY_FLR;
      }
    } else if(is_uint(toType)) {
      if(is_float(fromType) || is_int(fromType) || is_frac(fromType)) {
        operations |= APPLY_MAX0; 
      }
      operations |= APPLY_FLR;
    } else if(is_frac(toType)) {
      if (is_float(fromType) || is_int(fromType)) {
        operations |= APPLY_MAX_1;
        operations |= APPLY_MIN1;
      } else if (is_uint(fromType)) {
        operations |= APPLY_MIN1;
      }
    } else if(is_ufrac(toType)) {
      if (is_float(fromType) || is_int(fromType)) {
        operations |= APPLY_MAX0;
        operations |= APPLY_MIN1;
      } else if (is_uint(fromType)) {
        operations |= APPLY_MIN1;
      }
    }

    if (operations) {
      // make another temp in case one of var/result is an IN/OUT and hence cannot be used in computation 
      Variable temp(var.node()->clone(SH_TEMP, var.size(), VALUETYPE_END, SEMANTICTYPE_END, false, false));

      stmtList.insert(I, Statement(temp, OP_ASN, var));

      if(operations & APPLY_MAX_1) {
        Variable one(var.node()->clone(SH_CONST, 1, VALUETYPE_END, SEMANTICTYPE_END, false, false));
        one.setVariant(variantFactory(var.valueType())->generateOne());
        stmtList.insert(I, Statement(temp, temp, OP_MAX, one)); 
      }

      if(operations & APPLY_MAX0)  {
        Variable zero(var.node()->clone(SH_CONST, 1, VALUETYPE_END, SEMANTICTYPE_END, false, false));
        zero.setVariant(variantFactory(var.valueType())->generateZero());
        stmtList.insert(I, Statement(temp, temp, OP_MAX, zero)); 
      }

      if(operations & APPLY_MIN1)  {
        Variable one(var.node()->clone(SH_CONST, 1, VALUETYPE_END, SEMANTICTYPE_END, false, false));
        one.setVariant(variantFactory(var.valueType())->generateOne());
        stmtList.insert(I, Statement(temp, temp, OP_MIN, one)); 
      }

      if(operations & APPLY_FLR) stmtList.insert(I, Statement(temp, OP_FLR, temp)); 

      if((operations & APPLY_FLR) && (operations - APPLY_FLR)) {
#ifdef SH_DEBUG_TYPECONVERT
        SH_DEBUG_PRINT("Unhandled conversion operations");
#endif
      }

      stmtList.insert(I, Statement(result, OP_ASN, temp));
    } else {
      stmtList.insert(I, Statement(result, OP_ASN, var));
    }
  }

  // Adds required conversions for statment *I
  void fixStatement(BasicBlock::StmtList &stmtList, const BasicBlock::StmtList::iterator &I) {
    Statement &stmt = *I;
    if(stmt.dest.null()) return;
#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Checking a statement op=" << opInfo[stmt.op].name);
#endif

    const EvalOpInfo* evalOpInfo; 
    EvalOpInfo* texInfo = 0;
    switch(stmt.op) {
      case OP_TEX:
      case OP_TEXI:
      case OP_FETCH:
      case OP_TEXD:
      case OP_DX:
      case OP_DY:
        // @todo type think of a cleaner solution.
        // - we probably shouldn't be using the host-side set of operations
        // anyway - maybe use a functor given by the backend that decides
        // which types to use for a given set of src/dest types 
        
        // temporary hack - make an evalop for this
        evalOpInfo = texInfo = new EvalOpInfo(stmt.op, 0, stmt.dest.valueType(), stmt.src[0].valueType(), stmt.src[1].valueType(), stmt.src[2].valueType());
        break;
      case OP_ASN: // FIXME cheap hack to ignore the casts we add... don't deal with assignments
        if (m_preserve_casts) return;
        // FALL THROUGH
      default:
        evalOpInfo = m_eval->getEvalOpInfo(
            stmt.op,
            stmt.dest.valueType(), 
            stmt.src[0].valueType(),
            stmt.src[1].valueType(),
            stmt.src[2].valueType());
        break;
    }

    if(!evalOpInfo) {
      switch(stmt.op) {
        case OP_TEX:
        case OP_TEXI:
        case OP_FETCH:
        case OP_TEXD:
        case OP_KIL:
        case OP_RET:
          // @todo type think of a cleaner solution.
          // - we probably shouldn't be using the host-side set of operations
          // anyway - maybe use a functor given by the backend that decides
          // which types to use for a given set of src/dest types 
          
          // temporary hack - make an evalop for this
          evalOpInfo = texInfo = new EvalOpInfo(stmt.op, 0, stmt.dest.valueType(), stmt.src[0].valueType(), stmt.src[1].valueType(), stmt.src[2].valueType());
          break;

        default:
          // It could also be that no operation matched the arguments
          SH_DEBUG_PRINT("Possible problem finding evaluator for op = " << opInfo[stmt.op].name); 
          return;
      }
    }


    for(int i = 0; i < 3; ++i) {
      ValueType srcValueType = stmt.src[i].valueType();
      ValueType opValueType = evalOpInfo->m_src[i]; 
      if(srcValueType == opValueType) continue;
      if((m_valueTypeMap.count(srcValueType) == 0) && (m_valueTypeMap.count(opValueType) == 0)) continue;

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting src[" << i << "] from " << typeInfo(srcValueType)->name()
          << " to " << typeInfo(opValueType)->name());
#endif

      // Step 2: prepend code to convert src[i], making sure all variables
      // involved are converted if their type is in m_valueTypeMap
      ValueType tempValueType = opValueType;
      if(m_valueTypeMap.count(tempValueType) > 0) tempValueType = m_valueTypeMap[tempValueType];
      Variable temp(stmt.src[i].node()->clone(SH_TEMP, stmt.src[i].size(), tempValueType, SEMANTICTYPE_END, false, false));

      VariableNodePtr varNode = stmt.src[i].node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      Variable newsrc(varNode, stmt.src[i].swizzle(), stmt.src[i].neg());

      insertConversion(stmtList, I, newsrc, srcValueType, temp, opValueType); 

      stmt.src[i] = temp;
    }

    // Step 3: append code to convert dest 
    //
    // @todo type - for certain ops, doing them in float doesn't give the same
    // result as non-float, so the output needs to be fixed...
    //
    // @todo in particular, for int, DIV, POW need flooring afterwards 
    ValueType destValueType = stmt.dest.valueType();
    ValueType opDest = evalOpInfo->m_dest;
    if((destValueType != opDest) &&
       (m_valueTypeMap.count(destValueType) + m_valueTypeMap.count(opDest) > 0)) {

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting dest from " << typeInfo(opDest)->name() << " to " << typeInfo(destValueType)->name() );
#endif

      BasicBlock::StmtList::iterator afterI = I;
      ++afterI;

      ValueType tempValueType = opDest;
      if(m_valueTypeMap.count(tempValueType) > 0) tempValueType = m_valueTypeMap[tempValueType];
      Variable temp(stmt.dest.node()->clone(SH_TEMP, stmt.dest.size(), tempValueType, SEMANTICTYPE_END, false, false));

      VariableNodePtr varNode = stmt.dest.node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      Variable newdest(varNode, stmt.dest.swizzle(), stmt.dest.neg()); 

      // @todo type check for other special cases
      unsigned int forcedOps = 0;
      if((stmt.op == OP_DIV || stmt.op == OP_POW) && 
         (is_int(destValueType) || is_uint(destValueType))) {
        forcedOps |= APPLY_FLR; 
      }

      insertConversion(stmtList, afterI, temp, opDest, newdest, destValueType, forcedOps); 

      stmt.dest = temp;
    }

    if(texInfo) delete texInfo;
  }

  // Adds a conversion for p into the m_converts map 
  void operator()(const VariableNodePtr &p) {
    // check if done or conversion not necessary
    if(m_converts.count(p) > 0) return; 
    if(m_valueTypeMap.count(p->valueType()) == 0) return; 
    VariableNodePtr &converted_p = m_converts[p] 
      = p->clone(BINDINGTYPE_END, 0, m_preserve_casts ? p->valueType() : m_valueTypeMap[p->valueType()], SEMANTICTYPE_END, false);

    if(p->hasValues()) {
      converted_p->setVariant(p->getVariant());

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("Setting values on replacement = " << converted_p->getVariant()->encode() << " original = " << p->getVariant()->encode());
#endif
    }
    
    if(p->uniform()) {
      Program prg = SH_BEGIN_PROGRAM("uniform") {
        Variable original(p);
        Variable converted(converted_p->clone(SH_OUTPUT, 0, VALUETYPE_END,
                                              SEMANTICTYPE_END, true, false));
        shASN(converted, original);
      } SH_END;
      converted_p->attach(prg.node());
    }

#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Converting " << p->name() << " from " << typeInfo(p->valueType())->name()
      << " to " << typeInfo(m_valueTypeMap[p->valueType()])->name()); 
#endif
  }


  Transformer::ValueTypeMap &m_valueTypeMap;
  VarMap &m_converts;

  private:
    Eval* m_eval;
    bool m_preserve_casts;
};

void Transformer::convertToFloat(ValueTypeMap &valueTypeMap, bool preserve_casts)
{
  VarMap converts;
  FloatConverter floatconv(valueTypeMap, converts, preserve_casts);

  // Step 1
  m_program->collectVariables(); 
  std::for_each(m_program->inputs.begin(), m_program->inputs.end(), floatconv); 
  std::for_each(m_program->outputs.begin(), m_program->outputs.end(), floatconv); 
  std::for_each(m_program->temps.begin(), m_program->temps.end(), floatconv); 
  std::for_each(m_program->constants.begin(), m_program->constants.end(), floatconv); 
  std::for_each(m_program->uniforms.begin(), m_program->uniforms.end(), floatconv); 

  // Steps 2-3
  m_program->ctrlGraph->dfs(floatconv);

  // Step 4
  VariableReplacer vr(converts);
  m_program->ctrlGraph->dfs(vr);
  vr(m_program->inputs);
  vr(m_program->outputs);

  if(!converts.empty()) m_changed = true;
}

}

