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
#include <fstream>
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariable.hpp"
#include "ShVariableNode.hpp"
#include "ShRecord.hpp"
#include "ShInternals.hpp"
#include "ShSyntax.hpp"
#include "ShProgram.hpp"
#include "ShInstructions.hpp"
#include "ShTransformer.hpp"
#include "ShAlgebra.hpp"
#include "ShManipulator.hpp"
#include "ShCtrlGraphWranglers.hpp"
#include "ShOptimizations.hpp"
#include "ShRangeBranchFixer.hpp"

//#define SH_DEBUG_ICONV
namespace {

using namespace SH;

struct ShVariablePair {
  ShVariable lo, hi;
  ShVariablePair(ShBindingType Binding, int N, ShValueType V) 
    : lo(new ShVariableNode(Binding, N, V)),
      hi(new ShVariableNode(Binding, N, V)) 
  {}

  ShRecord record() 
  {
    return (lo & hi);
  }
};


// A set of programs that does interval arithmetic operations, except 
// on 2N-tuples of type T instead of a pair of lo/hi N-tuples
ShProgram intervalASN(int N, ShValueType valueType)
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INOUT, N, valueType);
  } SH_END;
  return result;
}

template<ShOperation OP>
ShProgram intervalBinaryMonotonic(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(
        ShStatement(r.lo, a.lo, OP, b.lo));
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(
        ShStatement(r.hi, a.hi, OP, b.hi));
  } SH_END;
  return result;
}


ShProgram intervalMUL(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    ShVariable ll(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable lh(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable hl(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable hh(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable temp(new ShVariableNode(SH_TEMP, N, valueType));

    shMUL(ll, a.lo, b.lo);
    shMUL(lh, a.lo, b.hi);
    shMUL(hl, a.hi, b.lo);
    shMUL(hh, a.hi, b.hi);

    shMIN(temp, ll, lh);
    shMIN(r.lo, hl, hh);
    shMIN(r.lo, r.lo, temp);

    shMAX(temp, ll, lh);
    shMAX(r.hi, hl, hh);
    shMAX(r.hi, r.hi, temp);
  } SH_END;
  return result;
}

ShProgram intervalRCP(int N, ShValueType valueType); // forward declaration

ShProgram intervalDIV(int N, ShValueType valueType)
{
  ShProgram mul = intervalMUL(N, valueType);
  ShProgram rcp = intervalRCP(N, valueType);
  ShProgram result = mul << rcp; 
  result = result << shSwizzle(2, 3, 0, 1); 
  return result;
}

ShProgram intervalSLT(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shSLT(r.lo, a.hi, b.lo);
    shSGT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

ShProgram intervalSLE(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shSLE(r.lo, a.hi, b.lo);
    shSGT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

ShProgram intervalSGT(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shSGT(r.lo, a.hi, b.lo);
    shSLT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

ShProgram intervalSGE(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shSGE(r.lo, a.hi, b.lo);
    shSLT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

ShProgram intervalCSUM(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, 1, valueType);

    shCSUM(r.lo, a.lo);
    shCSUM(r.hi, a.hi);
  } SH_END;
  return result;
}

ShProgram intervalDOT(int N, ShValueType valueType) 
{
  ShProgram mul = intervalMUL(N, valueType);
  ShProgram csum = intervalCSUM(N, valueType);
  return csum << mul; 
}

ShProgram intervalFRAC(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);
    ShVariable temp(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariablePair frac_a(SH_TEMP, N, valueType);

    // if floor(a.lo) is not floor(a.hi),  
    shFLR(temp, a.lo); 
    shADD(temp, a.hi, -temp);
    shSGE(temp, temp, ShConstAttrib1f(1.0f).repeat(N));

    shFRAC(frac_a.lo, a.lo);
    shFRAC(frac_a.hi, a.hi);
    shCOND(r.lo, temp, ShConstAttrib1f(0.0f).repeat(N), frac_a.lo);
    shCOND(r.hi, temp, ShConstAttrib1f(1.0f).repeat(N), frac_a.hi);
  } SH_END;
  return result;
}

ShProgram intervalNEG(int N, ShValueType valueType)
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shASN(r.lo, -a.hi);
    shASN(r.hi, -a.lo);
  } SH_END;
  return result;
}

ShProgram intervalMAD(int N, ShValueType valueType) 
{
  ShProgram mul = intervalMUL(N, valueType);
  ShProgram add = intervalBinaryMonotonic<SH_OP_ADD>(N, valueType);
  return add << mul; 
}

ShProgram intervalLRP(int N, ShValueType valueType) 
{
  ShProgram mad = intervalMAD(N, valueType);
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair c(SH_INPUT, N, valueType);
    ShVariablePair bsc(SH_TEMP, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shADD(bsc.lo, b.lo, -c.hi);
    shADD(bsc.hi, b.hi, -c.lo);
    r.record() = mad(a.record() & bsc.record() & c.record());
  } SH_END;
  return result; 
}


ShProgram intervalRCP(int N, ShValueType valueType)
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shRCP(r.lo, a.hi);
    shRCP(r.hi, a.lo);
  } SH_END;
  return result;
}


