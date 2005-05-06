// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <algorithm>
#include <map>
#include <list>
#include "ShSyntax.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariableNode.hpp"
#include "ShInternals.hpp"
#include "ShInstructions.hpp"
#include "ShEval.hpp"
#include "ShTransformer.hpp"

// #define SH_DEBUG_TYPECONVERT

// @file ShTypeConvertTransformer.cpp
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
//   Add a replacement to an ShVarMap
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
// 5) Run a ShVariableReplacer to replace old non-float variables
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
  FloatConverter(ShTransformer::ValueTypeMap &valueTypeMap, ShVarMap &converts)
    : m_valueTypeMap(valueTypeMap), m_converts(converts), m_eval(ShEval::instance())
  {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    
    ShBasicBlock::ShStmtList::iterator I;
    for(I = block->begin(); I != block->end(); ++I) {
      fixStatement(block->m_statements, I);  
    }
  }

  // @todo might want to make these more global (perhaps even
  // put it into the TypeInfo for standard types?)
  bool isFloat(ShValueType valueType) {
    return shIsFloat(valueType) && shIsRegularValueType(valueType);
  }

  bool isInt(ShValueType valueType) {
    return shIsInteger(valueType) && shIsSigned(valueType);
  }

  // @todo not implemented yet 
  bool isUint(ShValueType valueType) {
    return shIsInteger(valueType) && !shIsSigned(valueType);
  }

  // @todo not implemented yet 
  bool isFrac(ShValueType valueType) {
    return shIsFraction(valueType) && shIsSigned(valueType);
  }

  bool isUfrac(ShValueType valueType) {
    return shIsFraction(valueType) && !shIsSigned(valueType);
  }

  // flags to apply following operations (in order) 
  static const unsigned int APPLY_MAX_1  = 0x01; // clamp >= -1
  static const unsigned int APPLY_MAX0   = 0x02; // clamp >= 0
  static const unsigned int APPLY_MIN1   = 0x04; // clamp <= 1
  static const unsigned int APPLY_FLR    = 0x08; // take floor

  // inserts conversion code before m_I in m_curList that takes the given var 
  // and converts type from fromType to toType and puts this in
  // result ShVariable             
  //
  // Instead of using var's variable node in these operations,
  // it uses newvarNode (this may in fact be the same as var->node()) 
  //
  // var and result should have the converted types already,
  // but fromType and toType are the original types requested for the statement.
  //
  void insertConversion(ShBasicBlock::ShStmtList &stmtList, const ShBasicBlock::ShStmtList::iterator &I,
      const ShVariable &var, ShValueType fromType, const ShVariable &result, ShValueType toType,
      unsigned int forced = 0)
  {
    unsigned int operations = forced;
    if(isFloat(toType)) {
    } else if(isInt(toType)) {
      if(!(isInt(fromType) || isUint(fromType))) {
        operations |= APPLY_FLR;
      }
    } else if(isUint(toType)) {
      if(isFloat(fromType) || isInt(fromType) || isFrac(fromType)) {
        operations |= APPLY_MAX0; 
      }
      operations |= APPLY_FLR;
    } else if(isFrac(toType)) {
      if (isFloat(fromType) || isInt(fromType)) {
        operations |= APPLY_MAX_1;
        operations |= APPLY_MIN1;
      } else if (isUint(fromType)) {
        operations |= APPLY_MIN1;
      }
    } else if(isUfrac(toType)) {
      if (isFloat(fromType) || isInt(fromType)) {
        operations |= APPLY_MAX0;
        operations |= APPLY_MIN1;
      } else if (isUint(fromType)) {
        operations |= APPLY_MIN1;
      }
    }
    // @todo make sure to run make another temp in case
    // one of var/result is an IN/OUT and hence cannot be used in computation 
    ShVariable temp(var.node()->clone(SH_TEMP, var.size(), SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false, false));

    stmtList.insert(I, ShStatement(temp, SH_OP_ASN, var));

    if(operations & APPLY_MAX_1) {
      ShVariable one(var.node()->clone(SH_CONST, 1, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false, false));
      one.setVariant(shVariantFactory(var.valueType())->generateOne());
      stmtList.insert(I, ShStatement(temp, temp, SH_OP_MAX, one)); 
    }

    if(operations & APPLY_MAX0)  {
      ShVariable zero(var.node()->clone(SH_CONST, 1, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false, false));
      zero.setVariant(shVariantFactory(var.valueType())->generateZero());
      stmtList.insert(I, ShStatement(temp, temp, SH_OP_MAX, zero)); 
    }

    if(operations & APPLY_MIN1)  {
      ShVariable one(var.node()->clone(SH_CONST, 1, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false, false));
      one.setVariant(shVariantFactory(var.valueType())->generateOne());
      stmtList.insert(I, ShStatement(temp, temp, SH_OP_MIN, one)); 
    }

    if(operations & APPLY_FLR) stmtList.insert(I, ShStatement(temp, SH_OP_FLR, temp)); 

    stmtList.insert(I, ShStatement(result, SH_OP_ASN, temp));

    if((operations & APPLY_FLR) && (operations - APPLY_FLR)) {
#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("Unhandled conversion operations");
#endif
    }
  }

  // Adds required conversions for statment *I
  void fixStatement(ShBasicBlock::ShStmtList &stmtList, const ShBasicBlock::ShStmtList::iterator &I) {
    ShStatement &stmt = *I;
    if(stmt.dest.null()) return;
#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Checking a statement op=" << opInfo[stmt.op].name);
#endif

    const ShEvalOpInfo* evalOpInfo; 
    ShEvalOpInfo* texInfo = 0;
    switch(stmt.op) {
      case SH_OP_TEX:
      case SH_OP_TEXI:
      case SH_OP_FETCH:
      case SH_OP_TEXD:
        // @todo type think of a cleaner solution.
        // - we probably shouldn't be using the host-side set of operations
        // anyway - maybe use a functor given by the backend that decides
        // which types to use for a given set of src/dest types 
        
        // temporary hack - make an evalop for this
        evalOpInfo = texInfo = new ShEvalOpInfo(stmt.op, 0, stmt.dest.valueType(), stmt.src[0].valueType(), stmt.src[1].valueType(), stmt.src[2].valueType());
        break;
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
        case SH_OP_TEX:
        case SH_OP_TEXI:
        case SH_OP_FETCH:
        case SH_OP_TEXD:
        case SH_OP_KIL:
          // @todo type think of a cleaner solution.
          // - we probably shouldn't be using the host-side set of operations
          // anyway - maybe use a functor given by the backend that decides
          // which types to use for a given set of src/dest types 
          
          // temporary hack - make an evalop for this
          evalOpInfo = texInfo = new ShEvalOpInfo(stmt.op, 0, stmt.dest.valueType(), stmt.src[0].valueType(), stmt.src[1].valueType(), stmt.src[2].valueType());
          break;

        default:
          // It could also be that no operation matched the arguments
          SH_DEBUG_PRINT("Possible problem finding evaluator for op = " << opInfo[stmt.op].name); 
          return;
      }
    }


    for(int i = 0; i < 3; ++i) {
      ShValueType srcValueType = stmt.src[i].valueType();
      ShValueType opValueType = evalOpInfo->m_src[i]; 
      if(srcValueType == opValueType) continue;
      if((m_valueTypeMap.count(srcValueType) == 0) && (m_valueTypeMap.count(opValueType) == 0)) continue;

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting src[" << i << "] from " << shTypeInfo(srcValueType)->name()
          << " to " << shTypeInfo(opValueType)->name());
