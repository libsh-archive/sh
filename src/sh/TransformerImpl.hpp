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
#ifndef SHTRANSFORMERIMPL_HPP
#define SHTRANSFORMERIMPL_HPP

#include <algorithm>
#include <sstream>
#include <map>
#include <list>
#include "Context.hpp"
#include "Debug.hpp"
#include "Eval.hpp"
#include "TypeInfo.hpp"
#include "Transformer.hpp"

#define DBG_TRANSFORMER

namespace SH {
#if 0
template<class CanCast>
struct CastExtractorBase: public TransformerParent 
{
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNodePtr node)
  {
    Statement &stmt = *I;
    if(stmt.dest.null()) return false;

    // get eval op
    const EvalOpInfo* evalInfo = Eval::instance()->getEvalOpInfo(stmt);  

    // handle sources
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
      ValueType from = stmt.src[i].valueType();
      ValueType to = evalInfo->m_src[i];
      if(from == to) continue;
      if(!CanCast::checkSrc(stmt.op, from, to)) {
#ifdef DBG_TRANSFORMER
        SH_DEBUG_PRINT("Turning src[" << i << "] into ASN for cast " 
            << valueTypeName(from) << "->" 
            << valueTypeName(to) << " on stmt=" << stmt);
#endif
        Variable temp(stmt.src[i].node()->clone(SH_TEMP, 0, to));
        node->block->m_statements.insert(I, Statement(temp, OP_ASN, stmt.src[i]));
        stmt.src[i] = temp;
        m_changed = true;
      }
    }

    // handle dest
    ValueType from = stmt.dest.valueType();
    ValueType to = evalInfo->m_dest;
    if(from != to && !CanCast::checkDest(stmt.op, from, to)) {
#ifdef DBG_TRANSFORMER
      SH_DEBUG_PRINT("Turning dest into ASN for cast " 
          << valueTypeName(from) << "->" 
          << valueTypeName(to) << " on stmt=" << stmt);
#endif
      Variable temp(stmt.dest.node()->clone(SH_TEMP, 0, to));
      BasicBlock::StmtList::iterator J = I;
      ++J;
      node->block->m_statements.insert(J, Statement(stmt.dest, OP_ASN, temp));
      stmt.dest = temp;
      m_changed = true;
    }
    return false;
  }
};


template<class CanCast>
void Transformer::extractCasts()
{
#ifdef DBG_TRANSFORMER
  m_program->dump("extcast_start");
#endif
  Context::current()->enter(m_program); // since we might be adding temps
    DefaultTransformer<CastExtractorBase<CanCast> > ce;
    ce.transform(m_program);
    m_changed |= ce.changed();
  Context::current()->exit(); // since we might be adding temps
#ifdef DBG_TRANSFORMER
  m_program->dump("extcast_done");
#endif
}

template<class DoVec>
struct ScalarVectorizerBase: public TransformerParent
{
  bool handleStmt(BasicBlock::StmtList::iterator& I, CtrlGraphNodePtr node) {
    Statement& stmt = *I;

    if(!DoVec::check(stmt.op)) return false;
    if(opInfo[stmt.op].arity < 2) return false;
    if(opInfo[stmt.op].result_source == OperationInfo::EXTERNAL || 
       opInfo[stmt.op].result_source == OperationInfo::IGNORE) return false;

    // for multiple arguments, upswizzle all arguments to  
    int maxSize = 0;
    for(int i = 0; i < opInfo[stmt.op].arity; ++i){
      maxSize = std::max(maxSize, stmt.src[i].size());
    }

    if(maxSize == 1) return false;

#ifdef DBG_TRANSFORMER
    SH_DEBUG_PRINT("Vectorizing to size=" << maxSize << " on stmt=" << stmt);
#endif
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
      if(stmt.src[i].size() == 1) {
        stmt.src[i] = Variable(stmt.src[i].node(), 
            Swizzle(stmt.src[i].swizzle(), maxSize), stmt.src[i].neg()); 
      }
    }
    return false;
  }
};

template<class DoVec>
void Transformer::vectorizeScalars()
{
#ifdef DBG_TRANSFORMER
  m_program->dump("vectscal_start");
#endif
  DefaultTransformer<ScalarVectorizerBase<DoVec> > sv;
  sv.transform(m_program);
  m_changed |= sv.changed();
#ifdef DBG_TRANSFORMER
  m_program->dump("vectscal_done");
#endif
}
#endif

template<typename T>
void DefaultTransformer<T>::operator()(const CtrlGraphNodePtr& node) {
  if (!node) return;
  BasicBlockPtr block = node->block;
  if (!block) return;

  for (BasicBlock::StmtList::iterator I = block->begin(); I != block->end();) {
    if(!T::handleStmt(I, node)) ++I;
  }
}

template<typename T>
bool DefaultTransformer<T>::transform(const ProgramNodePtr& p) {
  T::start(p);

  T::handleVarList(p->inputs, SH_INPUT);
  T::handleVarList(p->outputs, SH_OUTPUT);
  T::handleVarList(p->temps, SH_TEMP);
  T::handleVarList(p->constants, SH_CONST);
  T::handleVarList(p->uniforms, SH_TEMP);

  T::handleTexList(p->textures);
  T::handlePaletteList(p->palettes);

  p->ctrlGraph->dfs(*this);

  T::finish();
  return T::changed();
}

}

#undef DBG_TRANSFORMER

#endif