ShProgram intervalUNION(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shMIN(r.lo, a.lo, b.lo);
    shMAX(r.hi, a.hi, b.hi);
  } SH_END;
  return result;
}

ShProgram intervalISCT(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shMAX(r.lo, a.lo, b.lo);
    shMIN(r.hi, a.hi, b.hi);
  } SH_END;
  return result;
}

// @todo range - do this properly
ShProgram intervalCONTAINS(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariablePair a(SH_INPUT, N, valueType);
    ShVariablePair b(SH_INPUT, N, valueType);
    ShVariablePair r(SH_OUTPUT, N, valueType);

    shSLE(r.lo, a.lo, b.lo);
    shSGE(r.hi, a.hi, b.hi);
    shMUL(r.lo, r.lo, r.hi);
    shASN(r.hi, r.lo);
  } SH_END;
  return result;
}

ShProgram getProgram(ShOperation op, int N, ShValueType valueType) {
  switch(op) {
    case SH_OP_ASN:   return intervalASN(N, valueType);
    case SH_OP_ADD:   return intervalBinaryMonotonic<SH_OP_ADD>(N, valueType);
    case SH_OP_MUL:   return intervalMUL(N, valueType);
    case SH_OP_DIV:   return intervalDIV(N, valueType);

    case SH_OP_SLT:   return intervalSLT(N, valueType);
    case SH_OP_SLE:   return intervalSLE(N, valueType);
    case SH_OP_SGT:   return intervalSGT(N, valueType);
    case SH_OP_SGE:   return intervalSGE(N, valueType);

    case SH_OP_CEIL:   return intervalBinaryMonotonic<SH_OP_CEIL>(N, valueType);
    case SH_OP_CSUM:   return intervalCSUM(N, valueType);
    case SH_OP_DOT:   return intervalDOT(N, valueType);
    case SH_OP_EXP:   return intervalBinaryMonotonic<SH_OP_EXP>(N, valueType);
    case SH_OP_EXP2:   return intervalBinaryMonotonic<SH_OP_EXP2>(N, valueType);
    case SH_OP_EXP10:   return intervalBinaryMonotonic<SH_OP_EXP10>(N, valueType);
    case SH_OP_FLR:   return intervalBinaryMonotonic<SH_OP_FLR>(N, valueType);
    case SH_OP_FRAC:  return intervalFRAC(N, valueType); 
    case SH_OP_LOG:   return intervalBinaryMonotonic<SH_OP_LOG>(N, valueType);
    case SH_OP_LOG2:   return intervalBinaryMonotonic<SH_OP_LOG2>(N, valueType);
    case SH_OP_LOG10:   return intervalBinaryMonotonic<SH_OP_LOG10>(N, valueType);
    case SH_OP_MAD:   return intervalMAD(N, valueType);
    case SH_OP_LRP:   return intervalLRP(N, valueType);
    case SH_OP_MAX:   return intervalBinaryMonotonic<SH_OP_MAX>(N, valueType);
    case SH_OP_MIN:   return intervalBinaryMonotonic<SH_OP_MIN>(N, valueType);
    case SH_OP_RCP:   return intervalRCP(N, valueType);
    case SH_OP_RND:   return intervalBinaryMonotonic<SH_OP_RND>(N, valueType);
    case SH_OP_SGN:   return intervalBinaryMonotonic<SH_OP_SGN>(N, valueType);
    case SH_OP_SQRT:   return intervalBinaryMonotonic<SH_OP_SQRT>(N, valueType);

    case SH_OP_UNION:   return intervalUNION(N, valueType);
    case SH_OP_ISCT:   return intervalISCT(N, valueType);
    case SH_OP_CONTAINS:   return intervalCONTAINS(N, valueType);
    // @todo type add in some more operations
    default:
      SH_DEBUG_PRINT("Unable to translate op: " << opInfo[op].name);
      shError(ShTransformerException(
            "Cannot translate interval arithmetic operator" ));
  }
  return ShProgram();
}

typedef std::map<int, int> ValueTypeMap; 

