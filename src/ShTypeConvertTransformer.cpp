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
#include <algorithm>
#include <map>
#include <list>
#include "ShContext.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShVariableNode.hpp"
#include "ShInternals.hpp"
#include "ShEval.hpp"
#include "ShTransformer.hpp"

// #define SH_DEBUG_TYPECONVERT

namespace SH {

// algorithm
// Converts non-float types into floats (for standard Sh types)
//
// @todo allow backend to specify which float format to convert to.
// eg. short integers may be fine as half-floats for platforms that
// support that.
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
// or to is a key in m_typeMap 
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
  FloatConverter(ShTransformer::TypeIndexMap &typeMap, ShVarMap &converts)
    : m_typeMap(typeMap), m_converts(converts), m_eval(ShEval::instance())
  {
    m_floatIndices.push_back(shTypeIndex<double>());
    m_floatIndices.push_back(shTypeIndex<float>());

    m_intIndices.push_back(shTypeIndex<int>());
    m_intIndices.push_back(shTypeIndex<short>());
    m_intIndices.push_back(shTypeIndex<char>());
    
    //m_uintIndices.push_back(shTypeIndex<unsigned int>());
    //m_uintIndices.push_back(shTypeIndex<unsigned short>());
    //m_uintIndices.push_back(shTypeIndex<unsigned char>());
    
    //@todo frac types
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
  bool isFloat(int typeIndex) {
    return std::find(m_floatIndices.begin(), m_floatIndices.end(), typeIndex) != m_floatIndices.end();
  }

  bool isInt(int typeIndex) {
    return std::find(m_intIndices.begin(), m_intIndices.end(), typeIndex) != m_intIndices.end();
  }

  // @todo not implemented yet 
  bool isUint(int typeIndex) {
    //return std::find(m_uintIndices.begin(), m_uintIndices.end(), typeIndex) != m_uintIndices.end();
    return false;
  }

  // @todo not implemented yet 
  bool isFrac(int typeIndex) {
    //return std::find(m_fracIndices.begin(), m_fracIndices.end(), typeIndex) != m_fracIndices.end();
    return false;
  }

  bool isUfrac(int typeIndex) {
    //return std::find(m_ufracIndices.begin(), m_fracIndices.end(), typeIndex) != m_fracIndices.end();
    return false;
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
  void insertConversion(ShBasicBlock::ShStmtList &stmtList, const ShBasicBlock::ShStmtList::iterator &I,
      const ShVariable &var, int fromType, const ShVariable &result, int toType,
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
      // @todo
      SH_DEBUG_PRINT("Conversion to fractional types not implemented yet.");
    } else if(isUfrac(toType)) {
      // @todo
      SH_DEBUG_PRINT("Conversion to unsigned fractional types not implemented yet.");
    }
    // @todo make sure to run make another temp in case
    // one of var/result is an IN/OUT and hence cannot be used in computation 
    ShVariable temp(var.node()->clone(SH_TEMP, SH_ATTRIB, var.size(), false, false));

    stmtList.insert(I, ShStatement(temp, SH_OP_ASN, var));
    //if(operations & APPLY_MAX_1) stmtList.insert(I, ShStatement(temp, SH_OP_FLR, temp)); 
    //if(operations & APPLY_MAX0)  stmtList.insert(I, ShStatement(temp, SH_OP_FLR, temp)); 
    //if(operations & APPLY_MIN1)  stmtList.insert(I, ShStatement(temp, SH_OP_FLR, temp)); 
    if(operations & APPLY_FLR) stmtList.insert(I, ShStatement(temp, SH_OP_FLR, temp)); 

    stmtList.insert(I, ShStatement(result, SH_OP_ASN, temp));

    if((operations & APPLY_FLR) && (operations - APPLY_FLR)) {
      SH_DEBUG_PRINT("Unhandled conversion operations");
    }
  }

  // Adds required conversions for statment *I
  void fixStatement(ShBasicBlock::ShStmtList &stmtList, const ShBasicBlock::ShStmtList::iterator &I) {
    ShStatement &stmt = *I;
#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Checking a statement op=" << opInfo[stmt.op].name);
#endif

    // Get the operation information used for this statement 
    const ShEvalOpInfo* evalOpInfo = m_eval->getEvalOpInfo(
        stmt.op,
        stmt.dest.typeIndex(), 
        stmt.src[0].typeIndex(),
        stmt.src[1].typeIndex(),
        stmt.src[2].typeIndex());

    if(!evalOpInfo) {
      // @todo type 
      // for now, assume its one of the instructions that has no immediate mode functionality (e.g. kill, texture lookup)
      // It could also be that no operation matched the arguments
      SH_DEBUG_PRINT("Possible problem finding evaluator for op = " << opInfo[stmt.op].name); 
      return;
    }

    for(int i = 0; i < 3; ++i) {
      int srcIndex = stmt.src[i].typeIndex();
      int opIndex = evalOpInfo->m_src[i]; 
      if(srcIndex == opIndex) continue;
      if((m_typeMap.count(srcIndex) == 0) && (m_typeMap.count(opIndex) == 0)) continue;

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting src[" << i << "] from " << shTypeInfo(srcIndex)->name()
          << " to " << shTypeInfo(opIndex)->name());
#endif

      // Step 2: prepend code to convert src[i], making sure all variables
      // involved are converted if their type is in m_typeMap
      int tempIndex = opIndex;
      if(m_typeMap.count(tempIndex) > 0) tempIndex = m_typeMap[tempIndex];
      ShVariable temp(stmt.src[i].node()->clone(SH_TEMP, stmt.src[i].size(), tempIndex, false, false));

      ShVariableNodePtr varNode = stmt.src[i].node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      ShVariable newsrc(varNode, stmt.src[i].swizzle(), stmt.src[i].neg());

      insertConversion(stmtList, I, newsrc, srcIndex, temp, opIndex); 

      stmt.src[i] = temp;
    }

    // Step 3: append code to convert dest 
    //
    // @todo type - for certain ops, doing them in float doesn't give the same
    // result as non-float, so the output needs to be fixed...
    //
    // @todo in particular, for int, DIV, POW need flooring afterwards 
    int destIndex = stmt.dest.typeIndex();
    int opDest = evalOpInfo->m_dest;
    if((destIndex != opDest) &&
       (m_typeMap.count(destIndex) + m_typeMap.count(opDest) > 0)) {

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("  Converting dest from " << shTypeInfo(opDest)->name() << " to " << shTypeInfo(destIndex)->name() );
#endif

      ShBasicBlock::ShStmtList::iterator afterI = I;
      ++afterI;

      int tempIndex = opDest;
      if(m_typeMap.count(tempIndex) > 0) tempIndex = m_typeMap[tempIndex];
      ShVariable temp(stmt.dest.node()->clone(SH_TEMP, stmt.dest.size(), tempIndex, false, false));

      ShVariableNodePtr varNode = stmt.dest.node();
      if(m_converts.count(varNode) > 0) varNode = m_converts[varNode];
      ShVariable newdest(varNode, stmt.dest.swizzle(), stmt.dest.neg()); 

      // @todo type check for other special cases
      unsigned int forcedOps = 0;
      if(stmt.op == SH_OP_DIV || stmt.op == SH_OP_POW) forcedOps |= APPLY_FLR; 

      insertConversion(stmtList, afterI, temp, opDest, newdest, destIndex, forcedOps); 

      stmt.dest = temp;
    }
  }

