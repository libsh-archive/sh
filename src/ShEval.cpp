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
#include "ShCloak.hpp"
#include "ShCastManager.hpp"

namespace SH {

ShEval* ShEval::m_instance = 0;

void ShEval::operator()(ShOperation op, ShCloakPtr dest, 
    ShCloakCPtr a, ShCloakCPtr b, ShCloakCPtr c) const
{
  SH_DEBUG_PRINT("PRE OP=" << opInfo[op].name << " "
      << (dest ? dest->encode() : "NULL") << " = "
      << (a ? a->encode() : "NULL") << ", "
      << (b ? b->encode() : "NULL") << ", "
      << (c ? c->encode() : "NULL")); 

  const ShEvalOpInfo *evalOpInfo = 
    getEvalOpInfo(op, dest ? dest->typeIndex() : 0, 
                      a ? a->typeIndex() : 0, 
                      b ? b->typeIndex() : 0, 
                      c ? c->typeIndex() : 0);

  SH_DEBUG_ASSERT((evalOpInfo != 0) && (m_evalOpMap.count(*evalOpInfo) > 0));
  ShPointer<const ShEvalOp> evalOp = m_evalOpMap.find(*evalOpInfo)->second; 
 
  // cast arguments to required types, and set up a destination
  // of the required type
  ShCastManager *castmgr = ShCastManager::instance();

  // note - we do a castdest in case the op only modifies part of the value
  // in each tuple component of dest (e.g. setting lo on intervals)
  ShCloakPtr castdest = dest ? castmgr->doCast(evalOpInfo->m_dest, dest) : dest;
  a = a ? castmgr->doCast(evalOpInfo->m_src[0], a) : a;
  b = b ? castmgr->doCast(evalOpInfo->m_src[1], b) : b;
  c = c ? castmgr->doCast(evalOpInfo->m_src[2], c) : c;

  (*evalOp)(castdest, a, b, c);

  // cast destination back and assign 
  // @todo remove the casting cruft if type indicies are the same
  if(dest) {
    castdest = castmgr->doCast(dest->typeIndex(), castdest);
    dest->set(castdest);
  }
  SH_DEBUG_PRINT("   RES=" << opInfo[op].name << " "
      << (dest ? dest->encode() : "NULL") << " = "
      << (a ? a->encode() : "NULL") << ", "
      << (b ? b->encode() : "NULL") << ", "
      << (c ? c->encode() : "NULL")); 
}

void ShEval::addOp(ShOperation op, ShPointer<const ShEvalOp> evalOp, int dest, int src0Index, int src1Index, int src2Index)
{
  ShEvalOpInfo key(op, dest, src0Index, src1Index, src2Index);

  SH_DEBUG_ASSERT(m_evalOpMap.count(key) == 0);
  m_evalOpMap[key] = evalOp;
}

const ShEvalOpInfo* ShEval::getEvalOpInfo(ShOperation op, int dest,
    int src0, int src1, int src2) const
{
  // do the naive thing now.  Can cache results later
  ShEvalOpInfo key = ShEvalOpInfo(op, dest, src0, src1, src2);
  if(m_evalOpMap.count(key) > 0) return &m_evalOpMap.find(key)->first;

  InfoOpMap::const_iterator I;
  const ShEvalOpInfo* result = 0;
  int min = 1234567890; // TODO pick something that you didn't pick out 
                        // of the sky after seeing a UFO.
  ShCastManager* castmgr = ShCastManager::instance();
  for(I = m_evalOpMap.begin(); I != m_evalOpMap.end(); ++I) {
    // @todo make this faster than linear search...
    if(op != I->first.m_op) continue;
    int dist = castmgr->castDist(I->first.m_src[0], src0) +
               castmgr->castDist(I->first.m_src[1], src1) +
               castmgr->castDist(I->first.m_src[2], src2); 
    if(dist < min) {
      min = dist;
      result = &(I->first); 
    }
  }
  SH_DEBUG_PRINT("Mapping op=" << opInfo[op].name << " dest,src[0-2]= " 
      << (dest ? shTypeInfo(dest)->name() : "NULL")
      << ", " << (src0 ? shTypeInfo(src0)->name() : "NULL")
      << ", " << (src1 ? shTypeInfo(src1)->name() : "NULL")
      << ", " << (src2 ? shTypeInfo(src2)->name() : "NULL"));

  SH_DEBUG_PRINT("    to " << result->encode()); 

  return result;
}

bool ShEvalOpInfo::operator<(const ShEvalOpInfo &other) const 
{
  if(m_op < other.m_op) return true;
  else if(m_op > other.m_op) return false;
  for(int i = 0; i < 3; ++i) {
    if(m_src[i] < other.m_src[i]) return true;
    if(m_src[i] > other.m_src[i]) return false;
  }
  return false;
}

ShStatementInfo* ShEvalOpInfo::clone() const 
{
  return new ShEvalOpInfo(m_op, m_dest, m_src[0], m_src[1], m_src[2]); 
}

std::string ShEvalOpInfo::encode() const
{
  std::ostringstream out;
  out << opInfo[m_op].name << ",";
  out << (m_dest ? shTypeInfo(m_dest)->name() : "NULL");
  for(int i = 0; i < 3; ++i) out << ", " << (m_src[i] ? shTypeInfo(m_src[i])->name() : "NULL");
  return out.str();
}

std::string ShEval::availableOps() const
{
  std::ostringstream out;
  for(InfoOpMap::const_iterator I = m_evalOpMap.begin();
      I != m_evalOpMap.end(); ++I) {
    out << I->first.encode() << std::endl;
  }
  return out.str();
}

ShEval* ShEval::instance() 
{
  if(!m_instance) m_instance = new ShEval();
  return m_instance;
  
}

ShEval::ShEval()
{}

ShEvalOpInfo::ShEvalOpInfo(ShOperation op, int dest, int src0, int src1, int src2) 
  : m_op(op), m_dest(dest)
{
  m_src[0] = src0;
  m_src[1] = src1;
  m_src[2] = src2;
}

ShEvalOp::~ShEvalOp() 
{}


}