// Converts operations on ShInterval<T> to operations on T 
// This takes the same form as the variable splitters, but does more complicated
// code splices.
//
// So the first step is to assign new lo/hi tuples corresponding to each
// interval tuple.
//
// Then interval operations are converted to use lo/hi tuples by splicing in
// code fragments using ShProgram objects in ShSplitIntervalProgram 
struct IntervalSplitter {

  IntervalSplitter(ShTransformer::VarSplitMap& splits, bool &changed)
    : splits(splits), changed(changed) {
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      splitVars(*I);
    }
  }

  // this must be called BEFORE running a DFS on the program
  // to split temporaries (otherwise the stupid hack marked (#) does not work)
  void splitVarList(ShProgramNode::VarList &vars) {
    for(ShProgramNode::VarList::iterator I = vars.begin();
        I != vars.end();) {
      if(split(*I)) {
        vars.insert(I, splits[*I].begin(), splits[*I].end());
        I = vars.erase(I); 
      } else ++I;
    }
  }

  ShVariableNodePtr cloneNode(ShVariableNodePtr node, ShValueType rangeValueType) {
    return node->clone(SH_BINDINGTYPE_END, 0, rangeValueType, SH_SEMANTICTYPE_END, false); 
  }
  
  void splitVars(ShStatement& stmt) {
    if(stmt.dest.node()) split(stmt.dest.node()); 
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) split(stmt.src[i].node()); 
  }

  // returns true if variable split
  // does not add variable to Program's VarList, so this must be handled manually 
  // (since this matters only for IN/OUT/INOUT types, splitVarList handles the
  // insertions nicely)
  bool split(ShVariableNodePtr node)
  {
    ShValueType valueType = node->valueType(); 

    if(!shIsInterval(valueType)) return false; 
    else if(splits.count(node) > 0) return true;

    if( node->kind() == SH_TEXTURE || node->kind() == SH_STREAM ) {
      shError( ShTransformerException(
            "Interval Arithmetic support is not implemented for textures or streams"));
    }
    changed = true;

#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("Splitting Variable " << node->nameOfType() << " " << node->name());
#endif

    ShTransformer::VarNodeVec &splitNodes = splits[node];
    ShValueType newValueType = shRegularValueType(valueType);

    splitNodes.push_back(cloneNode(node, newValueType));  // lo
    splitNodes.push_back(cloneNode(node, newValueType));  // hi
    splitNodes[0]->name(node->name() + "_lo");
    splitNodes[1]->name(node->name() + "_hi");

    if(node->hasValues()) { // copy over values
      ShContext::current()->enter(0);
      ShVariable loVar(splitNodes[0]);
      shLO(loVar, ShVariable(node));

      ShVariable hiVar(splitNodes[1]);
      shHI(hiVar, ShVariable(node));
      ShContext::current()->exit();
    }

    if(node->uniform()) { // set up dependent uniform
#ifdef SH_DEBUG_ICONV
      SH_DEBUG_PRINT("Splitting uniform and setting up dependencies for node " << node->name());
#endif
      ShProgram loprg = SH_BEGIN_PROGRAM("uniform") {
        ShVariable original(node);
        ShVariable converted(splitNodes[0]);
        shLO(converted, original);
      } SH_END;
      splitNodes[0]->attach(loprg.node());

      ShProgram hiprg = SH_BEGIN_PROGRAM("uniform") {
        ShVariable original(node);
        ShVariable converted(splitNodes[1]);
        shHI(converted, original);
      } SH_END;
      splitNodes[1]->attach(hiprg.node());
    }

    return true; 
  }

  ShTransformer::VarSplitMap &splits;
  bool& changed;
};

// @todo type handle negations - remove all negations from interval ShVariables 
// since negating the lo/hi doesn't give a valid interval any more 

/** Splits up operations on intervals into separate non-interval ops
 * replaces LO, HI, SETLO, SETHI instructions with appropriatly.
 */
struct IntervalStatementFixer {
  typedef std::vector<ShVariable> VarVec;

