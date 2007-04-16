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
#include <algorithm>
#include <map>
#include <list>
#include <fstream>
#include "Error.hpp"
#include "Debug.hpp"
#include "TypeInfo.hpp"
#include "Variable.hpp"
#include "VariableNode.hpp"
#include "Record.hpp"
#include "Internals.hpp"
#include "Syntax.hpp"
#include "Program.hpp"
#include "Instructions.hpp"
#include "Transformer.hpp"
#include "Algebra.hpp"
#include "Manipulator.hpp"
#include "CtrlGraphWranglers.hpp"
#include "Optimizations.hpp"
// @todo range - add this back in
//#include "RangeBranchFixer.hpp"

/* @todo fix this */
//#include "IntervalTexture.hpp"

// @todo range Change this to use a wrapper like AaVariable for cleaner code. 

// @todo debug
// #define SH_DEBUG_ICONV
namespace {

using namespace SH;

struct VariablePair {
  Variable lo, hi;
  VariablePair(BindingType Binding, int N, ValueType V) 
    : lo(new VariableNode(Binding, N, V)),
      hi(new VariableNode(Binding, N, V)) 
  {}

  Record record() 
  {
    return (lo & hi);
  }

  void name(const std::string& name)
  {
    lo.name(name + "_lo");
    hi.name(name + "_hi");
  }
};


// A set of programs that does interval arithmetic operations, except 
// on 2N-tuples of type T instead of a pair of lo/hi N-tuples
Program intervalASN(int N, ValueType valueType)
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INOUT, N, valueType);
  } SH_END;
  return result;
}

template<Operation OP>
Program intervalBinaryMonotonic(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    Context::current()->parsing()->tokenizer.blockList()->addStatement(
        Statement(r.lo, a.lo, OP, b.lo));
    Context::current()->parsing()->tokenizer.blockList()->addStatement(
        Statement(r.hi, a.hi, OP, b.hi));
  } SH_END;
  return result;
}


Program intervalMUL(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    Variable ll(new VariableNode(SH_TEMP, N, valueType));
    Variable lh(new VariableNode(SH_TEMP, N, valueType));
    Variable hl(new VariableNode(SH_TEMP, N, valueType));
    Variable hh(new VariableNode(SH_TEMP, N, valueType));
    Variable temp(new VariableNode(SH_TEMP, N, valueType));

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

Program intervalRCP(int N, ValueType valueType); // forward declaration

Program intervalDIV(int N, ValueType valueType)
{
  Program mul = intervalMUL(N, valueType);
  Program rcp = intervalRCP(N, valueType);
  Program result = mul << rcp; 
  result = result << swizzle(2, 3, 0, 1); 
  return result;
}

Program intervalSLT(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shSLT(r.lo, a.hi, b.lo);
    shSGT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

Program intervalSLE(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shSLE(r.lo, a.hi, b.lo);
    shSGT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

Program intervalSGT(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shSGT(r.lo, a.hi, b.lo);
    shSLT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

Program intervalSGE(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shSGE(r.lo, a.hi, b.lo);
    shSLT(r.hi, a.lo, b.hi);
  } SH_END;
  return result;
}

Program intervalCSUM(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, 1, valueType);

    shCSUM(r.lo, a.lo);
    shCSUM(r.hi, a.hi);
  } SH_END;
  return result;
}

Program intervalDOT(int N, ValueType valueType) 
{
  Program mul = intervalMUL(N, valueType);
  Program csum = intervalCSUM(N, valueType);
  return csum << mul; 
}

Program intervalFRAC(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);
    Variable temp(new VariableNode(SH_TEMP, N, valueType));
    VariablePair frac_a(SH_TEMP, N, valueType);

    // if floor(a.lo) is not floor(a.hi),  
    shFLR(temp, a.lo); 
    shADD(temp, a.hi, -temp);
    shSGE(temp, temp, ConstAttrib1f(1.0f).repeat(N));

    shFRAC(frac_a.lo, a.lo);
    shFRAC(frac_a.hi, a.hi);
    shCOND(r.lo, temp, ConstAttrib1f(0.0f).repeat(N), frac_a.lo);
    shCOND(r.hi, temp, ConstAttrib1f(1.0f).repeat(N), frac_a.hi);
  } SH_END;
  return result;
}

