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
#include <sstream>
#include <map>
#include <list>
#include "Context.hpp"
#include "Error.hpp"
#include "Debug.hpp"
#include "Variant.hpp"
#include "VariableNode.hpp"
#include "Internals.hpp"
#include "Transformer.hpp"
#include "TextureNode.hpp"

// #define DBG_TRANSFORMER

#define Pi 3.14159265

namespace SH {

Transformer::Transformer(const ProgramNodePtr& program)
  : m_program(program), m_changed(false)
{
}

Transformer::~Transformer()
{
}

bool Transformer::changed()  { return m_changed; }

// Variable splitting, marks statements for which some variable is split 
struct VariableSplitter {

  VariableSplitter(int maxTuple, Transformer::VarSplitMap& splits, bool& changed)
    : maxTuple(maxTuple), splits(splits), changed(changed) {}

  // assignment operator could not be generated: declaration only
  VariableSplitter& operator=(VariableSplitter const&);

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
        // (#) erase the stuff that split added to the end of the var list
        // TODO check if this is actually no longer needed. 
        //vars.resize(vars.size() - splits[*I].size());

        vars.insert(I, splits[*I].begin(), splits[*I].end());
        I = vars.erase(I); 
      } else ++I;
    }
  }
  
  void splitVars(Statement& stmt) {
    stmt.marked = false;
    if(stmt.dest.node()) stmt.marked = split(stmt.dest.node()) || stmt.marked;
    for(int i = 0; i < 3; ++i) if(stmt.src[i].node()) stmt.marked = split(stmt.src[i].node()) || stmt.marked; 
  }

  // returns true if variable split
  // does not add variable to Program's VarList, so this must be handled manually 
  // (since this matters only for IN/OUT/SH_INOUT types, splitVarList handles the
  // insertions nicely)
  bool split(const VariableNodePtr& node)
  {
    int i, offset;
    int n = node->size();
    if(n <= maxTuple ) return false;
    else if(splits.count(node) > 0) return true;
    if( node->kind() == SH_TEXTURE) {
      error( TransformerException(
            "Long tuple support is not implemented for textures"));
            
    }
    changed = true;
    Transformer::VarNodeVec &nodeVarNodeVec = splits[node];
    VariableNodePtr newNode;
    int* copySwiz = new int[maxTuple];
    for(offset = 0; n > 0; offset += maxTuple, n -= maxTuple) {
      ProgramNodePtr prev = Context::current()->parsing();
      // @todo type should not be necessary any more
      //if(node->uniform()) Context::current()->exit(); 

      int newSize = n < maxTuple ? n : maxTuple;
      newNode = node->clone(BINDINGTYPE_END, newSize, 
          VALUETYPE_END, SEMANTICTYPE_END, false); 
      std::ostringstream sout;
      sout << node->name() << "_" << offset;
      newNode->name(sout.str());

      // @todo type should not be necessary any more
      // if(node->uniform()) Context::current()->enter(0);

      if( node->hasValues() ) { 
        // @todo type set up dependent uniforms here 
        for(i = 0; i < newSize; ++i) copySwiz[i] = offset + i;
        VariantCPtr subVariant = node->getVariant()->get(false,
            Swizzle(node->size(), newSize, copySwiz));
        newNode->setVariant(subVariant);
      }
      nodeVarNodeVec.push_back( newNode );
    }
	delete [] copySwiz;
    return true;
  }

  int maxTuple;
  Transformer::VarSplitMap &splits;
  bool& changed;
};

struct StatementSplitter {
  typedef std::vector<Variable> VarVec;

  StatementSplitter(int maxTuple, Transformer::VarSplitMap &splits, bool& changed)
    : maxTuple(maxTuple), splits(splits), changed(changed) {}