  IntervalStatementFixer(bool &changed, ShTransformer::VarSplitMap &splits)
    : changed(changed), splits(splits) {}

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if(splitStatement(I, node)) break;
    }
  }

  bool isSplit(const ShVariable &var) {
    return splits.count(var.node()) > 0;
  }

  // swizzles the source if it is scalar and the dest is a vector
  // This should be used whenever there is a risk that the src.size() !=
  // dest.size()
  ShStatement makeASN(const ShVariable &dest, const ShVariable &src)
  {
    if(dest.size() == src.size()) return ShStatement(dest, SH_OP_ASN, src);

    SH_DEBUG_ASSERT(src.size() == 1);
    int *swiz = new int[dest.size()];
    for(int i = 0; i < dest.size(); ++i) swiz[i] = 0;
    ShStatement result(dest, SH_OP_ASN, src(dest.size(), swiz));
    delete [] swiz;
    return result;
  }

  void doASN(const ShVariable &dest, const ShVariable &src)
  {
    ShContext::current()->parsing()->tokenizer.blockList()->addStatement(makeASN(dest, src));
  }


  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  bool splitStatement(ShBasicBlock::ShStmtList::iterator stmtI, ShCtrlGraphNodePtr node)
  {
    int i;
    ShStatement &stmt = *stmtI;

    changed = true;

    // find out the op size
    int opSize = 0; 
    for(i = 0; i < opInfo[stmt.op].arity; ++i) opSize = std::max(opSize, stmt.src[i].size());

#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("Checking " << stmt);
#endif
    switch(stmt.op) {
      // for the simple cases, replace with an appropriate assignment statement
      //
      // actually, try to smack this into the same framework as the program insertions below.
      // this will very quickly become unmanageable
      //
      // @todo type decide whether we want LO and HI to work with non-interval
      // srcs
      case SH_OP_LO: // replace interval src with the lo tuple as a src  
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(shIsInterval(stmt.src[0].valueType()));
        stmt.src[0] = ShVariable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = SH_OP_ASN;
        return false;

      case SH_OP_HI:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(shIsInterval(stmt.src[0].valueType()));
        stmt.src[0] = ShVariable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = SH_OP_ASN;
        return false;

      case SH_OP_WIDTH:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(shIsInterval(stmt.src[0].valueType()));
        stmt.src[1] = ShVariable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = -ShVariable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = SH_OP_ADD;
        return false;

      case SH_OP_RADIUS:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(shIsInterval(stmt.src[0].valueType()));
        stmt.src[2] = ShVariable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[1] = -ShVariable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = ShConstAttrib1f(0.5f);
        stmt.op = SH_OP_LRP;
        return false;

      case SH_OP_CENTER:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        if(!shIsInterval(stmt.src[0].valueType())) {
          SH_DEBUG_PRINT("Expecting interval value in src[0] in stmt=" << stmt);
          SH_DEBUG_ASSERT(0);
        }
        stmt.src[2] = ShVariable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[1] = ShVariable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = ShConstAttrib1f(0.5f);
        stmt.op = SH_OP_LRP;
        return false;

      case SH_OP_IVAL: {
#ifdef SH_DEBUG_ICONV
          SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
          SH_DEBUG_ASSERT(shIsInterval(stmt.dest.valueType()));
          // change to two asns (stmt becomes asn to lo, insert new asn to hi)
          ShVariable lo(splits[stmt.dest.node()][0], stmt.dest.swizzle(), stmt.dest.neg());
          ShVariable hi(splits[stmt.dest.node()][1], stmt.dest.swizzle(), stmt.dest.neg());

          node->block->m_statements.insert(stmtI, makeASN(hi, stmt.src[1]));
          stmt = makeASN(lo, stmt.src[0]);
          return false;
        }

      default:
        break;
    }
    bool iasrc, iadest;
    iadest = shIsInterval(stmt.dest.valueType()); 
    iasrc = false;
    for(i = 0; i < opInfo[stmt.op].arity; ++i) {
      iasrc = iasrc || shIsInterval(stmt.src[i].valueType());
    }

    if(!iasrc && !iadest) return false;

    ShValueType iaType = stmt.dest.valueType();
    ShValueType newType = shRegularValueType(iaType);
#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("ia type = " << shValueTypeName(iaType) << " newtype = " << shValueTypeName(newType)); 
#endif

    // @todo type do actual detection of operation type, don't just assume thd
    // est type is it
    if(!iasrc) {
#ifdef SH_DEBUG_ICONV
      SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name << "as an updated assignment");
#endif
      ShBasicBlock::ShStmtList::iterator afterI = stmtI;
      ++afterI;
      ShVariable lo(splits[stmt.dest.node()][0], stmt.dest.swizzle(), stmt.dest.neg());
      ShVariable hi(splits[stmt.dest.node()][1], stmt.dest.swizzle(), stmt.dest.neg());
      stmt.dest = ShVariable(stmt.dest.node()->clone(SH_TEMP, stmt.dest.size(), newType, 
            SH_SEMANTICTYPE_END, false, false));

      node->block->m_statements.insert(afterI, ShStatement(lo, SH_OP_ASN, stmt.dest));
      node->block->m_statements.insert(afterI, ShStatement(hi, SH_OP_ASN, stmt.dest));

      return false;
    } 

    // otherwise we need to replace the statement by a program fragment
  
    // since we used list splices, this should not invalidate the iterator
    // stmtI or variable stmt.
