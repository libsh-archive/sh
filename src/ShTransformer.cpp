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
#include <sstream>
#include <map>
#include <list>
#include "ShContext.hpp"
#include "ShError.hpp"
#include "ShDebug.hpp"
#include "ShVariant.hpp"
#include "ShVariableNode.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"

// #define SH_DBG_TRANSFORMER

namespace SH {

ShTransformer::ShTransformer(const ShProgramNodePtr& program)
  : m_program(program), m_changed(false)
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

  // this must be called BEFORE running a DFS on the program
  // to split temporaries (otherwise the stupid hack marked (#) does not work)
  void splitVarList(ShProgramNode::VarList &vars) {
    for(ShProgramNode::VarList::iterator I = vars.begin();
        I != vars.end();) {
      if(split(*I)) {
        // (#) erase the stuff that split added to the end of the var list
        // TODO check if this is actually no longer needed. 
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
    int i, offset;
    int n = node->size();
    if(n <= maxTuple ) return false;
    else if(splits.count(node) > 0) return true;
    if( node->kind() == SH_TEXTURE || node->kind() == SH_STREAM ) {
      shError( ShTransformerException(
            "Long tuple support is not implemented for textures or streams"));
            
    }
    changed = true;
    ShTransformer::VarNodeVec &nodeVarNodeVec = splits[node];
    ShVariableNodePtr newNode;
    int* copySwiz = new int[maxTuple];
    for(offset = 0; n > 0; offset += maxTuple, n -= maxTuple) {
      ShProgramNodePtr prev = ShContext::current()->parsing();
      // @todo type should not be necessary any more
      //if(node->uniform()) ShContext::current()->exit(); 

      int newSize = n < maxTuple ? n : maxTuple;
      newNode = node->clone(SH_BINDINGTYPE_END, newSize, 
          SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false); 
      std::ostringstream sout;
      sout << node->name() << "_" << offset;
      newNode->name(sout.str());

      // @todo type should not be necessary any more
      // if(node->uniform()) ShContext::current()->enter(0);

      if( node->hasValues() ) { 
        // @todo type set up dependent uniforms here 
        for(i = 0; i < newSize; ++i) copySwiz[i] = offset + i;
        ShVariantCPtr subVariant = node->getVariant()->get(false,
            ShSwizzle(node->size(), newSize, copySwiz));
        newNode->setVariant(subVariant);
      }
      nodeVarNodeVec.push_back( newNode );
    }
	delete [] copySwiz;
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
    std::size_t i, j, k;
    int n;
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
      
      // TODO check that uniforms don't get screwed
      // TODO check that typing works correctly - temporary should
      // have same type as the statement's operation type
      ShVariable tempVar(resizeCloneNode(v.node(), tsize));
      std::ostringstream sout;
      sout << v.name() << "_" << i << "_temp";
      tempVar.name(sout.str());
      vv.push_back(tempVar);

      int* tempSwiz = new int[tsize];
      int* srcSwiz = new int[tsize];
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
      delete [] tempSwiz;
      delete [] srcSwiz;
    }
  }

  // moves the result to the destination based on the destination swizzle
  void movToDest(ShTransformer::VarNodeVec &destVec, const ShSwizzle &destSwiz, 
      const VarVec &resultVec, ShBasicBlock::ShStmtList &stmts) {
    std::size_t j;
    int k;
    int offset = 0;
    int* swizd = new int[maxTuple];
    int* swizr = new int[maxTuple];
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
    delete [] swizd;
    delete [] swizr;
  }

  ShVariableNodePtr resizeCloneNode(ShVariableNodePtr node, int newSize) {
    return node->clone(SH_TEMP, newSize, SH_VALUETYPE_END, 
        SH_SEMANTICTYPE_END, true, false);
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
      case SH_OP_CSUM:
        {
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          ShVariable partialt = ShVariable(resizeCloneNode(dest.node(), 1));
          ShVariable sumt = ShVariable(resizeCloneNode(dest.node(), 1));

          stmts.push_back(ShStatement(sumt, SH_OP_CSUM, srcVec[0][0]));
          for(size_t i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(ShStatement(partialt, SH_OP_CSUM, srcVec[0][i]));
            stmts.push_back(ShStatement(sumt, sumt, SH_OP_ADD, partialt));
          }
          resultVec.push_back(sumt);
        }
        break;
      case SH_OP_CMUL:
        {
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          ShVariable partialt = ShVariable(resizeCloneNode(dest.node(), 1));
          ShVariable prodt = ShVariable(resizeCloneNode(dest.node(), 1));

          stmts.push_back(ShStatement(prodt, SH_OP_CMUL, srcVec[0][0]));
          for(size_t i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(ShStatement(partialt, SH_OP_CMUL, srcVec[0][i]));
            stmts.push_back(ShStatement(prodt, prodt, SH_OP_MUL, partialt));
          }
          resultVec.push_back(prodt);
        }
        break;
      case SH_OP_DOT:
        { 
          // TODO for large tuples, may want to use another dot to sum up results instead of 
          // SH_OP_ADD. For now, do naive method
          SH_DEBUG_ASSERT(destSwiz.size() == 1);

          // TODO check that this works correctly for weird types
          // (temporaries should have same type as the ShStatement's operation type) 
          ShVariable dott = ShVariable(resizeCloneNode(dest.node(), 1));
          ShVariable sumt = ShVariable(resizeCloneNode(dest.node(), 1));

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

          // TODO check typing
          ShVariable result = ShVariable(resizeCloneNode(dest.node(), 3));

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
            // TODO check typing
            ShVariable resultPart(resizeCloneNode(dest.node(), srcVec[maxi][i].size()));

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
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("splittupl_start");
#endif

  VariableSplitter vs(maxTuple, splits, m_changed);
  vs.splitVarList(m_program->inputs);
  vs.splitVarList(m_program->outputs);
  m_program->ctrlGraph->dfs(vs);

#ifdef SH_DBG_TRANSFORMER
  m_program->dump("splittupl_vars");
#endif

  StatementSplitter ss(maxTuple, splits, m_changed);
  m_program->ctrlGraph->dfs(ss);

#ifdef SH_DBG_TRANSFORMER
  m_program->dump("splittupl_done");
#endif
}

static int id = 0;

// Output Convertion to temporaries 
struct InputOutputConvertor {
  InputOutputConvertor(const ShProgramNodePtr& program,
                       ShVarMap &varMap, bool& changed)
    : m_program(program), m_varMap( varMap ), m_changed(changed), m_id(++id)
  {}

  void operator()(ShCtrlGraphNodePtr node) {
    if (!node) return;
    ShBasicBlockPtr block = node->block;
    if (!block) return;
    for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convertIO(*I);
    }
  }

  // Turn node into a temporary, but do not update var list and do not keep
  // uniform
  ShVariableNodePtr cloneNode(ShVariableNodePtr node, const char* suffix, ShBindingType binding_type=SH_TEMP) {
    ShVariableNodePtr result = node->clone(binding_type, 0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, false, false);
    result->name(node->name() + suffix); 
    return result;
  }

  /* Convert all INOUT nodes that appear in a VarList (use std::for_each with this object)
   * (currently InOuts are always converted) */ 
  void operator()(ShVariableNodePtr node) {
    if (node->kind() != SH_INOUT || m_varMap.count(node) > 0) return;
    m_varMap[node] = cloneNode(node, "_ioc-iot");
  }

  // Convert inputs, outputs only when they appear in incompatible locations
  // (inputs used as dest, outputs used as src)
  void convertIO(ShStatement& stmt)
  {
    if(!stmt.dest.null()) {
      const ShVariableNodePtr &oldNode = stmt.dest.node();
      if(oldNode->kind() == SH_INPUT) { 
        if(m_varMap.count(oldNode) == 0) {
          m_varMap[oldNode] = cloneNode(oldNode, "_ioc-it");
        }
      }
    }
    for(int i = 0; i < 3; ++i) {
      if(!stmt.src[i].null()) {
        const ShVariableNodePtr &oldNode = stmt.src[i].node();
        if(oldNode->kind() == SH_OUTPUT) { 
          if(m_varMap.count(oldNode) == 0) {
            m_varMap[oldNode] = cloneNode(oldNode, "_ioc-ot");
          }
        }
      }
    }
  }

  void updateGraph() {
    if(m_varMap.empty()) return;
    m_changed = true;

    // create block after exit
    ShCtrlGraphNodePtr oldExit = m_program->ctrlGraph->appendExit(); 
    ShCtrlGraphNodePtr oldEntry = m_program->ctrlGraph->prependEntry();

    for(ShVarMap::const_iterator it = m_varMap.begin(); it != m_varMap.end(); ++it) {
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
        ShVariableNodePtr newInNode(cloneNode(oldNode, "_ioc-i", SH_INPUT));
        ShVariableNodePtr newOutNode(cloneNode(oldNode, "_ioc-o", SH_OUTPUT));

        std::replace(m_program->inputs.begin(), m_program->inputs.end(),
            oldNode, newInNode);

        std::replace(m_program->outputs.begin(), m_program->outputs.end(),
            oldNode, newOutNode);

        // add mov statements to/from temporary 
        oldEntry->block->addStatement(ShStatement(
              ShVariable(it->second), SH_OP_ASN, ShVariable(newInNode)));
        oldExit->block->addStatement(ShStatement(
              ShVariable(newOutNode), SH_OP_ASN, ShVariable(it->second)));
      }
    }
  }

  ShProgramNodePtr m_program;
  ShVarMap &m_varMap; // maps from outputs used as srcs in computation to their temporary variables
  bool& m_changed;
  int m_id;
};

void ShTransformer::convertInputOutput()
{
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("ioconvert_start");
#endif
  ShVarMap varMap; // maps from outputs used as srcs in computation to their temporary variables

  InputOutputConvertor ioc(m_program, varMap, m_changed);
  std::for_each(m_program->inputs.begin(), m_program->inputs.end(), ioc);
  std::for_each(m_program->outputs.begin(), m_program->outputs.end(), ioc);
  m_program->ctrlGraph->dfs(ioc);

  ShVariableReplacer vr(varMap);
  m_program->ctrlGraph->dfs(vr);

  ioc.updateGraph(); 

#ifdef SH_DBG_TRANSFORMER
  m_program->dump("ioconvert_done");
#endif
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

  ShVariableNodePtr cloneNode(ShVariableNodePtr node) {
    return node->clone(SH_TEMP, 0, SH_VALUETYPE_END, SH_SEMANTICTYPE_END, true, false);
  }

  void convert(ShBasicBlockPtr block, ShBasicBlock::ShStmtList::iterator& I)
  {
    const ShStatement& stmt = *I;
    if (stmt.op != SH_OP_TEX && stmt.op != SH_OP_TEXI) return;
    ShTextureNodePtr tn = shref_dynamic_cast<ShTextureNode>(stmt.src[0].node());

    ShBasicBlock::ShStmtList newStmts;
    
    if (!tn) { SH_DEBUG_ERROR("TEX Instruction from non-texture"); return; }
    if (stmt.op == SH_OP_TEX && tn->dims() == SH_TEXTURE_RECT) {
      // TODO check typing
      //ShVariable tc(new ShVariableNode(SH_TEMP, tn->texSizeVar().size()));
      ShVariable tc(cloneNode(tn->texSizeVar().node()));

      newStmts.push_back(ShStatement(tc, stmt.src[1], SH_OP_MUL, tn->texSizeVar()));
      newStmts.push_back(ShStatement(stmt.dest, stmt.src[0], SH_OP_TEXI, tc));
    } else if (stmt.op == SH_OP_TEXI && tn->dims() != SH_TEXTURE_RECT) {
      // TODO check typing
      //ShVariable tc(new ShVariableNode(SH_TEMP, tn->texSizeVar().size()));
      ShVariable tc(cloneNode(tn->texSizeVar().node()));

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
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("texlkup_start");
#endif

  TextureLookupConverter conv;
  m_program->ctrlGraph->dfs(conv);
  if (conv.changed) m_changed = true;

#ifdef SH_DBG_TRANSFORMER
  m_program->dump("texlkup_done");
#endif
}

struct DummyOpStripperBase: public ShTransformerParent 
{
 bool handleStmt(ShBasicBlock::ShStmtList::iterator &I, ShCtrlGraphNodePtr node) { 
   switch(I->op) {
     case SH_OP_STARTSEC:
     case SH_OP_ENDSEC:
     case SH_OP_COMMENT:
       I = node->block->erase(I);
       return true;
     default:
       break;
   }
   return false; 
 }
};
typedef ShDefaultTransformer<DummyOpStripperBase> DummyOpStripper;

void ShTransformer::stripDummyOps()
{
  DummyOpStripper dos;
  m_changed |= dos.transform(m_program);
}

}