  // assignment operator could not be generated: declaration only
  StatementSplitter& operator=(StatementSplitter const&);

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end();) {
      splitStatement(block, I);
    }
  }

  void makeSrcTemps(const Variable &v, VarVec &vv, BasicBlock::StmtList &stmts) {
    if( v.size() <= maxTuple && v.node()->size() <= maxTuple ) {
      vv.push_back(v);
      return;
    }
    std::size_t i, j, k;
    int n;
    const Swizzle &swiz = v.swizzle();
    
    // get VarNodeVec for src
    Transformer::VarNodeVec srcVec;
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
      Variable tempVar(resizeCloneNode(v.node(), tsize));
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
          Variable srcVar(srcVec[j]);
          stmts.push_back(Statement(tempVar(tempSize, tempSwiz), OP_ASN, srcVar(tempSize, srcSwiz)));
        }
      }
      delete [] tempSwiz;
      delete [] srcSwiz;
    }
  }

  // moves the result to the destination based on the destination swizzle
  void movToDest(Transformer::VarNodeVec &destVec, const Swizzle &destSwiz, 
      const VarVec &resultVec, BasicBlock::StmtList &stmts) {
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
          Variable destVar(destVec[j]);
          stmts.push_back(Statement(destVar(size, swizd), OP_ASN, (*I)(size, swizr)));
        }
      }
    }
    delete [] swizd;
    delete [] swizr;
  }

  VariableNodePtr resizeCloneNode(const VariableNodePtr& node, int newSize) {
    return node->clone(SH_TEMP, newSize, VALUETYPE_END, 
        SEMANTICTYPE_END, true, false);
  }
  // works on two assumptions
  // 1) special cases for DOT, XPD (and any other future non-componentwise ops) implemented separately
  // 2) Everything else is in the form N = [1|N]+ in terms of tuple sizes involved in dest and src
  void updateStatement(Statement &oldStmt, VarVec srcVec[3], BasicBlock::StmtList &stmts) {
    std::size_t i, j;
    Variable &dest = oldStmt.dest;
    const Swizzle &destSwiz = dest.swizzle();
    Transformer::VarNodeVec destVec;
    VarVec resultVec;

    if(splits.count(dest.node()) > 0) {
      destVec = splits[dest.node()];
    } else destVec.push_back(dest.node());

    switch(oldStmt.op) {
      case OP_CSUM:
        {
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          Variable partialt = Variable(resizeCloneNode(dest.node(), 1));
          Variable sumt = Variable(resizeCloneNode(dest.node(), 1));

          stmts.push_back(Statement(sumt, OP_CSUM, srcVec[0][0]));
          for(size_t i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(Statement(partialt, OP_CSUM, srcVec[0][i]));
            stmts.push_back(Statement(sumt, sumt, OP_ADD, partialt));
          }
          resultVec.push_back(sumt);
        }
        break;
      case OP_CMUL:
        {
          SH_DEBUG_ASSERT(destSwiz.size() == 1);
          Variable partialt = Variable(resizeCloneNode(dest.node(), 1));
          Variable prodt = Variable(resizeCloneNode(dest.node(), 1));

          stmts.push_back(Statement(prodt, OP_CMUL, srcVec[0][0]));
          for(size_t i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(Statement(partialt, OP_CMUL, srcVec[0][i]));
            stmts.push_back(Statement(prodt, prodt, OP_MUL, partialt));
          }
          resultVec.push_back(prodt);
        }
        break;
      case OP_DOT:
        { 
          // TODO for large tuples, may want to use another dot to sum up results instead of 
          // OP_ADD. For now, do naive method
          SH_DEBUG_ASSERT(destSwiz.size() == 1);

          // TODO check that this works correctly for weird types
          // (temporaries should have same type as the Statement's operation type) 
          Variable dott = Variable(resizeCloneNode(dest.node(), 1));
          Variable sumt = Variable(resizeCloneNode(dest.node(), 1));

          stmts.push_back(Statement(sumt, srcVec[0][0], OP_DOT, srcVec[1][0]));
          for(i = 1; i < srcVec[0].size(); ++i) {
            stmts.push_back(Statement(dott, srcVec[0][i], OP_DOT, srcVec[1][i]));
            stmts.push_back(Statement(sumt, sumt, OP_ADD, dott));
          }
          resultVec.push_back(sumt);
        }
        break;
      case OP_XPD:
        {
          SH_DEBUG_ASSERT( srcVec[0].size() == 1 && srcVec[0][0].size() == 3 &&
              srcVec[1].size() == 1 && srcVec[1][0].size() == 3); 

          // TODO check typing
          Variable result = Variable(resizeCloneNode(dest.node(), 3));

          stmts.push_back(Statement(result, srcVec[0][0], OP_XPD, srcVec[1][0]));
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
            Variable resultPart(resizeCloneNode(dest.node(), srcVec[maxi][i].size()));

            Statement newStmt(resultPart, oldStmt.op);
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
  void splitStatement(const BasicBlockPtr& block, BasicBlock::StmtList::iterator &stit) {
    Statement &stmt = *stit;
    int i;
    if(!stmt.marked && stmt.dest.size() <= maxTuple) {
      for(i = 0; i < 3; ++i) if(stmt.src[i].size() > maxTuple) break;
      if(i == 3) { // nothing needs splitting
        ++stit;
        return; 
      }
    }
    changed = true;
    BasicBlock::StmtList newStmts;
    VarVec srcVec[3];

    for(i = 0; i < 3; ++i) if(stmt.src[i].node()) makeSrcTemps(stmt.src[i], srcVec[i], newStmts);
    updateStatement(stmt, srcVec, newStmts);

    // remove old statmeent and splice in new statements
    stit = block->erase(stit);
    block->splice(stit, newStmts);
  }

  int maxTuple;
  Transformer::VarSplitMap &splits;
  bool& changed;
};

void Transformer::splitTuples(int maxTuple, Transformer::VarSplitMap &splits) {
  SH_DEBUG_ASSERT(maxTuple > 0); 
#ifdef DBG_TRANSFORMER
  m_program->dump("splittupl_start");
#endif

  VariableSplitter vs(maxTuple, splits, m_changed);
  vs.splitVarList(m_program->inputs);
  vs.splitVarList(m_program->outputs);
  m_program->ctrlGraph->dfs(vs);

#ifdef DBG_TRANSFORMER
  m_program->dump("splittupl_vars");
#endif

  StatementSplitter ss(maxTuple, splits, m_changed);
  m_program->ctrlGraph->dfs(ss);

#ifdef DBG_TRANSFORMER
  m_program->dump("splittupl_done");
#endif
}

static int id = 0;

// Output Convertion to temporaries 
struct InputOutputConvertor {
  InputOutputConvertor(const ProgramNodePtr& program,
                       VarMap &varMap, bool& changed)
    : m_program(program), m_varMap( varMap ), m_changed(changed), m_id(++id)
  {}

  // assignment operator could not be generated: declaration only
  InputOutputConvertor& operator=(InputOutputConvertor const&);

  void operator()(CtrlGraphNode* node) {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convertIO(*I);
    }
  }

  // Turn node into a temporary, but do not update var list and do not keep
  // uniform
  VariableNodePtr cloneNode(const VariableNodePtr& node, const char* suffix, BindingType binding_type=SH_TEMP) {
    VariableNodePtr result = node->clone(binding_type, 0, VALUETYPE_END, SEMANTICTYPE_END, false, false);
    result->name(node->name() + suffix); 
    return result;
  }

  /* Convert all SH_INOUT nodes that appear in a VarList (use std::for_each with this object)
   * (currently InOuts are always converted) */ 
  void operator()(const VariableNodePtr& node) {
    if (node->kind() != SH_INOUT || m_varMap.count(node) > 0) return;
    m_varMap[node] = cloneNode(node, "_ioc-iot");
  }

  // Convert inputs, outputs only when they appear in incompatible locations
  // (inputs used as dest, outputs used as src)
  void convertIO(Statement& stmt)
  {
    if(!stmt.dest.null()) {
      const VariableNodePtr &oldNode = stmt.dest.node();
      if(oldNode->kind() == SH_INPUT) { 
        if(m_varMap.count(oldNode) == 0) {
          m_varMap[oldNode] = cloneNode(oldNode, "_ioc-it");
        }
      }
      if(oldNode->uniform()) { 
        if(m_varMap.count(oldNode) == 0) {
          m_varMap[oldNode] = cloneNode(oldNode, "_ioc-ut");
        }
      }
    }
    for(int i = 0; i < 3; ++i) {
      if(!stmt.src[i].null()) {
        const VariableNodePtr &oldNode = stmt.src[i].node();
        if(oldNode->kind() == SH_OUTPUT) { 
          if(m_varMap.count(oldNode) == 0) {
            m_varMap[oldNode] = cloneNode(oldNode, "_ioc-ot");
          }
        }
      }
    }
  }

  void updateGraph(VarTransformMap *varTransMap) {
    if(m_varMap.empty()) return;
    m_changed = true;

    // create block after exit
    CtrlGraphNode* oldExit = m_program->ctrlGraph->append_exit(); 
    CtrlGraphNode* oldEntry = m_program->ctrlGraph->prepend_entry();

    for(VarMap::const_iterator it = m_varMap.begin(); it != m_varMap.end(); ++it) {
      // assign temporary to output
      VariableNodePtr oldNode = it->first; 
      if(oldNode->kind() == SH_OUTPUT) {
        oldExit->block->addStatement(Statement(
              Variable(oldNode), OP_ASN, Variable(it->second)));
      } else if(oldNode->kind() == SH_INPUT) {
        oldEntry->block->addStatement(Statement(
              Variable(it->second), OP_ASN, Variable(oldNode)));
      } else if(oldNode->uniform()) {
        oldEntry->block->addStatement(Statement(
          Variable(it->second), OP_ASN, Variable(oldNode)));
      } else if(oldNode->kind() == SH_INOUT) {
        // replace SH_INOUT nodes in input/output lists with SH_INPUT and SH_OUTPUT nodes
        VariableNodePtr newInNode(cloneNode(oldNode, "_ioc-i", SH_INPUT));
        VariableNodePtr newOutNode(cloneNode(oldNode, "_ioc-o", SH_OUTPUT));

        std::replace(m_program->inputs.begin(), m_program->inputs.end(),
            oldNode, newInNode);

        std::replace(m_program->outputs.begin(), m_program->outputs.end(),
            oldNode, newOutNode);

        // we replaced original nodes from the inputs/outputs so store that in our
        // transform map so that the user can get back to the *real* originals if
        // they so wish.
        varTransMap->add_variable_transform(oldNode, newInNode);
        varTransMap->add_variable_transform(oldNode, newOutNode);

        // add mov statements to/from temporary 
        oldEntry->block->addStatement(Statement(
              Variable(it->second), OP_ASN, Variable(newInNode)));
        oldExit->block->addStatement(Statement(
              Variable(newOutNode), OP_ASN, Variable(it->second)));
      }
    }
  }

  ProgramNodePtr m_program;
  VarMap &m_varMap; // maps from outputs used as srcs in computation to their temporary variables
  bool& m_changed;
  int m_id;
};

void Transformer::convertInputOutput(VarTransformMap *varTransMap)
{
#ifdef DBG_TRANSFORMER
  m_program->dump("ioconvert_start");
#endif
  VarMap varMap; // maps from outputs used as srcs in computation to their temporary variables

  InputOutputConvertor ioc(m_program, varMap, m_changed);
  std::for_each(m_program->inputs.begin(), m_program->inputs.end(), ioc);
  std::for_each(m_program->outputs.begin(), m_program->outputs.end(), ioc);
  m_program->ctrlGraph->dfs(ioc);

  VariableReplacer vr(varMap);
  m_program->ctrlGraph->dfs(vr);

  // If they didn't provide a map, make a temporary one
  bool TempVarMap = false;
  if (!varTransMap) {
    varTransMap = new VarTransformMap();
    TempVarMap = true;
  }

  ioc.updateGraph(varTransMap);

  // Cleanup
  if (TempVarMap) {
    delete varTransMap;
    varTransMap = 0;
  }

#ifdef DBG_TRANSFORMER
  m_program->dump("ioconvert_done");
#endif
}

struct TextureLookupConverter {
  TextureLookupConverter() : changed(false) {}
  
  void operator()(CtrlGraphNode* node)
  {
    if (!node) return;
    BasicBlockPtr block = node->block;
    if (!block) return;
    for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end(); ++I) {
      convert(block, I);
    }
  }

  VariableNodePtr cloneNode(const VariableNodePtr& node) {
    return node->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false);
  }

  void convert(const BasicBlockPtr& block, BasicBlock::StmtList::iterator& I)
  {
    const Statement& stmt = *I;
    if (stmt.op != OP_TEX && stmt.op != OP_TEXI) return;
    TextureNodePtr tn = shref_dynamic_cast<TextureNode>(stmt.src[0].node());

    BasicBlock::StmtList newStmts;
    
    if (!tn) { SH_DEBUG_ERROR("TEX Instruction from non-texture"); return; }
    if (stmt.op == OP_TEX && tn->dims() == SH_TEXTURE_RECT) {
      // TODO check typing
      //Variable tc(new VariableNode(SH_TEMP, tn->texSizeVar().size()));
      Variable tc(cloneNode(tn->texSizeVar().node()));

      newStmts.push_back(Statement(tc, stmt.src[1], OP_MUL, tn->texSizeVar()));
      newStmts.push_back(Statement(stmt.dest, stmt.src[0], OP_TEXI, tc));
    } else if (stmt.op == OP_TEXI && tn->dims() != SH_TEXTURE_RECT) {
      // TODO check typing
      //Variable tc(new VariableNode(SH_TEMP, tn->texSizeVar().size()));
      Variable tc(cloneNode(tn->texSizeVar().node()));

      newStmts.push_back(Statement(tc, stmt.src[1], OP_DIV, tn->texSizeVar()));
      newStmts.push_back(Statement(stmt.dest, stmt.src[0], OP_TEX, tc));
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

void Transformer::convertTextureLookups()
{
#ifdef DBG_TRANSFORMER
  m_program->dump("texlkup_start");
#endif

  TextureLookupConverter conv;
  m_program->ctrlGraph->dfs(conv);
  if (conv.changed) m_changed = true;

#ifdef DBG_TRANSFORMER
  m_program->dump("texlkup_done");
#endif
}

struct DummyOpStripperBase: public TransformerParent 
{
 bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) { 
   switch(I->op) {
     case OP_STARTSEC:
     case OP_ENDSEC:
       I = node->block->erase(I);
       m_changed = true;
       return true;
     default:
       break;
   }
   return false; 
 }
};
typedef DefaultTransformer<DummyOpStripperBase> DummyOpStripper;

void Transformer::stripDummyOps()
{
  DummyOpStripper dos;
  m_changed |= dos.transform(m_program);
}

VariableNodePtr allocate_constant(const Variable& dest, double constant)
{
  const VariableNodePtr& dest_node = dest.node();
  VariableNode* node = new VariableNode(SH_CONST, dest_node->size(), 
                                            dest_node->valueType(), 
                                            dest_node->specialType());
    
  DataVariant<double>* variant = new DataVariant<double>(dest_node->size(), 
                                                             constant);
  VariantCPtr variant_ptr = variant;
  node->setVariant(variant_ptr);
  
  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

VariableNodePtr allocate_constant(unsigned int size, float* constants)
{
  VariableNode* node = new VariableNode(SH_CONST, size,
                                            SH_FLOAT,
                                            SH_ATTRIB);
    
  DataVariant<float>* variant = new DataVariant<float>(size,
                                                           constants);
  VariantCPtr variant_ptr = variant;
  node->setVariant(variant_ptr);
  
  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

VariableNodePtr allocate_temp(const Variable& dest)
{
  const VariableNodePtr& dest_node = dest.node();
  VariableNode* node = new VariableNode(SH_TEMP, dest_node->size(), 
                                            dest_node->valueType(), 
                                            dest_node->specialType());
  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

VariableNodePtr allocate_scalar_temp(const Variable& dest)
{
  const VariableNodePtr& dest_node = dest.node();
  VariableNode* node = new VariableNode(SH_TEMP, 1, 
                                            dest_node->valueType(), 
                                            dest_node->specialType());
  VariableNodePtr node_ptr = Pointer<VariableNode>(node);
  return node_ptr;
}

struct ATan2ExpanderBase: public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_ATAN2 == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp1(allocate_temp(I->dest));
      Variable tmp2(allocate_temp(I->dest));
      Variable tmp3(allocate_temp(I->dest));

      Variable zero(allocate_constant(I->dest, 0.0));
      Variable pi_over_two(allocate_constant(I->dest, Pi/2.0));
      Variable pi(allocate_constant(I->dest, Pi));

      // tmp1 = (b != 0) ? atan(a/b) : sign(a) * Pi/2
      new_stmts.push_back(Statement(tmp1, I->src[0], OP_DIV, I->src[1]));
      new_stmts.push_back(Statement(tmp1, OP_ATAN, tmp1));
      new_stmts.push_back(Statement(tmp2, OP_SGN, I->src[0]));
      new_stmts.push_back(Statement(tmp2, tmp2, OP_MUL, pi_over_two));
      new_stmts.push_back(Statement(tmp3, I->src[1], OP_SNE, zero));
      new_stmts.push_back(Statement(tmp1, OP_COND, tmp3, tmp1, tmp2));

      // atan2(a, b) = (b < 0) * (a >=0 ? Pi : -Pi) + tmp1
      new_stmts.push_back(Statement(tmp3, I->src[1], OP_SLT, zero));
      new_stmts.push_back(Statement(tmp2, I->src[0], OP_SGE, zero));
      new_stmts.push_back(Statement(tmp2, OP_COND, tmp2, pi, -pi));
      new_stmts.push_back(Statement(I->dest, OP_MAD, tmp3, tmp2, tmp1));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ATan2ExpanderBase> ATan2Expander;
void Transformer::expand_atan2()
{
  ATan2Expander expander;
  m_changed |= expander.transform(m_program);
}


struct TexdToTexlodBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_TEXD == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp1(allocate_scalar_temp(I->dest));
      Variable tmp2(allocate_scalar_temp(I->dest));
      Variable tmp3(allocate_temp(I->src[2]));

      TextureNodePtr texnode = shref_dynamic_cast<TextureNode>(I->src[0].node());

      int size_indices[6];
      for (int i = 0; i < texnode->texSizeVar().size(); i++) {
        size_indices[i] = i;
        size_indices[texnode->texSizeVar().size() + i] = i;
      }
      Variable size_var(texnode->texSizeVar().node(), Swizzle(texnode->texSizeVar().size(), texnode->texSizeVar().size() * 2, size_indices), texnode->texSizeVar().neg());

      new_stmts.push_back(Statement(tmp3, I->src[2], OP_MUL, size_var));

      int indices[] = {0, 1, 2, 3, 4, 5};
      Variable dx(tmp3.node(), Swizzle(tmp3.size(), tmp3.size()/2, indices), tmp3.neg());
      Variable dy(tmp3.node(), Swizzle(tmp3.size(), tmp3.size()/2, indices + tmp3.size()/2), tmp3.neg());


      new_stmts.push_back(Statement(tmp1, dx, OP_DOT, dx));
      new_stmts.push_back(Statement(tmp2, dy, OP_DOT, dy));
      new_stmts.push_back(Statement(tmp1, tmp1, OP_MAX, tmp2));
      new_stmts.push_back(Statement(tmp1, OP_SQRT, tmp1));
      new_stmts.push_back(Statement(tmp1, OP_LOG2, tmp1));

      new_stmts.push_back(Statement(I->dest, OP_TEXLOD, I->src[0], I->src[1], tmp1));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<TexdToTexlodBase> TexdToTexlod;
void Transformer::texd_to_texlod()
{
  TexdToTexlod t2t;
  m_changed |= t2t.transform(m_program);
}

struct ExpandNormalizeBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_NORM == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp1(allocate_scalar_temp(I->dest));

      new_stmts.push_back(Statement(tmp1, I->src[0], OP_DOT, I->src[0]));
      new_stmts.push_back(Statement(tmp1, OP_RSQ, tmp1));
      new_stmts.push_back(Statement(I->dest, tmp1, OP_MUL, I->src[0]));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandNormalizeBase> ExpandNormalize;
void Transformer::expand_normalize()
{
  ExpandNormalize en;
  m_changed |= en.transform(m_program);
}


struct ReciprocateSqrtBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_SQRT == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp1(allocate_temp(I->dest));

      new_stmts.push_back(Statement(tmp1, OP_RSQ, I->src[0]));
      new_stmts.push_back(Statement(I->dest, OP_RCP, tmp1));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ReciprocateSqrtBase> ReciprocateSqrt;
void Transformer::reciprocate_sqrt()
{
  ReciprocateSqrt rs;
  m_changed |= rs.transform(m_program);
}

struct ExpandDivBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_DIV == I->op && 
        !(I->src[0].valueType() & VALUETYPE_TYPE_INT) &&
        !(I->src[1].valueType() & VALUETYPE_TYPE_INT)) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp1(allocate_temp(I->src[1]));

      new_stmts.push_back(Statement(tmp1, OP_RCP, I->src[1]));
      new_stmts.push_back(Statement(I->dest, I->src[0], OP_MUL, tmp1));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandDivBase> ExpandDiv;
void Transformer::expand_div()
{
  ExpandDiv ed;
  m_changed |= ed.transform(m_program);
}

struct ExpandXpdBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_XPD == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable tmp(allocate_temp(I->dest));

      new_stmts.push_back(Statement(tmp, I->src[0](1,2,0), OP_MUL, I->src[1](2,0,1)));
      new_stmts.push_back(Statement(I->dest, OP_MAD, - I->src[1](1,2,0), I->src[0](2,0,1), tmp));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandXpdBase> ExpandXpd;
void Transformer::expand_xpd()
{
  ExpandXpd ex;
  m_changed |= ex.transform(m_program);
}

struct ExpandRndBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_RND == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable half(allocate_constant(I->src[0], 0.5));
      Variable temp(allocate_temp(I->dest));

      new_stmts.push_back(Statement(temp, I->src[0], OP_ADD, half));
      new_stmts.push_back(Statement(I->dest, OP_FLR, temp));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandRndBase> ExpandRnd;
void Transformer::expand_rnd()
{
  ExpandRnd ex;
  m_changed |= ex.transform(m_program);
}

struct ExpandLrpBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_LRP == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable one(allocate_constant(I->src[0], 1));
      Variable temp(allocate_temp(I->src[0]));

      new_stmts.push_back(Statement(temp, OP_ADD, one, -I->src[0]));
      new_stmts.push_back(Statement(temp, OP_MUL, temp, I->src[2]));
      new_stmts.push_back(Statement(I->dest, OP_MAD, I->src[0], I->src[1], temp));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandLrpBase> ExpandLrp;
void Transformer::expand_lrp()
{
  ExpandLrp ex;
  m_changed |= ex.transform(m_program);
}

struct ExpandModBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_MOD == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable temp(allocate_temp(I->src[0]));

      new_stmts.push_back(Statement(temp, OP_DIV, I->src[0], I->src[1]));
      new_stmts.push_back(Statement(temp, OP_FLR, temp));
      new_stmts.push_back(Statement(I->dest, OP_MAD, temp, -I->src[1], I->src[0]));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandModBase> ExpandMod;
void Transformer::expand_mod()
{
  ExpandMod ex;
  m_changed |= ex.transform(m_program);
}

struct ExpandSgnBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (OP_SGN == I->op) {
      BasicBlock::StmtList new_stmts;
      
      Variable zero(allocate_constant(I->src[0], 0));
      Variable temp(allocate_temp(I->src[0]));

      new_stmts.push_back(Statement(I->dest, OP_SGT, I->src[0], zero));
      new_stmts.push_back(Statement(temp, OP_SLT, I->src[0], zero));
      new_stmts.push_back(Statement(I->dest, OP_ADD, I->dest, -temp));

      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<ExpandSgnBase> ExpandSgn;
void Transformer::expand_sgn()
{
  ExpandSgn ex;
  m_changed |= ex.transform(m_program);
}

struct InverseHyperbolicExpanderBase: public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    BasicBlock::StmtList new_stmts;
    switch (I->op) {
    case OP_ACOSH:
      {
        // acosh(x) = ln(x + sqrt(x^2 - 1))
        Variable minus_one(allocate_constant(I->src[0], -1));
        Variable tmp(allocate_temp(I->src[0]));
                
        new_stmts.push_back(Statement(tmp, I->src[0], OP_MUL, I->src[0]));
        new_stmts.push_back(Statement(tmp, tmp, OP_ADD, minus_one));
        new_stmts.push_back(Statement(tmp, OP_SQRT, tmp));
        new_stmts.push_back(Statement(tmp, I->src[0], OP_ADD, tmp));
        new_stmts.push_back(Statement(I->dest, OP_LOG, tmp));
        break;
      }
    case OP_ASINH:
      {
        // asinh(x) = ln(x + sqrt(x^2 + 1))
        Variable one(allocate_constant(I->src[0], 1));
        Variable tmp(allocate_temp(I->src[0]));
                
        new_stmts.push_back(Statement(tmp, I->src[0], OP_MUL, I->src[0]));
        new_stmts.push_back(Statement(tmp, tmp, OP_ADD, one));
        new_stmts.push_back(Statement(tmp, OP_SQRT, tmp));
        new_stmts.push_back(Statement(tmp, I->src[0], OP_ADD, tmp));
        new_stmts.push_back(Statement(I->dest, OP_LOG, tmp));
        break;
      }
    case OP_ATANH:
      {
        // atanh(x) = 1/2 * ln((1+x) / (1-x))
        Variable one(allocate_constant(I->src[0], 1));
        Variable tmp1(allocate_temp(I->src[0]));
        Variable tmp2(allocate_temp(I->src[0]));
        Variable half(allocate_constant(I->src[0], 0.5));

        new_stmts.push_back(Statement(tmp1, one, OP_ADD, I->src[0]));
        new_stmts.push_back(Statement(tmp2, OP_NEG, I->src[0]));
        new_stmts.push_back(Statement(tmp2, one, OP_ADD, tmp2));
        new_stmts.push_back(Statement(tmp1, tmp1, OP_DIV, tmp2));
        new_stmts.push_back(Statement(tmp1, OP_LOG, tmp1));
        new_stmts.push_back(Statement(I->dest, half, OP_MUL, tmp1));
        break;
      }
    default:
      break;
    }

    if (!new_stmts.empty()) {
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<InverseHyperbolicExpanderBase> InverseHyperbolicExpander;
void Transformer::expand_inverse_hyperbolic()
{
  InverseHyperbolicExpander expander;
  m_changed |= expander.transform(m_program);
}


bool ordered(const Swizzle& s)
{
  int m = -1;

  for (int i = 0; i < s.size(); i++) {
    if (s[i] <= m) return false;
    m = s[i];
  }

  return true;
}

struct DestSwizzleOrdererBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (I->dest.node() && !I->dest.swizzle().identity() && !ordered(I->dest.swizzle())) {
      BasicBlock::StmtList new_stmts;

      Variable tmp1(new VariableNode(SH_TEMP, I->dest.size(), I->dest.node()->valueType(), I->dest.node()->specialType()));

      // First, run the original statement, but into an
      // unmasked/swizzled temp.
      Statement orig = *I;
      orig.dest = tmp1;
      new_stmts.push_back(orig);

      // Now, compute the indices of how we need to swizzle out of tmp1
      int* indices = new int[I->dest.size()];
      int* new_mask = new int[I->dest.size()];

      unsigned int count = 0;

      // Assumes no duplicates in dest.swizzle
      for (int i = 0; i < I->dest.node()->size(); i++) {
        for (int j = 0; j < I->dest.size(); j++) {
          if (I->dest.swizzle()[j] == i) {
            new_mask[count] = i;
            indices[j] = count;
            count++;
            break;
          }
        }
      }
      
      Variable swizzled_temp(tmp1.node(), Swizzle(tmp1.node()->size(), I->dest.size(), indices), false);
      Variable ordered_dest(I->dest.node(), Swizzle(I->dest.node()->size(), I->dest.size(), new_mask), false);

      delete [] indices;
      delete [] new_mask;

      new_stmts.push_back(Statement(ordered_dest, OP_ASN, swizzled_temp));
      
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<DestSwizzleOrdererBase> DestSwizzleOrderer;
void Transformer::order_dest_swizzles()
{
  DestSwizzleOrderer orderer;
  m_changed |= orderer.transform(m_program);
}

struct RemoveWritemasksBase : public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node)
  { 
    if (I->dest.node() && I->dest.swizzle().size() < I->dest.node()->size()) {
      BasicBlock::StmtList new_stmts;

      Variable tmp1(new VariableNode(SH_TEMP, I->dest.size(), I->dest.node()->valueType(), I->dest.node()->specialType()));

      // First, run the original statement, but into an
      // unmasked temp.
      Statement orig = *I;
      orig.dest = tmp1;
      new_stmts.push_back(orig);

      float* mask_values = new float[I->dest.node()->size()];
      int* tmp1_expand = new int[I->dest.node()->size()];
      for (int i = 0; i < I->dest.node()->size(); i++) mask_values[i] = 0.0f;
      for (int i = 0; i < I->dest.node()->size(); i++) tmp1_expand[i] = 0;
      

      for (int i = 0; i < I->dest.size(); i++) {
        mask_values[I->dest.swizzle()[i]] = 1.0f;
      }

      int count = 0;
      for (int i = 0; i < I->dest.node()->size(); i++) {
        if (mask_values[i]) {
          tmp1_expand[i] = count;
          count++;
        }
      }
      
      Variable mask(allocate_constant(I->dest.node()->size(), mask_values));
      Variable dest_nomask(I->dest.node());
      Variable tmp1_rightsize(tmp1.node(), Swizzle(tmp1.size(), I->dest.node()->size(), tmp1_expand), false);

      new_stmts.push_back(Statement(dest_nomask, OP_COND, mask, tmp1_rightsize, dest_nomask));

      delete [] mask_values;
      delete [] tmp1_expand;
      
      I = node->block->erase(I);
      node->block->splice(I, new_stmts);
      m_changed = true;
      return true;
    } else {
      return false;
    }
  }
};

typedef DefaultTransformer<RemoveWritemasksBase> RemoveWritemasks;
void Transformer::remove_writemasks()
{
  RemoveWritemasks r;
  m_changed |= r.transform(m_program);
}

}
