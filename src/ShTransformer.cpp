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
#include "ShEnvironment.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShVariableNode.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"

namespace SH {

ShTransformer::ShTransformer(ShProgram program)
  : m_program(program), m_graph(program->ctrlGraph), m_changed(false)
{
}

ShTransformer::~ShTransformer()
{
}

bool ShTransformer::changed()  { return m_changed; }

// Variable splitting, marks statements for which some variable is split 
struct VariableSplitter {

  VariableSplitter(int maxTuple, ShTransformer::VarSplitMap& splits, bool& changed)
    : maxTuple(maxTuple), splits(splits), changed(changed) {}

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      splitVars(*I);
    }
  }

  void splitVarList(ShProgramNode::VarList &vars) {
    for(ShProgramNode::VarList::iterator I = vars.begin();
        I != vars.end();) {
      if(split(*I)) {
        I = vars.erase(I); 
        ++I;
        vars.insert(I, splits[*I].begin(), splits[*I].end());
      } else ++I;
    }
  }
  
  void splitVars(ShStatement& stmt) {
    stmt.marked = false;
    if(stmt.dest.node()) stmt.marked = split(stmt.dest.node()) || stmt.marked;
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) stmt.marked = split(stmt.src[i].node()) || stmt.marked; 
  }

  // returns true if variable split
  bool split(ShVariableNodePtr node)
  {
    int i, offset;
    int n = node->size();
    if(n <= maxTuple ) return false;
    else if(splits.count(node) > 0) return true;
    if( node->kind() == SH_TEXTURE || node->kind() == SH_STREAM ) {
      ShError( ShTransformerException(
            "Long tuple support is not implemented for textures or streams"));
            
    }
    changed = true;
    ShTransformer::VarNodeVec &nodeVarNodeVec = splits[node];
    ShVariableNodePtr newNode;
    for(offset = 0; n > 0; offset += maxTuple, n -= maxTuple) {
      if(node->uniform()) ShEnvironment::insideShader = false;
      newNode = new ShVariableNode(node->kind(), n < maxTuple ? n : maxTuple, node->specialType());
      newNode->name(node->name());
      if(node->uniform()) ShEnvironment::insideShader = true;

      if( node->hasValues() ) { 
        for(i = 0; i < newNode->size(); ++i){
          newNode->setValue(i, node->getValue(offset + i));
        }
      }
      nodeVarNodeVec.push_back( newNode );
    }
    return true;
  }

  int maxTuple;
  ShTransformer::VarSplitMap &splits;
  bool& changed;
};

struct StatementSplitter {
  typedef std::vector<ShVariable> VarVec;

  StatementSplitter(int maxTuple, ShTransformer::VarSplitMap &splits, bool& changed)
    : maxTuple(maxTuple), splits(splits), changed(changed) {}

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
    std::size_t i, j, k, n;
    const ShSwizzle &swiz = v.swizzle();
    
    // get VarNodeVec for src
    ShTransformer::VarNodeVec srcVec;
    if(splits.count(v.node()) > 0) {
      srcVec = splits[v.node()];
    } else srcVec.push_back(v.node());

