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
#include "ShTransformer.hpp"
#include <algorithm>
#include <map>
#include <list>
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShInternals.hpp"

namespace SH {

ShTransformer::ShTransformer(ShCtrlGraphPtr graph, ShBackendPtr backend)
  : m_graph(graph), m_backend(backend)
{
}

ShTransformer::~ShTransformer()
{
}

void ShTransformer::transform() 
{
  bool changed = false;
  inputOutputConversion(changed);

  // check if tupleSplitting is required
  tupleSplitting(changed);

}

typedef std::vector<ShVariableNodePtr> VarNodeVec;
typedef std::map<ShVariableNodePtr, VarNodeVec> VarSplitMap;

// Variable splitting, marks statements for which some variable is split 
struct VariableSplitter {

  VariableSplitter(ShTransformer& t, bool& changed)
    : transformer(t), changed(changed) {
    maxTuple = t.m_backend->getCapability(SH_BACKEND_MAX_TUPLE);
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      splitVars(*I);
    }
  }
  
  void splitVars(ShStatement& stmt) {
    stmt.marked = false;
    if(stmt.dest.node()) stmt.marked = stmt.marked || split(stmt.dest.node());
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) stmt.marked = stmt.marked || split(stmt.src[i].node()); 
  }

  // returns true if variable split
  bool split(ShVariableNodePtr node)
  {
    int n = node->size();
    if(n <= maxTuple ) return false;
    else if(splits.count(node) > 0) return true;
    VarNodeVec &nodeVarNodeVec = splits[node];
    for(; n > 0; n -= maxTuple) {
      nodeVarNodeVec.push_back(new ShVariableNode(node->kind(), 
            n < maxTuple ? n : maxTuple, node->specialType()));
    }
    return true;
  }

  ShTransformer& transformer;
  bool& changed;
  int maxTuple;
  VarSplitMap splits;
};

struct StatementSplitter {
  typedef std::vector<ShVariable> VarVec;