Program intervalNEG(int N, ValueType valueType)
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shASN(r.lo, -a.hi);
    shASN(r.hi, -a.lo);
  } SH_END;
  return result;
}

Program intervalMAD(int N, ValueType valueType) 
{
  Program mul = intervalMUL(N, valueType);
  Program add = intervalBinaryMonotonic<OP_ADD>(N, valueType);
  return add << mul; 
}

Program intervalLRP(int N, ValueType valueType) 
{
  Program mad = intervalMAD(N, valueType);
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair c(SH_INPUT, N, valueType);
    VariablePair bsc(SH_TEMP, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shADD(bsc.lo, b.lo, -c.hi);
    shADD(bsc.hi, b.hi, -c.lo);
    r.record() = mad(a.record() & bsc.record() & c.record());
  } SH_END;
  return result; 
}


Program intervalRCP(int N, ValueType valueType)
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    // @todo range debugging hack
    // using conditionals hopefully to get by NaNs
    // (check if a.lo < 0 && 0 < a.hi 
    //
    VariablePair test(SH_TEMP, N, valueType);
    VariablePair zerocond(SH_TEMP, N, valueType);
    shSLT(test.lo, a.lo, ConstAttrib1f(1e-4).repeat(N));
    shSLT(test.hi, ConstAttrib1f(-1e-4).repeat(N), a.hi);

    shMUL(test.lo, test.lo, test.hi);

    SH_IF(test.lo) {
      shASN(r.lo, ConstAttrib1f(-1e8).repeat(N));
      shASN(r.hi, ConstAttrib1f(1e8).repeat(N));
    } SH_ELSE {
      shRCP(r.lo, a.hi);
      shRCP(r.hi, a.lo);
    } SH_ENDIF;
  } SH_END;
  return result;
}


Program intervalUNION(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shMIN(r.lo, a.lo, b.lo);
    shMAX(r.hi, a.hi, b.hi);
  } SH_END;
  return result;
}

Program intervalISCT(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shMAX(r.lo, a.lo, b.lo);
    shMIN(r.hi, a.hi, b.hi);
  } SH_END;
  return result;
}

// @todo range - do this properly
Program intervalCONTAINS(int N, ValueType valueType) 
{
  Program result = SH_BEGIN_PROGRAM() {
    VariablePair a(SH_INPUT, N, valueType);
    VariablePair b(SH_INPUT, N, valueType);
    VariablePair r(SH_OUTPUT, N, valueType);

    shSLE(r.lo, a.lo, b.lo);
    shSGE(r.hi, a.hi, b.hi);
    shMUL(r.lo, r.lo, r.hi);
    shASN(r.hi, r.lo);
  } SH_END;
  return result;
}

Program intervalTEX(const Variable& texVar, int N, ValueType destvt, ValueType srcvt) 
{
  assert(false);
  Program result = SH_BEGIN_PROGRAM() {
#if 0
    VariablePair tc(SH_INPUT, N, srcvt);
    VariablePair r(SH_OUTPUT, texVar.size(), destvt);

    TextureNodePtr texNode = shref_dynamic_cast<TextureNode>(texVar.node());
    SH_DEBUG_ASSERT(texNode);
    //IntervalTexturePtr iatex = IntervalTexture::convert(texNode);
    SH_DEBUG_ASSERT(iatex);

    Record rRec = r.record();
    rRec = iatex->lookup(tc.lo, tc.hi);
#endif
  } SH_END;
  return result;
}