    // make and assign to a VarVec for temps
    for(i = 0, n = v.size(); n > 0; i += maxTuple, n -= maxTuple) {
      std::size_t tsize = (int)n < maxTuple ? n : maxTuple;
      //TODO  make this smarter so that it reuses variable nodes if it's just reswizlling a src node
      // (check that move elimination doesn't do this for us already)
      ShVariable tempVar(new ShVariableNode(SH_TEMP, tsize, SH_ATTRIB));
      vv.push_back(tempVar);

      int tempSwiz[tsize];
      int srcSwiz[tsize];
      int tempSize;
      for(j = 0; j < srcVec.size(); ++j) {
        tempSize = 0;
        for(k = 0; k < tsize; ++k ) { 
          if(swiz[i + k] / maxTuple == (int)j) {
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
  void movToDest(ShTransformer::VarNodeVec &destVec, const ShSwizzle &destSwiz, 
      const VarVec &resultVec, ShBasicBlock::ShStmtList &stmts) {
    std::size_t j;
    int k;
    int offset = 0;
    int swizd[maxTuple];
    int swizr[maxTuple];
    int size;
    for(VarVec::const_iterator I = resultVec.begin(); I != resultVec.end(); 
        offset += I->size(), ++I) {
      for(j = 0; j < destVec.size(); ++j) {
        size = 0;
        for(k = 0; k < I->size(); ++k) {
          if( destSwiz[k + offset] / maxTuple == (int)j) {
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
    std::size_t i, j;
    ShVariable &dest = oldStmt.dest;
    const ShSwizzle &destSwiz = dest.swizzle();
    ShTransformer::VarNodeVec destVec;
    VarVec resultVec;

    if(splits.count(dest.node()) > 0) {
      destVec = splits[dest.node()];
    } else destVec.push_back(dest.node());

    switch(oldStmt.op) {
      case SH_OP_DOT:
        { 
          // TODO for large tuples, may want to use another dot to sum up results instead of 
          // SH_OP_ADD. For now, do naive method
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          ShVariable dott = ShVariable(new ShVariableNode(SH_TEMP, 1, SH_ATTRIB)); 
          ShVariable sumt = ShVariable(new ShVariableNode(SH_TEMP, 1, SH_ATTRIB)); 
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
          ShVariable result = ShVariable(new ShVariableNode(SH_TEMP, 3, SH_ATTRIB));
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
            ShVariable resultPart(new ShVariableNode(SH_TEMP, srcVec[maxi][i].size(), SH_ATTRIB));
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
    int i;
    if(!stmt.marked && stmt.dest.size() <= maxTuple) {
      for(i = 0; i < 3; ++i) if(stmt.src[i].size() > maxTuple) break;
      if(i == 3) { // nothing needs splitting
        ++stit;
        return; 
      }
    }
    changed = true;
    ShBasicBlock::ShStmtList newStmts;
    VarVec srcVec[3];

    for(i = 0; i < 3; ++i) if(stmt.src[i].node()) makeSrcTemps(stmt.src[i], srcVec[i], newStmts);
    updateStatement(stmt, srcVec, newStmts);

    // remove old statmeent and splice in new statements
    stit = block->erase(stit);
    block->splice(stit, newStmts);
  }

  int maxTuple;
  ShTransformer::VarSplitMap &splits;
  bool& changed;
};

void ShTransformer::splitTuples(int maxTuple, ShTransformer::VarSplitMap &splits) {
  SH_DEBUG_ASSERT(maxTuple > 0); 

  VariableSplitter vs(maxTuple, splits, m_changed);
  vs.splitVarList(m_program->inputs);
  vs.splitVarList(m_program->outputs);
  m_graph->dfs(vs);


  StatementSplitter ss(maxTuple, splits, m_changed);
  m_graph->dfs(ss);
}

static int id = 0;

// Output Convertion to temporaries 
struct InputOutputConvertor {
  InputOutputConvertor(ShProgram program, ShVariableReplacer::VarMap &varMap, bool& changed)
    : m_program(program), m_graph(program->ctrlGraph), m_varMap( varMap ), m_changed(changed), m_id(++id)
  {}

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convertIO(*I);
    }
  }

  /* Convert all INOUT nodes that appear in a VarList (use std::for_each with this object)
   * (currently InOuts are always converted) */ 
  void operator()(ShVariableNodePtr node) {
    if (node->kind() != SH_INOUT || m_varMap.count(node) > 0) return;
    m_varMap[node] = dupNode(node);
  }

  // dup that works only on nodes without values (inputs, outputs fall into this category)
  ShVariableNodePtr dupNode(ShVariableNodePtr node, ShVariableKind newKind = SH_TEMP) {
    ShVariableNodePtr result( new ShVariableNode(newKind,
          node->size(), node->specialType()));
    result->name(node->name());
    return result;
  }
  
  // Convert inputs, outputs only when they appear in incompatible locations
  // (inputs used as dest, outputs used as src)
  void convertIO(ShStatement& stmt)
  {
    if(!stmt.dest.null()) {
      ShVariableNodePtr &oldNode = stmt.dest.node();
      if(oldNode->kind() == SH_INPUT) { 
        if(m_varMap.count(oldNode) == 0) {
          m_varMap[oldNode] = dupNode(oldNode); 
        }
      }
    }
    for(int i = 0; i < 3; ++i) {
      if(!stmt.src[i].null()) {
        ShVariableNodePtr &oldNode = stmt.src[i].node();
        if(oldNode->kind() == SH_OUTPUT) { 
          if(m_varMap.count(oldNode) == 0) {
            m_varMap[oldNode] = dupNode(oldNode); 
          }
        }
      }
    }
  }

  void updateGraph() {
    if(m_varMap.empty()) return;
    m_changed = true;

    // create block after exit
    ShCtrlGraphNodePtr oldExit = m_graph->appendExit(); 
    ShCtrlGraphNodePtr oldEntry = m_graph->prependEntry();

    for(ShVariableReplacer::VarMap::const_iterator it = m_varMap.begin(); it != m_varMap.end(); ++it) {
      // assign temporary to output
      ShVariableNodePtr oldNode = it->first; 
      if(oldNode->kind() == SH_OUTPUT) {
        oldExit->block->addStatement(ShStatement(
              ShVariable(oldNode), SH_OP_ASN, ShVariable(it->second)));
      } else if(oldNode->kind() == SH_INPUT) {
        oldEntry->block->addStatement(ShStatement(
              ShVariable(it->second), SH_OP_ASN, ShVariable(oldNode)));
      } else if(oldNode->kind() == SH_INOUT) {
        // replace INOUT nodes in input/output lists with INPUT and OUTPUT nodes
        ShVariableNodePtr newInNode(dupNode(oldNode, SH_INPUT));
        ShVariableNodePtr newOutNode(dupNode(oldNode, SH_OUTPUT));

        std::replace(m_program->inputs.begin(), m_program->inputs.end(),
            oldNode, newInNode);
        m_program->inputs.pop_back();

        std::replace(m_program->outputs.begin(), m_program->outputs.end(),
            oldNode, newOutNode);
        m_program->outputs.pop_back();

        // add mov statements to/from temporary 
        oldEntry->block->addStatement(ShStatement(
              ShVariable(it->second), SH_OP_ASN, ShVariable(newInNode)));
        oldExit->block->addStatement(ShStatement(
              ShVariable(newOutNode), SH_OP_ASN, ShVariable(it->second)));
      }
    }
  }

  ShProgram m_program;
  ShCtrlGraphPtr m_graph;
  ShVariableReplacer::VarMap &m_varMap; // maps from outputs used as srcs in computation to their temporary variables
  bool& m_changed;
  int m_id;
};

void ShTransformer::convertInputOutput()
{
  ShVariableReplacer::VarMap varMap; // maps from outputs used as srcs in computation to their temporary variables

  InputOutputConvertor ioc(m_program, varMap, m_changed);
  std::for_each(m_program->inputs.begin(), m_program->inputs.end(), ioc);
  std::for_each(m_program->outputs.begin(), m_program->outputs.end(), ioc);
  m_graph->dfs(ioc);

  ShVariableReplacer vr(varMap);
  m_graph->dfs(vr);

  ioc.updateGraph(); 
}

struct TextureLookupConverter {
  TextureLookupConverter() : changed(false) {}
  
  void operator()(const ShCtrlGraphNodePtr& node)
  {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convert(block, I);
    }
  }

  void convert(ShBasicBlockPtr block, ShBasicBlock::ShStmtList::iterator& I)
  {
    const ShStatement& stmt = *I;
    if (stmt.op != SH_OP_TEX && stmt.op != SH_OP_TEXI) return;
    ShTextureNodePtr tn = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());

    ShBasicBlock::ShStmtList newStmts;
    
    if (!tn) { SH_DEBUG_ERROR("TEX Instruction from non-texture"); return; }
    if (stmt.op == SH_OP_TEX && tn->dims() == SH_TEXTURE_RECT) {
      ShVariable tc(new ShVariableNode(SH_TEMP, tn->texSizeVar().size()));
      newStmts.push_back(ShStatement(tc, stmt.src[1], SH_OP_MUL, tn->texSizeVar()));
      newStmts.push_back(ShStatement(stmt.dest, stmt.src[0], SH_OP_TEXI, tc));
    } else if (stmt.op == SH_OP_TEXI && tn->dims() != SH_TEXTURE_RECT) {
      ShVariable tc(new ShVariableNode(SH_TEMP, tn->texSizeVar().size()));
      newStmts.push_back(ShStatement(tc, stmt.src[1], SH_OP_DIV, tn->texSizeVar()));
      newStmts.push_back(ShStatement(stmt.dest, stmt.src[0], SH_OP_TEX, tc));
    } else {
      return;
    }
    I = block->erase(I); // I is pointing one past its original value now
    block->splice(I, newStmts);
    I--; // Make I point to its original value, it will be inc'd later.
    changed = true;
    return;
  }

  bool changed;
};

void ShTransformer::convertTextureLookups()
{
  TextureLookupConverter conv;
  m_graph->dfs(conv);
  if (conv.changed) m_changed = true;
}

}

