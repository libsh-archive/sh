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
#include "ShContext.hpp"
#include "ShDebug.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariant.hpp"
#include "ShVariantCast.hpp"
#include "ShCastManager.hpp"

namespace {
struct ShCastMgrEdgeWeigher
{
  typedef int WeightType;
  // larger than any useable cast sequence length
  // short enough that adding a few of these together won't overflow
  static const int LARGE = 10000000; 
  static const int ZERO = 0;

  ShCastMgrEdgeWeigher(bool needAuto = false)
    : m_needAuto(needAuto) {}

  int operator()(const SH::ShCastMgrEdge &e) const
  {
    if(m_needAuto && !e.m_auto) return LARGE;
    return 1;
  }

  bool m_needAuto;
};
}

namespace SH {

ShCastManager* ShCastManager::m_instance = 0;

ShCastMgrEdge::ShCastMgrEdge(const ShVariantCast* caster, bool automatic)
  : m_caster(caster), m_auto(automatic)
{
}

ShCastMgrEdge::ShCastMgrEdge(const ShCastMgrEdge &other)
  : m_caster(other.m_caster), m_auto(other.m_auto)
{}

std::ostream& ShCastMgrEdge::graphvizDump(std::ostream& out) const
{
  if(m_auto) out << "[style=\"bold\" color=\"red\"";
  else out << "[style=\"dashed\"";
  out << ", arrow=\"rvee\"]";

  return out;
}

ShCastMgrVertex::ShCastMgrVertex(ShValueType valueType, ShDataType dataType)
  : m_valueType(valueType), m_dataType(dataType)
{}

ShCastMgrGraph::ShCastMgrGraph() 
{
  for(int i = 0; i < (int)SH_VALUETYPE_END; ++i) {
    for(int j = 0; j < (int)SH_DATATYPE_END; ++j) {
      m_vert[i][j] = 0;
    }
  }
}

ShCastMgrVertex* ShCastMgrGraph::addVertex(ShValueType valueType, ShDataType dataType)
{
  if(!m_vert[valueType][dataType]) {
    m_vert[valueType][dataType] = new ShCastMgrVertex(valueType, dataType);
    ShGraph<ShCastMgrGraphType>::addVertex(m_vert[valueType][dataType]);
  }
  return m_vert[valueType][dataType];
}

void ShCastMgrGraph::addEdge( ShCastMgrEdge *edge)
{
  ShValueType src, dest;
  ShDataType srcdt, destdt;
  edge->m_caster->getCastTypes(dest, destdt, src, srcdt);

  edge->start = addVertex(src, srcdt); 
  edge->end = addVertex(dest, destdt); 
  ShGraph<ShCastMgrGraphType>::addEdge(edge);
}

std::ostream& ShCastMgrVertex::graphvizDump(std::ostream& out) const
{
  out << "[label=\"" << valueTypeName[m_valueType] << ", " 
      << dataTypeName[m_dataType] << "\"]";
  return out;
}

void ShCastManager::addCast(const ShVariantCast* caster, bool automatic) {
  m_casts.addEdge(new ShCastMgrEdge(caster, automatic));
}

void ShCastManager::init() 
{
  // for m_castStep, m_castDist
  ShCastMgrEdgeWeigher cast_weigher(false);
  ShCastMgrEdgeWeigher auto_weigher(true);

  ShCastMgrGraph::FirstStepMap step;
  ShCastMgrGraph::FirstStepMap::iterator S;

  ShCastMgrGraph::VertexPairMap<int> temp;
  ShCastMgrGraph::VertexPairMap<int>::iterator T;

  // @todo type check graphs for errors

  // initializes m_castStep to zeros

  for(int i = 0; i < (int)SH_VALUETYPE_END; ++i) 
  for(int j = 0; j < (int)SH_DATATYPE_END; ++j) 
  for(int k = 0; k < (int)SH_VALUETYPE_END; ++k) 
  for(int l = 0; l < (int)SH_DATATYPE_END; ++l) {
    m_castStep[i][j][k][l] = 0; 
  }

  m_casts.floydWarshall(cast_weigher, temp, &step); 
  for(S = step.begin(); S != step.end(); ++S) {
    ShCastMgrVertex &src = *(S->first.first);
    ShCastMgrVertex &dest = *(S->first.second);
    m_castStep[dest.m_valueType][dest.m_dataType]
              [src.m_valueType][src.m_dataType] = S->second->m_caster;
  }

  m_casts.floydWarshall(auto_weigher, temp, &step); 
  for(T = temp.begin(); T != temp.end(); ++T) {
    ShCastMgrVertex &src = *(T->first.first);
    ShCastMgrVertex &dest = *(T->first.second);
    if((src.m_dataType != SH_HOST) || (dest.m_dataType != SH_HOST)) continue;
    m_autoDist[dest.m_valueType][src.m_valueType] = T->second;
  }
}

void ShCastManager::doCast(ShVariant* dest, const ShVariant* src)
{
  SH_DEBUG_ASSERT(dest != src);

  // should be the same size
  int size = dest->size();
  ShValueType destVt = dest->valueType();
  ShDataType destDt = dest->dataType();

  // start as srcValue and cast step by step to destValueType type 
  ShValueType srcVt = src->valueType();
  ShDataType srcDt = src->dataType();

  if((srcVt == destVt) && (srcDt == destDt)) {
    memcpy(dest->array(), src->array(), dest->datasize() * dest->size());
    return;
  }

  for(bool first = true;;first = false) {
    const ShVariantCast* caster = m_castStep[destVt][destDt][srcVt][srcDt];
    //SH_DEBUG_ASSERT(caster);

    caster->getDestTypes(srcVt, srcDt); // get results of next step in cast
    if((srcVt == destVt) && (srcDt == destDt)) {
      caster->doCast(dest, src);
      if(!first) delete src;
      break;
    } else {
      // make a temporary
      ShVariant *temp = shVariantFactory(srcVt, srcDt)->generate(size);
      caster->doCast(temp, src);
      if(!first) delete src;
      src = temp;
    }
  }
}

bool ShCastManager::doAllocCast(ShVariant*& dest, ShVariant *src, 
    ShValueType valueType, ShDataType dataType)
{
  if(!src || src->typeMatches(valueType, dataType)) {
    dest = src;
    return false;
  }
  dest = shVariantFactory(valueType, dataType)->generate(src->size());
  doCast(dest, src);
  return true;
}

bool ShCastManager::doAllocCast(const ShVariant*& dest, const ShVariant* src, 
    ShValueType valueType, ShDataType dataType)
{
  // do something devious, but it works as intended since src will not be
  // changed by doCast.
  return doAllocCast(const_cast<ShVariant *&>(dest), const_cast<ShVariant *>(src),
      valueType, dataType); 
}

int ShCastManager::castDist(ShValueType destValueType, ShValueType srcValueType)
{
  return m_autoDist[destValueType][srcValueType];
}

ShCastManager* ShCastManager::instance() 
{
  if(!m_instance) m_instance = new ShCastManager();
  return m_instance;
}

std::ostream& ShCastManager::graphvizDump(std::ostream& out) const
{
  SH::graphvizDump(out, m_casts);
  return out;
}

}