Program intervalTEXI(const Variable& texVar, int N, ValueType destvt, ValueType srcvt) 
{
  assert(false);
  Program result = SH_BEGIN_PROGRAM() {
#if 0
    VariablePair tc(SH_INPUT, N, srcvt);
    VariablePair r(SH_OUTPUT, texVar.size(), destvt);

    TextureNodePtr texNode = shref_dynamic_cast<TextureNode>(texVar.node());
    SH_DEBUG_ASSERT(texNode);
    //IntervalTexturePtr iatex = IntervalTexture::convert(texNode);
    assert(false);
    SH_DEBUG_ASSERT(iatex);

    r.record() = iatex->rect_lookup(tc.lo, tc.hi);
#endif
  } SH_END;
  return result;
}

Program getProgram(Operation op, int N, ValueType valueType) {
  switch(op) {
    case OP_ASN:   return intervalASN(N, valueType);
    case OP_ADD:   return intervalBinaryMonotonic<OP_ADD>(N, valueType);
    case OP_MUL:   return intervalMUL(N, valueType);
    case OP_DIV:   return intervalDIV(N, valueType);

    case OP_SLT:   return intervalSLT(N, valueType);
    case OP_SLE:   return intervalSLE(N, valueType);
    case OP_SGT:   return intervalSGT(N, valueType);
    case OP_SGE:   return intervalSGE(N, valueType);

    case OP_CEIL:   return intervalBinaryMonotonic<OP_CEIL>(N, valueType);
    case OP_CSUM:   return intervalCSUM(N, valueType);
    case OP_DOT:   return intervalDOT(N, valueType);
    case OP_EXP:   return intervalBinaryMonotonic<OP_EXP>(N, valueType);
    case OP_EXP2:   return intervalBinaryMonotonic<OP_EXP2>(N, valueType);
    case OP_EXP10:   return intervalBinaryMonotonic<OP_EXP10>(N, valueType);
    case OP_FLR:   return intervalBinaryMonotonic<OP_FLR>(N, valueType);
    case OP_FRAC:  return intervalFRAC(N, valueType); 
    case OP_LOG:   return intervalBinaryMonotonic<OP_LOG>(N, valueType);
    case OP_LOG2:   return intervalBinaryMonotonic<OP_LOG2>(N, valueType);
    case OP_LOG10:   return intervalBinaryMonotonic<OP_LOG10>(N, valueType);
    case OP_MAD:   return intervalMAD(N, valueType);
    case OP_LRP:   return intervalLRP(N, valueType);
    case OP_MAX:   return intervalBinaryMonotonic<OP_MAX>(N, valueType);
    case OP_MIN:   return intervalBinaryMonotonic<OP_MIN>(N, valueType);
    case OP_RSQ:   return intervalBinaryMonotonic<OP_RSQ>(N, valueType);
    case OP_RCP:   return intervalRCP(N, valueType);
    case OP_RND:   return intervalBinaryMonotonic<OP_RND>(N, valueType);
    case OP_SGN:   return intervalBinaryMonotonic<OP_SGN>(N, valueType);
    case OP_SQRT:   return intervalBinaryMonotonic<OP_SQRT>(N, valueType);

    case OP_UNION:   return intervalUNION(N, valueType);
    case OP_ISCT:   return intervalISCT(N, valueType);
    case OP_CONTAINS:   return intervalCONTAINS(N, valueType);
    // @todo type add in some more operations
    default:
      SH_DEBUG_PRINT("Unable to translate op: " << opInfo[op].name);
      error(TransformerException(
            "Cannot translate interval arithmetic operator" ));
  }
  return Program();
}

typedef std::map<int, int> ValueTypeMap; 

// Converts operations on Interval<T> to operations on T 
// This takes the same form as the variable splitters, but does more complicated
// code splices.
//
// So the first step is to assign new lo/hi tuples corresponding to each
// interval tuple.
//
// Then interval operations are converted to use lo/hi tuples by splicing in
// code fragments using Program objects in SplitIntervalProgram 
struct IntervalSplitter {