  StatementSplitter(ShTransformer& t, VariableSplitter &vs, bool& changed)
    : transformer(t), changed(changed), maxTuple(vs.maxTuple), splits(vs.splits) { }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
      splitStatement(block, I);
    }
  }

  void makeSrcTemps(const ShVariable &v, VarVec &vv, ShBasicBlock::ShStmtList &stmts) {
    if( v.size() <= maxTuple && v.node()->size() <= maxTuple ) {
      vv.push_back(v);
      return;
    }
    int i, j, k, n;
    const ShSwizzle &swiz = v.swizzle();
    
    // get VarNodeVec for src
    VarNodeVec srcVec;
    if(splits.count(v.node()) > 0) srcVec = splits[v.node()];
    else srcVec.push_back(v.node());

    // make and assign to a VarVec for temps
    for(i = 0, n = v.size(); n > 0; i += maxTuple, n -= maxTuple) {
      int tsize = n < maxTuple ? n : maxTuple;
      //TODO  make this smarter so that it reuses variable nodes if it's just reswizlling a src node
      // (check that move elimination doesn't do this for us already)
      ShVariable tempVar(new ShVariableNode(SH_VAR_TEMP, tsize, SH_VAR_ATTRIB));
      vv.push_back(tempVar);

      int tempSwiz[tsize];
      int srcSwiz[tsize];
      int tempSize;
      for(j = 0; j < srcVec.size(); ++j) {
        tempSize = 0;
        for(k = 0; k < tsize; ++k ) { 
          if(swiz[i + k] / maxTuple == j) {
            tempSwiz[tempSize] = k;
            srcSwiz[tempSize] = swiz[i + k] % maxTuple;
            tempSize++;
          } 
        }
        if( tempSize > 0 ) {
          ShVariable srcVar(srcVec[j]);
          stmts.push_back(ShStatement(tempVar(tempSize, tempSwiz), SH_OP_ASN, srcVar(tempSize, srcSwiz)));
        }
      }
    }
  }

  // moves the result to the destination based on the destination swizzle
  void movToDest(VarNodeVec &destVec, const ShSwizzle &destSwiz, const VarVec &resultVec,
      ShBasicBlock::ShStmtList &stmts) {
    int j, k;
    int offset = 0;
    int swizd[maxTuple];
    int swizr[maxTuple];
    int size;
    for(VarVec::const_iterator I = resultVec.begin(); I != resultVec.end(); 
        offset += I->size(), ++I) {
      for(j = 0; j < destVec.size(); ++j) {
        size = 0;
        for(k = 0; k < I->size(); ++k) {
          if( destSwiz[k + offset] / maxTuple == j) {
            swizd[size] = destSwiz[k + offset] % maxTuple;
            swizr[size] = k;
            size++;
          }
        }
        if( size > 0 ) {
          ShVariable destVar(destVec[j]);
          stmts.push_back(ShStatement(destVar(size, swizd), SH_OP_ASN, (*I)(size, swizr)));
        }
      }
    }
  }

  // works on two assumptions
  // 1) special cases for DOT, XPD (and any other future non-componentwise ops) implemented separately
  // 2) Everything else is in the form N = [1|N]+ in terms of tuple sizes involved in dest and src
  void updateStatement(ShStatement &oldStmt, VarVec srcVec[3], ShBasicBlock::ShStmtList &stmts) {
    int i, j;
    ShVariable &dest = oldStmt.dest;
    const ShSwizzle &destSwiz = dest.swizzle();
    VarNodeVec destVec;
    VarVec resultVec;

    if(splits.count(dest.node()) > 0) destVec = splits[dest.node()];
    else destVec.push_back(dest.node());

    switch(oldStmt.op) {
      case SH_OP_DOT:
        { 
          // TODO for large tuples, may want to use another dot to sum up results instead of 
          // SH_OP_ADD. For now, do naive method
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          ShVariable dott = ShVariable(new ShVariableNode(SH_VAR_TEMP, 1, SH_VAR_ATTRIB)); 
          ShVariable sumt = ShVariable(new ShVariableNode(SH_VAR_TEMP, 1, SH_VAR_ATTRIB)); 
          stmts.push_back(ShStatement(sumt, srcVec[0][0], SH_OP_DOT, srcVec[1][0]));
          for(i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(ShStatement(dott, srcVec[0][i], SH_OP_DOT, srcVec[1][i]));
            stmts.push_back(ShStatement(sumt, sumt, SH_OP_ADD, dott));
          }
          resultVec.push_back(sumt);
        }
        break;
      case SH_OP_XPD:
        {
          SH_DEBUG_ASSERT( srcVec[0].size() == 1 && srcVec[0][0].size() == 3 &&
              srcVec[1].size() == 1 && srcVec[1][0].size() == 3); 
          ShVariable result = ShVariable(new ShVariableNode(SH_VAR_TEMP, 3, SH_VAR_ATTRIB));
          stmts.push_back(ShStatement(result, srcVec[0][0], SH_OP_XPD, srcVec[1][0]));
          resultVec.push_back(result);
        }
        break;

      default:
        {
          int maxi = 0;
          if( srcVec[1].size() > srcVec[0].size() ) maxi = 1;
          if( srcVec[2].size() > srcVec[maxi].size() ) maxi = 2;
          for(i = 0; i < srcVec[maxi].size(); ++i) {
            ShVariable resultPart(new ShVariableNode(SH_VAR_TEMP, srcVec[maxi][i].size(), SH_VAR_ATTRIB));
            ShStatement newStmt(resultPart, oldStmt.op);
            for(j = 0; j < 3 && !srcVec[j].empty(); ++j) {
              newStmt.src[j] = srcVec[j].size() > i ? srcVec[j][i] : srcVec[j][0];
            }
            stmts.push_back(newStmt);
            resultVec.push_back(resultPart);
          }
        }
        break;
    }
    movToDest(destVec, destSwiz, resultVec, stmts); 
  }
  
  /** splices a new sequence of resized tuples in to replace the original statement 
   * and ensures stit points at next statement in block*/
  void splitStatement(ShBasicBlockPtr block, ShBasicBlock::ShStmtList::iterator &stit) {
    ShStatement &stmt = *stit;
    int i, j;
    if(!stmt.marked && stmt.dest.size() <= maxTuple) {
      for(i = 0; i < 3; ++i) if(stmt.src[i].size() > maxTuple) break;
      if(i == 3) { // nothing needs splitting
        ++stit;
        return; 
      }
    }
    ShBasicBlock::ShStmtList newStmts;
    VarVec srcVec[3];

    for(i = 0; i < 3; ++i) if(stmt.src[i].node()) makeSrcTemps(stmt.src[i], srcVec[i], newStmts);
    updateStatement(stmt, srcVec, newStmts);

    // remove old statmeent and splice in new statements
    stit = block->erase(stit);
    block->splice(stit, newStmts);
  }

  ShTransformer& transformer;
  bool& changed;
  int maxTuple;
  VarSplitMap &splits;
};