#endif

      // Step 2: prepend code to convert src[i], making sure all variables
      // involved are converted if their type is in m_valueTypeMap
      ShValueType tempValueType = opValueType;
      if(m_valueTypeMap.count(tempValueType) > 0) tempValueType = m_valueTypeMap[tempValueType];
      ShVariable temp(stmt.src[i].node()->clone(SH_TEMP, stmt.src[i].size(), tempValueType, SH_SEMANTICTYPE_END, false, false));

      ShVariableNodePtr varNode = stmt.src[i].node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      ShVariable newsrc(varNode, stmt.src[i].swizzle(), stmt.src[i].neg());

      insertConversion(stmtList, I, newsrc, srcValueType, temp, opValueType); 

      stmt.src[i] = temp;
    }

    // Step 3: append code to convert dest 
    //
    // @todo type - for certain ops, doing them in float doesn't give the same
    // result as non-float, so the output needs to be fixed...
    //
    // @todo in particular, for int, DIV, POW need flooring afterwards 
    ShValueType destValueType = stmt.dest.valueType();
    ShValueType opDest = evalOpInfo->m_dest;
    if((destValueType != opDest) &&
       (m_valueTypeMap.count(destValueType) + m_valueTypeMap.count(opDest) > 0)) {

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting dest from " << shTypeInfo(opDest)->name() << " to " << shTypeInfo(destValueType)->name() );
#endif

      ShBasicBlock::ShStmtList::iterator afterI = I;
      ++afterI;

      ShValueType tempValueType = opDest;
      if(m_valueTypeMap.count(tempValueType) > 0) tempValueType = m_valueTypeMap[tempValueType];
      ShVariable temp(stmt.dest.node()->clone(SH_TEMP, stmt.dest.size(), tempValueType, SH_SEMANTICTYPE_END, false, false));

      ShVariableNodePtr varNode = stmt.dest.node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      ShVariable newdest(varNode, stmt.dest.swizzle(), stmt.dest.neg()); 

      // @todo type check for other special cases
      unsigned int forcedOps = 0;
      if((stmt.op == SH_OP_DIV || stmt.op == SH_OP_POW) && 
         (isInt(destValueType) || isUint(destValueType))) {
        forcedOps |= APPLY_FLR; 
      }

      insertConversion(stmtList, afterI, temp, opDest, newdest, destValueType, forcedOps); 

      stmt.dest = temp;
    }

    if(texInfo) delete texInfo;
  }

  // Adds a conversion for p into the m_converts map 
  void operator()(const ShVariableNodePtr &p) {
    // check if done or conversion not necessary
    if(m_converts.count(p) > 0) return; 
    if(m_valueTypeMap.count(p->valueType()) == 0) return; 
    ShVariableNodePtr &converted_p = m_converts[p] 
      = p->clone(SH_BINDINGTYPE_END, 0, m_valueTypeMap[p->valueType()], SH_SEMANTICTYPE_END, false);

    if(p->hasValues()) {
      converted_p->setVariant(p->getVariant());

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("Setting values on replacement = " << converted_p->getVariant()->encode() << " original = " << p->getVariant()->encode());
#endif
    }
    
    if(p->uniform()) { // @todo set up dependent uniform
      ShProgram prg = SH_BEGIN_PROGRAM("uniform") {
        ShVariable original(p);
        ShVariable converted(converted_p);
        shASN(converted, original);
      } SH_END;
      converted_p->attach(prg.node());
    }

#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Converting " << p->name() << " from " << shTypeInfo(p->valueType())->name()
      << " to " << shTypeInfo(m_valueTypeMap[p->valueType()])->name()); 
#endif
  }


  ShTransformer::ValueTypeMap &m_valueTypeMap;
  ShVarMap &m_converts;

  private:
    ShEval* m_eval;
};

void ShTransformer::convertToFloat(ValueTypeMap &valueTypeMap)
{
  ShVarMap converts;
  FloatConverter floatconv(valueTypeMap, converts);

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
  ShVariableReplacer vr(converts);
  m_program->ctrlGraph->dfs(vr);
  vr(m_program->inputs);
  vr(m_program->outputs);

  if(!converts.empty()) m_changed = true;
}

}