  IntervalSplitter(Transformer::VarSplitMap& splits, bool &changed)
    : splits(splits), changed(changed) {
  }

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      splitVars(*I);
    }
  }

  // this must be called BEFORE running a DFS on the program
  // to split temporaries (otherwise the stupid hack marked (#) does not work)
  void splitVarList(ProgramNode::VarList &vars) {
    for(ProgramNode::VarList::iterator I = vars.begin();
        I != vars.end();) {
      if(split(*I)) {
        vars.insert(I, splits[*I].begin(), splits[*I].end());
        I = vars.erase(I); 
      } else ++I;
    }
  }

  VariableNodePtr cloneNode(VariableNodePtr node, ValueType rangeValueType) {
    return node->clone(BINDINGTYPE_END, 0, rangeValueType, SEMANTICTYPE_END, false); 
  }
  
  void splitVars(Statement& stmt) {
    if(stmt.dest.node()) split(stmt.dest.node()); 
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) split(stmt.src[i].node()); 
  }

  // returns true if variable split
  // does not add variable to Program's VarList, so this must be handled manually 
  // (since this matters only for IN/OUT/INOUT types, splitVarList handles the
  // insertions nicely)
  bool split(VariableNodePtr node)
  {
    ValueType valueType = node->valueType(); 

    if(!isInterval(valueType)) return false; 
    else if(splits.count(node) > 0) return true;

    if( node->kind() == SH_TEXTURE || node->kind() == SH_STREAM ) {
      error( TransformerException(
            "Interval Arithmetic support is not implemented for textures or streams"));
    }
    changed = true;

#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("Splitting Variable " << node->nameOfType() << " " << node->name());
#endif

    Transformer::VarNodeVec &splitNodes = splits[node];
    ValueType newValueType = regularValueType(valueType);

    splitNodes.push_back(cloneNode(node, newValueType));  // lo
    splitNodes.push_back(cloneNode(node, newValueType));  // hi
    splitNodes[0]->name(node->name() + "_lo");
    splitNodes[1]->name(node->name() + "_hi");

    if(node->hasValues()) { // copy over values
      Context::current()->enter(0);
      Variable loVar(splitNodes[0]);
      shLO(loVar, Variable(node));

      Variable hiVar(splitNodes[1]);
      shHI(hiVar, Variable(node));
      Context::current()->exit();
    }

    if(node->uniform()) { // set up dependent uniform
#ifdef SH_DEBUG_ICONV
      SH_DEBUG_PRINT("Splitting uniform and setting up dependencies for node " << node->name());
#endif
      Program loprg = SH_BEGIN_PROGRAM("uniform") {
        Variable original(node);
        Variable converted(splitNodes[0]);
        shLO(converted, original);
      } SH_END;
      splitNodes[0]->attach(loprg.node());

      Program hiprg = SH_BEGIN_PROGRAM("uniform") {
        Variable original(node);
        Variable converted(splitNodes[1]);
        shHI(converted, original);
      } SH_END;
      splitNodes[1]->attach(hiprg.node());
    }

    return true; 
  }

  Transformer::VarSplitMap &splits;
  bool& changed;
};

// @todo type handle negations - remove all negations from interval Variables 
// since negating the lo/hi doesn't give a valid interval any more 

/** Splits up operations on intervals into separate non-interval ops
 * replaces LO, HI, SETLO, SETHI instructions with appropriatly.
 */
struct IntervalStatementFixer {
  typedef std::vector<Variable> VarVec;

