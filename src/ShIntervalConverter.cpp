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
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariable.hpp"
#include "ShVariableNode.hpp"
#include "ShInternals.hpp"
#include "ShSyntax.hpp"
#include "ShProgram.hpp"
#include "ShInstructions.hpp"
#include "ShTransformer.hpp"

namespace {

using namespace SH;

// A set of programs that does interval arithmetic operations, except 
// on 2N-tuples of type T instead of a pair of lo/hi N-tuples
ShProgram intervalADD(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariable a_lo(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable a_hi(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable b_lo(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable b_hi(new ShVariableNode(SH_INPUT, N, valueType));

    ShVariable r_lo(new ShVariableNode(SH_OUTPUT, N, valueType));
    ShVariable r_hi(new ShVariableNode(SH_OUTPUT, N, valueType));

    shADD(r_lo, a_lo, b_lo);
    shADD(r_hi, a_hi, b_hi);
  } SH_END;
  return result;
}

ShProgram intervalMUL(int N, ShValueType valueType) 
{
  ShProgram result = SH_BEGIN_PROGRAM() {
    ShVariable a_lo(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable a_hi(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable b_lo(new ShVariableNode(SH_INPUT, N, valueType));
    ShVariable b_hi(new ShVariableNode(SH_INPUT, N, valueType));

    ShVariable r_lo(new ShVariableNode(SH_OUTPUT, N, valueType));
    ShVariable r_hi(new ShVariableNode(SH_OUTPUT, N, valueType));

    ShVariable ll(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable lh(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable hl(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable hh(new ShVariableNode(SH_TEMP, N, valueType));
    ShVariable temp(new ShVariableNode(SH_TEMP, N, valueType));

    shMUL(ll, a_lo, b_lo);
    shMUL(lh, a_lo, b_hi);
    shMUL(hl, a_hi, b_lo);
    shMUL(hh, a_hi, b_hi);

    shMIN(temp, ll, lh);
    shMIN(r_lo, hl, hh);
    shMIN(r_lo, r_lo, temp);

    shMAX(temp, ll, lh);
    shMAX(r_hi, hl, hh);
    shMAX(r_hi, r_hi, temp);
  } SH_END;
  return result;
}

ShProgram getProgram(ShOperation op, int N, ShValueType valueType) {
  switch(op) {
    case SH_OP_ADD:   return intervalADD(N, valueType);
    case SH_OP_MUL:   return intervalMUL(N, valueType);
    default:
      shError(ShTransformerException(
            "Cannot translate interval arithmetic"));
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
        // (#) erase the stuff that split added to the end of the var list
        // @todo type check if this is actually no longer needed. 
        //vars.resize(vars.size() - splits[*I].size());

        vars.insert(I, splits[*I].begin(), splits[*I].end());
        I = vars.erase(I); 
      } else ++I;
    }
  }
  
  void splitVars(ShStatement& stmt) {
    stmt.marked = false;
    if(stmt.dest.node()) stmt.marked = split(stmt.dest.node()) || stmt.marked;
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) stmt.marked = split(stmt.src[i].node()) || stmt.marked; 
  }

  // returns true if variable split
  // does not add variable to Program's VarList, so this must be handled manually 
  // (since this matters only for IN/OUT/INOUT types, splitVarList handles the
  // insertions nicely)
  bool split(ShVariableNodePtr node)
  {
    ShValueType valueType = node->valueType(); 

    if(!shIsIntervalValueType(valueType)) return false; 
    else if(splits.count(node) > 0) return true;

    if( node->kind() == SH_TEXTURE || node->kind() == SH_STREAM ) {
      shError( ShTransformerException(
            "Interval Arithmetic support is not implemented for textures or streams"));
    }
    changed = true;

    ShTransformer::VarNodeVec &splitNodes = splits[node];
    ShValueType newValueType = shRegularValueType(valueType);

    splitNodes.push_back(node->clone(newValueType, false));  // lo
    splitNodes.push_back(node->clone(newValueType, false));  // hi
    // @todo type handle constants/uniforms

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

  IntervalStatementFixer(ShTransformer::VarSplitMap &splits)
    : splits(splits), dirty(false) {}

  void operator()(ShCtrlGraphNodePtr node) {
    if (dirty) return;
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;

    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      if(I->marked) splitStatement(I, node);
    }
  }


  void splitStatement(ShBasicBlock::ShStmtList::iterator stmtI, ShCtrlGraphNodePtr node)
  {
    int i;
    ShStatement &stmt = *stmtI;

    // find out the op size
    int opSize = 0; 
    for(i = 0; stmt.src[i].node(); ++i) opSize = std::max(opSize, stmt.src[i].size());

    switch(stmt.op) {
      // for the simple cases, replace with an appropriate assignment statement
      case SH_OP_LO: // replace interval src with the lo tuple as a src  
        stmt.src[0] = ShVariable(splits[stmt.src[0].node()][0], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = SH_OP_ASN;
        break;

      case SH_OP_HI:
        stmt.src[0] = ShVariable(splits[stmt.src[0].node()][1], stmt.src[0].swizzle(), stmt.src[0].neg());
        stmt.op = SH_OP_ASN;
        break;

      case SH_OP_SETLO:
        stmt.dest = ShVariable(splits[stmt.dest.node()][0], stmt.dest.swizzle(), stmt.dest.neg());
        stmt.op = SH_OP_ASN;
        break;

      case SH_OP_SETHI:
        stmt.dest = ShVariable(splits[stmt.dest.node()][1], stmt.dest.swizzle(), stmt.dest.neg());
        stmt.op = SH_OP_ASN;
        break;

      default:
        // otherwise, we need to do some graph mangling
        dirty = true;

        // split up the control graph node at the current statement
        ShCtrlGraphNodePtr after = node->split(stmtI);

        // since we used list splices, this should not invalidate the iterator
        // stmtI or variable stmt.

        // @todo type handle statements that have some non-interval src/dest
        // for now, assume all interval, and all the same interval type
        ShValueType iaType = stmt.dest.valueType();
        ShValueType newType = shRegularValueType(iaType);

        ShProgram newProgram = getProgram(stmt.op, opSize, newType);
        
        // @todo type should use algebra ops to do below in fewer lines 
        
        // run a variable replacement to turn newProgram's inputs/outputs into
        // temps, and add them to lists for argument/result passing from the
        // original statement
        ShVarMap varMap;
        ShProgramNode::VarList destNodes; 
        ShProgramNode::VarList srcNodes; 
        ShProgramNodePtr pnode = newProgram.node();
        ShVariableNodePtr temp;
        ShProgramNode::VarList::const_iterator I;
        ShProgramNode::VarList::const_iterator O;

        for(I = pnode->inputs.begin(); I != pnode->inputs.end(); ++I) {
          varMap[*I] = temp = (*I)->clone(SH_TEMP); 
          srcNodes.push_back(temp);
        }

        for(O = pnode->outputs.begin(); O != pnode->outputs.end(); ++O) {
          varMap[*O] = temp = (*O)->clone(SH_TEMP); 
          destNodes.push_back(temp);
        }
        
        ShVariableReplacer varFix(varMap);
        newProgram.node()->ctrlGraph->dfs(varFix);

        // now do the appropriate assignments to send split up src vars to the appropriate
        // temps and results to the appropriate split up dest vars 
        // (this needs to be done incase of swizzling/write masking on the
        // src/dest.  we cannot just varmap those into the newProgram's in/out)
        ShBasicBlockPtr beforeBlock = node->block;
        I = srcNodes.begin();
        for(i = 0; stmt.src[i].node() && I != srcNodes.end(); ++i) {
          // assign lo and hi
          beforeBlock->addStatement(ShStatement(ShVariable(*I), SH_OP_ASN, 
                ShVariable(splits[stmt.src[i].node()][0],stmt.src[i].swizzle(), stmt.src[i].neg())));
          ++I;
          SH_DEBUG_ASSERT(I != srcNodes.end());
          beforeBlock->addStatement(ShStatement(ShVariable(*I), SH_OP_ASN,
                ShVariable(splits[stmt.src[i].node()][1],stmt.src[i].swizzle(), stmt.src[i].neg())));
          ++I;
        }

        ShBasicBlockPtr afterBlock = after->block;
        I = srcNodes.begin();
          // assign lo and hi
        afterBlock->addStatement(ShStatement(ShVariable(splits[stmt.dest.node()][0],stmt.dest.swizzle(), stmt.dest.neg()),
              SH_OP_ASN, ShVariable(*I)));
        ++I;
        SH_DEBUG_ASSERT(I != srcNodes.end());
        afterBlock->addStatement(ShStatement(ShVariable(splits[stmt.dest.node()][1],stmt.dest.swizzle(), stmt.dest.neg()),
              SH_OP_ASN, ShVariable(*I)));

        // join up the nodes appropriately
        node->append(newProgram.node()->ctrlGraph->entry());
        newProgram.node()->ctrlGraph->exit()->append(after);

        // the last thing - cleanup by removing original statement from the
        // block
        afterBlock->erase(stmtI); // get rid of the 
    }
  }

  void resetDirt() {
    dirty = false;
  }

  bool isDirty() {
    return dirty;
  }

  ShTransformer::VarSplitMap &splits;
  bool dirty;
};

}

namespace SH {

void ShTransformer::convertIntervalTypes(ShTransformer::VarSplitMap &splits) {
  IntervalSplitter is(splits, m_changed);
  is.splitVarList(m_program->inputs);
  is.splitVarList(m_program->outputs);
  m_program->ctrlGraph->dfs(is);

  // @todo type this thing is DARNED slow...
  // To make it faster, need to think about how to continue properly
  // traversing the control graph even if the statement fixer
  // starts mucking around with both the node currently being traversed
  // and its children...
  //
  // The default dfs might work...but until it provably does, 
  // the StatementFixer starts traversing anew on each pass
  // until it hits one statement that needs fixing,
  // mangles the graph around inserting the replacement
  // code, and quits...
  //
  // (It can fix LO, HI, SETLO, and SETHI statements in place without
  // graph mangling)

  IntervalStatementFixer isf(splits);
  do {
    isf.resetDirt();
    m_program->ctrlGraph->dfs(isf);
    m_changed = m_changed || isf.isDirty();
  } while(isf.isDirty());
}

}
