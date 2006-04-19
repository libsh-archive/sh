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
#include <sstream>
#include "Eval.hpp"
#include "Variant.hpp"
#include "CastManager.hpp"

namespace SH {

Eval* Eval::m_instance = 0;

//#define DEBUG_EVAL 
#define EVALOP_CACHE

void Eval::operator()(Operation op, Variant* dest, 
    const Variant* a, const Variant* b, const Variant* c) const
{
#ifdef DEBUG_EVAL
  SH_DEBUG_PRINT("PRE OP=" << opInfo[op].name << " "
      << (dest ? dest->encodeArray() : "NULL") << " = "
      << (a ? a->encodeArray() : "NULL") << ", "
      << (b ? b->encodeArray() : "NULL") << ", "
      << (c ? c->encodeArray() : "NULL")); 
#endif

  const EvalOpInfo *evalOpInfo = 
    getEvalOpInfo(op, dest ? dest->valueType() : VALUETYPE_END, 
                      a ? a->valueType() : VALUETYPE_END, 
                      b ? b->valueType() : VALUETYPE_END, 
                      c ? c->valueType() : VALUETYPE_END);
  if(!evalOpInfo) {
    // @todo range proper error message
    SH_DEBUG_ERROR("Unable to find eval op for " << opInfo[op].name << 
        " " << (dest ? valueTypeName(dest->valueType()) : "") << " <- "
        << (a ? valueTypeName(a->valueType()) : "") << ", "
        << (b ? valueTypeName(b->valueType()) : "") << ", "
        << (c ? valueTypeName(c->valueType()) : "") << ")"); 
    SH_DEBUG_ASSERT(0); 
  }

  const EvalOp* evalOp = evalOpInfo->m_evalOp; 
 
  // cast arguments to required types, and set up a destination
  // of the required type
  CastManager *castmgr = CastManager::instance();

  // cast versions of variables
  Variant *cdest; 
  const Variant *ca, *cb, *cc;
  bool newd, newa, newb, newc; //< indicate whether castmgr allocated new Variants
  if(dest->typeMatches(evalOpInfo->m_dest, HOST)) {
    newd = false; 
    cdest = dest;
  } else {
    newd = true;
    cdest = variantFactory(evalOpInfo->m_dest)->generate(dest->size());  
  }
  newa = castmgr->doAllocCast(ca, a, evalOpInfo->m_src[0], HOST);
  newb = castmgr->doAllocCast(cb, b, evalOpInfo->m_src[1], HOST);
  newc = castmgr->doAllocCast(cc, c, evalOpInfo->m_src[2], HOST);

  (*evalOp)(cdest, ca, cb, cc);

  // cast destination back and assign 
  // @todo remove the casting cruft if type indicies are the same
  if(newd) {
    castmgr->doCast(dest, cdest);
    delete cdest; 
  }
  if(newa) delete const_cast<Variant*>(ca);
  if(newb) delete const_cast<Variant*>(cb);
  if(newc) delete const_cast<Variant*>(cc);

#ifdef DEBUG_EVAL
  SH_DEBUG_PRINT("   RES=" << opInfo[op].name << " "
      << (dest ? dest->encodeArray() : "NULL") << " = "
      << (a ? a->encodeArray() : "NULL") << ", "
      << (b ? b->encodeArray() : "NULL") << ", "
      << (c ? c->encodeArray() : "NULL")); 
#endif
}

void Eval::operator()(Operation op, const VariantPtr& dest, 
    const VariantCPtr& a, const VariantCPtr& b, const VariantCPtr& c) const
{
  operator()(op, dest.object(), a.object(), b.object(), c.object());
}

void Eval::addOp(Operation op, const EvalOp* evalOp, ValueType dest, 
    ValueType src0, ValueType src1, ValueType src2)
{
  m_evalOpMap[op].push_back(EvalOpInfo(op, evalOp, dest, src0, src1, src2));
  m_evalOpCache(op, src0, src1, src2) = &(m_evalOpMap[op].back()); // @todo type no refcount inc is okay since above inc'ed 
}

const EvalOpInfo* Eval::getEvalOpInfo(Operation op, ValueType dest,
    ValueType src0, ValueType src1, ValueType src2) const
{
#ifdef DEBUG_EVAL
  SH_DEBUG_PRINT("Eval mapping op=" << opInfo[op].name << " dest,src[0-2]= " 
      << valueTypeName(dest)
      << ", " << valueTypeName(src0)
      << ", " << valueTypeName(src1) 
      << ", " << valueTypeName(src2)); 
#endif

#ifdef EVALOP_CACHE
  const EvalOpInfo*& result = m_evalOpCache(op, src0, src1, src2);
  if(result) {
#ifdef DEBUG_EVAL
      SH_DEBUG_PRINT("    cached result=" << result->encode()); 
#endif
    return result;
  }
#else
  const EvalOpinfo* result = 0;
#endif

  // @todo this really needs to be improved...
  // linear search through a table with hundreds of entries is stupid
  // (for *each* operation ever performed)
  
  const OpInfoList &oiList = m_evalOpMap[op];
  OpInfoList::const_iterator I;
  int mindist = 2000000001; 

  CastManager* castmgr = CastManager::instance();
  for(I = oiList.begin(); I != oiList.end(); ++I) {
    // @todo type this is a bit messy...
    int dist = 0;
    int arity = opInfo[op].arity;
    if(arity > 0) dist += castmgr->castDist(I->m_src[0], src0); 
    if(arity > 1) dist += castmgr->castDist(I->m_src[1], src1); 
    if(arity > 2) dist += castmgr->castDist(I->m_src[2], src2); 

#ifdef DEBUG_EVAL
      SH_DEBUG_PRINT("    dist=" << mindist << " to " << I->encode()); 
#endif

    if(dist < mindist) {
      mindist = dist;
      result = &*I;
    }
  }

  // @todo range identify cases when we haven't found an evaluator properly
  // instead of making up numbers that we think are big enough
  if(mindist > 1000) {
    result = 0;
  }

#ifdef DEBUG_EVAL
  if(result) {
      SH_DEBUG_PRINT("    result=" << result->encode()); 
  }
#endif

  return result;
}

const EvalOpInfo* Eval::getEvalOpInfo(const Statement &stmt) const
{
  return getEvalOpInfo(stmt.op, stmt.dest.valueType(),
      stmt.src[0].valueType(),
      stmt.src[1].valueType(),
      stmt.src[2].valueType()); 
}

Info* EvalOpInfo::clone() const 
{
  return new EvalOpInfo(m_op, m_evalOp, m_dest, m_src[0], m_src[1], m_src[2]); 
}

std::string EvalOpInfo::encode() const
{
  std::ostringstream out;
  out << opInfo[m_op].name << ",";
  out << (m_dest != VALUETYPE_END ? typeInfo(m_dest)->name() : "NULL");
  for(int i = 0; i < 3; ++i) out << ", " << (m_src[i] != VALUETYPE_END ? typeInfo(m_src[i])->name() : "NULL");
  return out.str();
}

std::string Eval::availableOps() const
{
  std::ostringstream out;
  for(int i = 0; i < (int)OPERATION_END; ++i) {
    for(OpInfoList::const_iterator I = m_evalOpMap[i].begin();
      I != m_evalOpMap[i].end(); ++I) {
      out << I->encode() << std::endl;
    }
  }
  return out.str();
}

Eval* Eval::instance() 
{
  if(!m_instance) m_instance = new Eval();
  return m_instance;
  
}

Eval::Eval()
{
}

EvalOpInfo::EvalOpInfo(Operation op, const EvalOp* evalOp, 
    ValueType dest, ValueType src0, ValueType src1, ValueType src2) 
  : m_op(op), m_evalOp(evalOp), m_dest(dest)
{
  m_src[0] = src0;
  m_src[1] = src1;
  m_src[2] = src2;
}

EvalOp::~EvalOp() 
{}


}