  IntervalStatementFixer(bool &changed, Transformer::VarSplitMap &splits)
    : changed(changed), splits(splits) {}

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;

    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if(splitStatement(I, node)) break;
    }
  }

  bool isSplit(const Variable &var) {
    return splits.count(var.node()) > 0;
  }

  // swizzles the source if it is scalar and the dest is a vector
  // This should be used whenever there is a risk that the src.size() !=
  // dest.size()
  Statement makeASN(const Variable &dest, const Variable &src)
  {
    if(dest.size() == src.size()) return Statement(dest, OP_ASN, src);

    SH_DEBUG_ASSERT(src.size() == 1);
    int *swiz = new int[dest.size()];
    for(int i = 0; i < dest.size(); ++i) swiz[i] = 0;
    Statement result(dest, OP_ASN, src(dest.size(), swiz));
    delete [] swiz;
    return result;
  }

  void doASN(const Variable &dest, const Variable &src)
  {
    Context::current()->parsing()->tokenizer.blockList()->addStatement(makeASN(dest, src));
  }


  /* Splits a statement.  Returns whether the operator requires splicing in a new program
   * fragment. 
   */
  bool splitStatement(BasicBlock::StmtList::iterator stmtI, CtrlGraphNode* node)
  {
    int i;
    Statement &stmt = *stmtI;

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
      case OP_LO: // replace interval src with the lo tuple as a src  
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(isInterval(stmt.src[0].valueType()));
        stmt.src[0] = Variable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = OP_ASN;
        return false;

      case OP_HI:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(isInterval(stmt.src[0].valueType()));
        stmt.src[0] = Variable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = OP_ASN;
        return false;

      case OP_WIDTH:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(isInterval(stmt.src[0].valueType()));
        stmt.src[1] = Variable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = -Variable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = OP_ADD;
        return false;

      case OP_RADIUS:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        SH_DEBUG_ASSERT(isInterval(stmt.src[0].valueType()));
        stmt.src[2] = Variable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[1] = -Variable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = ConstAttrib1f(0.5f);
        stmt.op = OP_LRP;
        return false;

      case OP_CENTER:
#ifdef SH_DEBUG_ICONV
        SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
        if(!isInterval(stmt.src[0].valueType())) {
          SH_DEBUG_PRINT("Expecting interval value in src[0] in stmt=" << stmt);
          SH_DEBUG_ASSERT(0);
        }
        stmt.src[2] = Variable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[1] = Variable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.src[0] = ConstAttrib1f(0.5f);
        stmt.op = OP_LRP;
        return false;

      case OP_IVAL: {
#ifdef SH_DEBUG_ICONV
          SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name);
#endif
          SH_DEBUG_ASSERT(isInterval(stmt.dest.valueType()));
          // change to two asns (stmt becomes asn to lo, insert new asn to hi)
          Variable lo(splits[stmt.dest.node()][0], stmt.dest.swizzle(), stmt.dest.neg());
          Variable hi(splits[stmt.dest.node()][1], stmt.dest.swizzle(), stmt.dest.neg());

          node->block->m_statements.insert(stmtI, makeASN(hi, stmt.src[1]));
          stmt = makeASN(lo, stmt.src[0]);
          return false;
        }

      default:
        break;
    }
    bool iasrc, iadest;
    iadest = isInterval(stmt.dest.valueType()); 
    iasrc = false;
    for(i = 0; i < opInfo[stmt.op].arity; ++i) {
      iasrc = iasrc || isInterval(stmt.src[i].valueType());
    }

    if(!iasrc && !iadest) return false;

    ValueType iaType = stmt.dest.valueType();
    ValueType newType = regularValueType(iaType);
