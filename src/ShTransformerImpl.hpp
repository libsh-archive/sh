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
#ifndef SH_TRANSFORMERIMPL_HPP
#define SH_TRANSFORMERIMPL_HPP

#include <algorithm>
#include <sstream>
#include <map>
#include <list>
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShEval.hpp"
#include "ShTypeInfo.hpp"
#include "ShTransformer.hpp"

#define SH_DBG_TRANSFORMER

namespace SH {
#if 0
template<class CanCast>
struct CastExtractorBase: public ShTransformerParent 
{
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node)
  {
    ShStatement &stmt = *I;
    if(stmt.dest.null()) return false;

    // get eval op
    const ShEvalOpInfo* evalInfo = ShEval::instance()->getEvalOpInfo(stmt);  

    // handle sources
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
      ShValueType from = stmt.src[i].valueType();
      ShValueType to = evalInfo->m_src[i];
      if(from == to) continue;
      if(!CanCast::checkSrc(stmt.op, from, to)) {
#ifdef SH_DBG_TRANSFORMER
        SH_DEBUG_PRINT("Turning src[" << i << "] into ASN for cast " 
            << shValueTypeName(from) << "->" 
            << shValueTypeName(to) << " on stmt=" << stmt);
#endif
        ShVariable temp(stmt.src[i].node()->clone(SH_TEMP, 0, to));
        node->block->m_statements.insert(I, ShStatement(temp, SH_OP_ASN, stmt.src[i]));
        stmt.src[i] = temp;
        m_changed = true;
      }
    }

    // handle dest
    ShValueType from = stmt.dest.valueType();
    ShValueType to = evalInfo->m_dest;
    if(from != to && !CanCast::checkDest(stmt.op, from, to)) {
#ifdef SH_DBG_TRANSFORMER
      SH_DEBUG_PRINT("Turning dest into ASN for cast " 
          << shValueTypeName(from) << "->" 
          << shValueTypeName(to) << " on stmt=" << stmt);
#endif
      ShVariable temp(stmt.dest.node()->clone(SH_TEMP, 0, to));
      ShBasicBlock::ShStmtList::iterator J = I;
      ++J;
      node->block->m_statements.insert(J, ShStatement(stmt.dest, SH_OP_ASN, temp));
      stmt.dest = temp;
      m_changed = true;
    }
    return false;
  }
};


template<class CanCast>
void ShTransformer::extractCasts()
{
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("extcast_start");
#endif
  ShContext::current()->enter(m_program); // since we might be adding temps
    ShDefaultTransformer<CastExtractorBase<CanCast> > ce;
    ce.transform(m_program);
    m_changed |= ce.changed();
  ShContext::current()->exit(); // since we might be adding temps
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("extcast_done");
#endif
}

template<class DoVec>
struct ScalarVectorizerBase: public ShTransformerParent
{
  bool handleStmt(ShBasicBlock::ShStmtList::iterator& I, ShCtrlGraphNodePtr node) {
    ShStatement& stmt = *I;

    if(!DoVec::check(stmt.op)) return false;
    if(opInfo[stmt.op].arity < 2) return false;
    if(opInfo[stmt.op].result_source == ShOperationInfo::EXTERNAL || 
       opInfo[stmt.op].result_source == ShOperationInfo::IGNORE) return false;

    // for multiple arguments, upswizzle all arguments to  
    int maxSize = 0;
    for(int i = 0; i < opInfo[stmt.op].arity; ++i){
      maxSize = std::max(maxSize, stmt.src[i].size());
    }

    if(maxSize == 1) return false;

#ifdef SH_DBG_TRANSFORMER
    SH_DEBUG_PRINT("Vectorizing to size=" << maxSize << " on stmt=" << stmt);
#endif
    for(int i = 0; i < opInfo[stmt.op].arity; ++i) {
      if(stmt.src[i].size() == 1) {
        stmt.src[i] = ShVariable(stmt.src[i].node(), 
            ShSwizzle(stmt.src[i].swizzle(), maxSize), stmt.src[i].neg()); 
      }
    }
    return false;
  }
};

template<class DoVec>
void ShTransformer::vectorizeScalars()
{
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("vectscal_start");
#endif
  ShDefaultTransformer<ScalarVectorizerBase<DoVec> > sv;
  sv.transform(m_program);
  m_changed |= sv.changed();
#ifdef SH_DBG_TRANSFORMER
  m_program->dump("vectscal_done");
#endif
}
#endif

template<typename T>
void ShDefaultTransformer<T>::operator()(ShCtrlGraphNodePtr node) {
  if (!node) return;
  ShBasicBlockPtr block = node->block;
  if (!block) return;

  for (ShBasicBlock::ShStmtList::iterator I = block->begin(); I != block->end();) {
    if(!T::handleStmt(I, node)) ++I;
  }
}

template<typename T>
bool ShDefaultTransformer<T>::transform(ShProgramNodePtr p) {
  T::start(p);

  T::handleVarList(p->inputs, SH_INPUT);
  T::handleVarList(p->outputs, SH_OUTPUT);
  T::handleVarList(p->temps, SH_TEMP);
  T::handleVarList(p->constants, SH_CONST);
  T::handleVarList(p->uniforms, SH_TEMP);

  T::handleTexList(p->textures);
  T::handleChannelList(p->channels);
  T::handlePaletteList(p->palettes);

  p->ctrlGraph->dfs(*this);

  T::finish();
  return T::changed();
}

}

#undef SH_DBG_TRANSFORMER

#endif