#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name << "as a program fragment replacement");
    SH_DEBUG_PRINT("stmt = " << stmt);
    SH_DEBUG_PRINT("  dest = " << stmt.dest);
    SH_DEBUG_PRINT("  src[0] = " << stmt.src[0]);
    SH_DEBUG_PRINT("  src[1] = " << stmt.src[1]);
#endif

    // generate the program fragment
    ShProgram newProgram = getProgram(stmt.op, opSize, newType);
    SH_DEBUG_ASSERT(newProgram.node());
    
    // map the interval versions of the original stmt src/dest to the newProgram's inputs/outputs
    // turn off optimization, since some of the glue programs here will vanish into nothing
    // if dead-code elimination is on
    int oldOptimization = ShContext::current()->optimization();
    ShContext::current()->optimization(0);
   
    ShProgramNode::VarList::const_iterator I = newProgram.inputs_begin(); 
    ShProgram inputMapper = SH_BEGIN_PROGRAM() {
      for(i = 0; i < opInfo[stmt.op].arity; ++i) {
        ShVariableNodePtr lo, hi;
        if(!shIsInterval(stmt.src[i].valueType())) {
          lo = stmt.src[i].node();
          hi = stmt.src[i].node();
        } else {
          lo = splits[stmt.src[i].node()][0];
          hi = splits[stmt.src[i].node()][1];
        }
        SH_DEBUG_ASSERT(I != newProgram.inputs_end());
        bool neg = stmt.src[i].neg();
        doASN(ShVariable((*I)->clone(SH_OUTPUT)), ShVariable(neg ? hi : lo, stmt.src[i].swizzle(), neg));
        ++I;
        SH_DEBUG_ASSERT(I != newProgram.inputs_end());
        doASN(ShVariable((*I)->clone(SH_OUTPUT)), ShVariable(neg ? lo : hi,stmt.src[i].swizzle(), neg));
        ++I;
      }
    } SH_END;

    I = newProgram.outputs_begin(); 
    ShProgram outputMapper = SH_BEGIN_PROGRAM() {
      SH_DEBUG_ASSERT(I != newProgram.outputs_end());
      if(splits.find(stmt.dest.node()) == splits.end()) {
        SH_DEBUG_PRINT("You fool " << stmt);
        SH_DEBUG_ASSERT(0);
      }
      bool neg = stmt.dest.neg();
      ShVariableNodePtr lo, hi;
      lo = splits[stmt.dest.node()][0];
      hi = splits[stmt.dest.node()][1];
      doASN(ShVariable(neg ? hi : lo, stmt.dest.swizzle(), neg), ShVariable((*I)->clone(SH_INPUT)));
      ++I;
      SH_DEBUG_ASSERT(I != newProgram.outputs_end());
      doASN(ShVariable(neg ? lo : hi, stmt.dest.swizzle(), neg), ShVariable((*I)->clone(SH_INPUT)));
    } SH_END;

    newProgram = outputMapper << newProgram << inputMapper; 
    
    spliceProgram(node, stmtI, newProgram);

    // re-enable optimization
    ShContext::current()->optimization(oldOptimization);

    // the last thing - cleanup by removing original statement from the block
    node->block->erase(stmtI); 

    return true;
  }

  bool &changed;
  ShTransformer::VarSplitMap &splits;
};

void dump(ShProgramNodePtr p, std::string desc) {
#ifdef SH_DEBUG_ICONV
  p->dump(desc);
#endif
}

}

namespace SH {

void ShTransformer::convertIntervalTypes() {
  dump(m_program, "iconv_before");
  // replace branches first with non-range types
  m_changed |= fixRangeBranches(m_program);

  dump(m_program, "iconv_fixbranch");

  // split up intervals into two variables
  ShTransformer::VarSplitMap splits;
  IntervalSplitter is(splits, m_changed);
  is.splitVarList(m_program->inputs);
  is.splitVarList(m_program->outputs);
  m_program->ctrlGraph->dfs(is);

  dump(m_program, "iconv_split");

  // @todo type this thing is DARNED slow...
  // (It can fix LO, HI, SETLO, and SETHI statements in place without
  // graph mangling)

  // fix statements that used to use interval types
  IntervalStatementFixer isf(m_changed, splits);
  m_program->ctrlGraph->dfs(isf);

  optimize(m_program); // should do this here since there are likely lots of program fragments that could use straightening before later work
  dump(m_program, "iconv_fixstmt");
}

}