#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("ia type = " << valueTypeName(iaType) << " newtype = " << valueTypeName(newType)); 
#endif

    // @todo type do actual detection of operation type, don't just assume the dest type is it
    if(!iasrc) {
#ifdef SH_DEBUG_ICONV
      SH_DEBUG_PRINT("Handling " << opInfo[stmt.op].name << "as an updated assignment");
#endif
      BasicBlock::StmtList::iterator afterI = stmtI;
      ++afterI;
      Variable lo(splits[stmt.dest.node()][0], stmt.dest.swizzle(), stmt.dest.neg());
      Variable hi(splits[stmt.dest.node()][1], stmt.dest.swizzle(), stmt.dest.neg());
      stmt.dest = Variable(stmt.dest.node()->clone(SH_TEMP, stmt.dest.size(), newType, 
            SEMANTICTYPE_END, false, false));

      node->block->m_statements.insert(afterI, Statement(lo, OP_ASN, stmt.dest));
      node->block->m_statements.insert(afterI, Statement(hi, OP_ASN, stmt.dest));

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

    Program newProgram;
    // @todo range merge this together instead of special casing:
    switch(stmt.op) {
      case OP_TEX:
        newProgram = intervalTEX(stmt.src[0], stmt.src[1].size(), newType, newType); 
        break;
      case OP_TEXI:
        newProgram = intervalTEXI(stmt.src[0], stmt.src[1].size(), newType, newType); 
        break;
      default:
        newProgram = getProgram(stmt.op, opSize, newType);
        break;
    }

    // generate the program fragment
    SH_DEBUG_ASSERT(newProgram.node());
    
    // map the interval versions of the original stmt src/dest to the newProgram's inputs/outputs
    // turn off optimization, since some of the glue programs here will vanish into nothing
    // if dead-code elimination is on
    int oldOptimization = Context::current()->optimization();
    Context::current()->optimization(0);
   
    ProgramNode::VarList::const_iterator I = newProgram.begin_inputs(); 
    Program inputMapper = SH_BEGIN_PROGRAM() {
      for(i = 0; i < opInfo[stmt.op].arity; ++i) {
        VariableNodePtr lo, hi;

        if(stmt.src[i].node()->kind() == SH_TEXTURE) continue;

        if(!isInterval(stmt.src[i].valueType())) {
          lo = stmt.src[i].node();
          hi = stmt.src[i].node();
        } else {
          lo = splits[stmt.src[i].node()][0];
          hi = splits[stmt.src[i].node()][1];
        }
        SH_DEBUG_ASSERT(I != newProgram.end_inputs());
        bool neg = stmt.src[i].neg();
        doASN(Variable((*I)->clone(SH_OUTPUT)), Variable(neg ? hi : lo, stmt.src[i].swizzle(), neg));
        ++I;
        SH_DEBUG_ASSERT(I != newProgram.end_inputs());
        doASN(Variable((*I)->clone(SH_OUTPUT)), Variable(neg ? lo : hi,stmt.src[i].swizzle(), neg));
        ++I;
      }
    } SH_END;

    I = newProgram.begin_outputs(); 
    Program outputMapper = SH_BEGIN_PROGRAM() {
      SH_DEBUG_ASSERT(I != newProgram.end_outputs());
      if(splits.find(stmt.dest.node()) == splits.end()) {
        SH_DEBUG_PRINT("You fool " << stmt);
        SH_DEBUG_ASSERT(0);
      }
      bool neg = stmt.dest.neg();
      VariableNodePtr lo, hi;
      lo = splits[stmt.dest.node()][0];
      hi = splits[stmt.dest.node()][1];
      doASN(Variable(neg ? hi : lo, stmt.dest.swizzle(), neg), Variable((*I)->clone(SH_INPUT)));
      ++I;
      SH_DEBUG_ASSERT(I != newProgram.end_outputs());
      doASN(Variable(neg ? lo : hi, stmt.dest.swizzle(), neg), Variable((*I)->clone(SH_INPUT)));
    } SH_END;

    newProgram = outputMapper << newProgram << inputMapper; 

    spliceProgram(node, stmtI, newProgram);

    // re-enable optimization
    Context::current()->optimization(oldOptimization);

#ifdef SH_DEBUG_ICONV
    SH_DEBUG_PRINT("Done handling " << opInfo[stmt.op].name << "as a program fragment replacement");
#endif

    // the last thing - cleanup by removing original statement from the block
    node->block->erase(stmtI); 

    return true;
  }

  bool &changed;
  Transformer::VarSplitMap &splits;
};

void dump(ProgramNodePtr p, std::string desc) {
#ifdef SH_DEBUG_ICONV
  p->dump(desc);
#endif
}

}

namespace SH {

void Transformer::convertIntervalTypes() {
  dump(m_program, "iconv_before");
  // replace branches first with non-range types
  //m_changed |= fixRangeBranches(m_program);

  //dump(m_program, "iconv_fixbranch");

  // split up intervals into two variables
  Transformer::VarSplitMap splits;
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
