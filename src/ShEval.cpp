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
#include <sstream>
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShCastManager.hpp"

namespace SH {

ShEval* ShEval::m_instance = 0;

// #define SH_DEBUG_SHEVAL 
#define SH_EVALOP_CACHE

void ShEval::operator()(ShOperation op, ShVariant* dest, 
    const ShVariant* a, const ShVariant* b, const ShVariant* c) const
{
#ifdef SH_DEBUG_SHEVAL
  SH_DEBUG_PRINT("PRE OP=" << opInfo[op].name << " "
      << (dest ? dest->encodeArray() : "NULL") << " = "
      << (a ? a->encodeArray() : "NULL") << ", "
      << (b ? b->encodeArray() : "NULL") << ", "
      << (c ? c->encodeArray() : "NULL")); 
#endif

  const ShEvalOpInfo *evalOpInfo = 
    getEvalOpInfo(op, dest ? dest->valueType() : SH_VALUETYPE_END, 
                      a ? a->valueType() : SH_VALUETYPE_END, 
                      b ? b->valueType() : SH_VALUETYPE_END, 
                      c ? c->valueType() : SH_VALUETYPE_END);
  if(!evalOpInfo) {
#ifdef SH_DEBUG_SHEVAL
    SH_DEBUG_PRINT("Unable to find eval op!");
    SH_DEBUG_ASSERT(0);
    return;
#endif
  }

  const ShEvalOp* evalOp = evalOpInfo->m_evalOp; 
 
  // cast arguments to required types, and set up a destination
  // of the required type
  ShCastManager *castmgr = ShCastManager::instance();

  // cast versions of variables
  ShVariant *cdest; 
  const ShVariant *ca, *cb, *cc;
  bool newd, newa, newb, newc; //< indicate whether castmgr allocated new ShVariants
  newd = castmgr->doAllocCast(cdest, dest, evalOpInfo->m_dest, SH_HOST);
  newa = castmgr->doAllocCast(ca, a, evalOpInfo->m_src[0], SH_HOST);
  newb = castmgr->doAllocCast(cb, b, evalOpInfo->m_src[1], SH_HOST);
  newc = castmgr->doAllocCast(cc, c, evalOpInfo->m_src[2], SH_HOST);

  (*evalOp)(cdest, ca, cb, cc);

  // cast destination back and assign 
  // @todo remove the casting cruft if type indicies are the same
  if(newd) {
    castmgr->doCast(dest, cdest);
    delete cdest; 
  }
  if(newa) delete const_cast<ShVariant*>(ca);
  if(newb) delete const_cast<ShVariant*>(cb);
  if(newc) delete const_cast<ShVariant*>(cc);

#ifdef SH_DEBUG_SHEVAL
  SH_DEBUG_PRINT("   RES=" << opInfo[op].name << " "
      << (dest ? dest->encodeArray() : "NULL") << " = "
      << (a ? a->encodeArray() : "NULL") << ", "
      << (b ? b->encodeArray() : "NULL") << ", "
      << (c ? c->encodeArray() : "NULL")); 
#endif
}

void ShEval::addOp(ShOperation op, const ShEvalOp* evalOp, ShValueType dest, 
    ShValueType src0, ShValueType src1, ShValueType src2)
{
  m_evalOpMap[op].push_back(ShEvalOpInfo(op, evalOp, dest, src0, src1, src2));
  m_evalOpCache(op, src0, src1, src2) = &(m_evalOpMap[op].back()); // @todo type no refcount inc is okay since above inc'ed 
}

const ShEvalOpInfo* ShEval::getEvalOpInfo(ShOperation op, ShValueType dest,
    ShValueType src0, ShValueType src1, ShValueType src2) const
{
#ifdef SH_DEBUG_SHEVAL
  SH_DEBUG_PRINT("ShEval mapping op=" << opInfo[op].name << " dest,src[0-2]= " 
      << shValueTypeName(dest)
      << ", " << shValueTypeName(src0)
      << ", " << shValueTypeName(src1) 
      << ", " << shValueTypeName(src2)); 
#endif

#ifdef SH_EVALOP_CACHE
  const ShEvalOpInfo*& result = m_evalOpCache(op, src0, src1, src2);
  if(result) {
#ifdef SH_DEBUG_SHEVAL
      SH_DEBUG_PRINT("    cached result=" << result->encode()); 
#endif
    return result;
  }
#else
  const ShEvalOpinfo* result = 0;
#endif

  // @todo this really needs to be improved...
  // linear search through a table with hundreds of entries is stupid
  // (for *each* operation ever performed)
  
  const OpInfoList &oiList = m_evalOpMap[op];
  OpInfoList::const_iterator I;
  int mindist = 2000000001; 

  ShCastManager* castmgr = ShCastManager::instance();
  for(I = oiList.begin(); I != oiList.end(); ++I) {
    // @todo type this is a bit messy...
    int dist = 0;
    int arity = opInfo[op].arity;
    if(arity > 0) dist += castmgr->castDist(I->m_src[0], src0); 
    if(arity > 1) dist += castmgr->castDist(I->m_src[1], src1); 
    if(arity > 2) dist += castmgr->castDist(I->m_src[2], src2); 

#ifdef SH_DEBUG_SHEVAL
      SH_DEBUG_PRINT("    dist=" << mindist << " to " << I->encode()); 
#endif

    if(dist < mindist) {
      mindist = dist;
      result = &*I;
    }
  }

#ifdef SH_DEBUG_SHEVAL
  if(result) {
      SH_DEBUG_PRINT("    result=" << result->encode()); 
  }
#endif

  return result;
}

ShStatementInfo* ShEvalOpInfo::clone() const 
{
  return new ShEvalOpInfo(m_op, m_evalOp, m_dest, m_src[0], m_src[1], m_src[2]); 
}

std::string ShEvalOpInfo::encode() const
{
  std::ostringstream out;
  out << opInfo[m_op].name << ",";
  out << (m_dest != SH_VALUETYPE_END ? shTypeInfo(m_dest)->name() : "NULL");
  for(int i = 0; i < 3; ++i) out << ", " << (m_src[i] != SH_VALUETYPE_END ? shTypeInfo(m_src[i])->name() : "NULL");
  return out.str();
}

std::string ShEval::availableOps() const
{
  std::ostringstream out;
  for(int i = 0; i < (int)SH_OPERATION_END; ++i) {
    for(OpInfoList::const_iterator I = m_evalOpMap[i].begin();
      I != m_evalOpMap[i].end(); ++I) {
      out << I->encode() << std::endl;
    }
  }
  return out.str();
}

ShEval* ShEval::instance() 
{
  if(!m_instance) m_instance = new ShEval();
  return m_instance;
  
}

ShEval::ShEval()
{
}

ShEvalOpInfo::ShEvalOpInfo(ShOperation op, const ShEvalOp* evalOp, 
    ShValueType dest, ShValueType src0, ShValueType src1, ShValueType src2) 
  : m_op(op), m_evalOp(evalOp), m_dest(dest)
{
  m_src[0] = src0;
  m_src[1] = src1;
  m_src[2] = src2;
}

ShEvalOp::~ShEvalOp() 
{}


}