  // Adds a conversion for p into the m_converts map 
  void operator()(const ShVariableNodePtr &p) {
    // check if done or conversion not necessary
    if(m_converts.count(p) > 0) return; 
    if(m_typeMap.count(p->typeIndex()) == 0) return; 
    m_converts[p] = p->clone(m_typeMap[p->typeIndex()], false);
    if(p->hasValues()) {
      m_converts[p]->setVariant(p->getVariant());

#ifdef SH_DEBUG_TYPECONVERT
      SH_DEBUG_PRINT("Setting values on replacement = " << m_converts[p]->getVariant()->encode() << " original = " << p->getVariant()->encode());
#endif
    }

#ifdef SH_DEBUG_TYPECONVERT
    SH_DEBUG_PRINT("Converting " << p->name() << " from " << shTypeInfo(p->typeIndex())->name()
      << " to " << shTypeInfo(m_typeMap[p->typeIndex()])->name()); 
#endif
  }


  ShTransformer::TypeIndexMap &m_typeMap;
  ShVarMap &m_converts;

  private:
    ShEval* m_eval;
    typedef std::vector<int> TypeIndexSet; 

    TypeIndexSet m_floatIndices;
    TypeIndexSet m_intIndices;
    //TypeIndexset m_uintIndices;
    //TypeIndexset m_fracIndices;
    //TypeIndexset m_ufracIndices;
};

void ShTransformer::convertToFloat(TypeIndexMap &typeMap, ShVarMap &converts)
{
  FloatConverter floatconv(typeMap, converts);

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