void ShTransformer::tupleSplitting(bool& changed)
{ 
  if( m_backend->getCapability(SH_BACKEND_MAX_TUPLE) == 0 ) return; 

  VariableSplitter vs(*this, changed);
  m_graph->dfs(vs);

  StatementSplitter ss(*this, vs, changed);
  m_graph->dfs(ss);
}

// Output Convertion to temporaries 
struct InputOutputConvertor {
  InputOutputConvertor(ShTransformer& t, bool& changed)
    : transformer(t), changed(changed)
  {
    doIn = !t.m_backend->getCapability(SH_BACKEND_USE_INPUT_DEST);
    doOut = !t.m_backend->getCapability(SH_BACKEND_USE_OUTPUT_SRC);
  }

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convertIO(*I);
    }
  }
  
  void convertIO(ShStatement& stmt)
  {
    if(doIn && (!stmt.dest.null())) {
      if(stmt.dest.node()->kind() == SH_VAR_INPUT) {
        ShVariableNodePtr &oldNode = stmt.dest.node();
        if(varMap.find(oldNode) == varMap.end()) {
          varMap[oldNode] = new ShVariableNode(SH_VAR_TEMP, 
              oldNode->size(), oldNode->specialType());
        }
      }
    }
    if(doOut) for(int i = 0; i < 3; ++i) {
      if(!stmt.src[i].null()) {
        if(stmt.src[i].node()->kind() == SH_VAR_OUTPUT) { 
          ShVariableNodePtr &oldNode = stmt.src[i].node();
          if(varMap.find(oldNode) == varMap.end()) {
            varMap[oldNode] = new ShVariableNode(SH_VAR_TEMP, 
                oldNode->size(), oldNode->specialType());
          }
        }
      }
    }
  }

  void updateGraph() {
    if(varMap.empty()) return;
    changed = true;

    // create block after exit
    ShCtrlGraphNodePtr newExit = new ShCtrlGraphNode(); 
    ShCtrlGraphNodePtr oldExit = transformer.m_graph->exit();
    oldExit->append(newExit);
    transformer.m_graph->setExit(newExit);

    // create block before entry 
    ShCtrlGraphNodePtr newEntry = new ShCtrlGraphNode(); 
    ShCtrlGraphNodePtr oldEntry = transformer.m_graph->entry();
    newEntry->append(oldEntry);
    transformer.m_graph->setEntry(newEntry); 

    // generate code in the old entry and exit blocks 
    if(!oldExit->block) {
      oldExit->block = new ShBasicBlock(); 
    }
    if(!oldEntry->block) {
      oldEntry->block = new ShBasicBlock(); 
    }
    for(ShVariableReplacer::VarMap::iterator it = varMap.begin(); it != varMap.end(); ++it) {
      // assign temporary to output
      ShVariableNodePtr oldNode = it->first; 
      if(oldNode->kind() == SH_VAR_OUTPUT) {
        oldExit->block->addStatement(ShStatement(
              ShVariable(oldNode), SH_OP_ASN, ShVariable(it->second)));
      } else {
        oldEntry->block->addStatement(ShStatement(
              ShVariable(it->second), SH_OP_ASN, ShVariable(oldNode)));
      }
    }
  }

  ShTransformer& transformer;
  bool& changed;
  bool doIn, doOut;
  ShVariableReplacer::VarMap varMap; // maps from outputs used as srcs in computation to their temporary variables
};

void ShTransformer::inputOutputConversion(bool& changed)
{
  InputOutputConvertor ioc(*this, changed);
  m_graph->dfs(ioc);

  ShVariableReplacer vr(ioc.varMap);
  m_graph->dfs(vr);

  ioc.updateGraph(); 
}

}

