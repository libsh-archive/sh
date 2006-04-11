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
#include "Context.hpp"
#include "Debug.hpp"
#include "TypeInfo.hpp"
#include "Variant.hpp"
#include "VariantCast.hpp"
#include "CastManager.hpp"

namespace {
struct CastMgrEdgeWeigher
{
  typedef int WeightType;
  // larger than any useable cast sequence length
  // short enough that adding a few of these together won't overflow
  static const int LARGE = 10000000; 
  static const int ZERO = 0;

  CastMgrEdgeWeigher(bool needAuto = false)
    : m_needAuto(needAuto) {}

  int operator()(const SH::CastMgrEdge &e) const
  {
    if(m_needAuto && !e.m_auto) return LARGE;
    return 1;
  }

  bool m_needAuto;
};
}

namespace SH {

CastManager* CastManager::m_instance = 0;

CastMgrEdge::CastMgrEdge(const VariantCast* caster, bool automatic)
  : m_caster(caster), m_auto(automatic)
{
}

CastMgrEdge::CastMgrEdge(const CastMgrEdge &other)
  : m_caster(other.m_caster), m_auto(other.m_auto)
{}

std::ostream& CastMgrEdge::graphvizDump(std::ostream& out) const
{
  if(m_auto) out << "[style=\"bold\" color=\"red\"";
  else out << "[style=\"dashed\"";
  out << ", arrow=\"rvee\"]";

  return out;
}

CastMgrVertex::CastMgrVertex(ValueType valueType, DataType dataType)
  : m_valueType(valueType), m_dataType(dataType)
{}

CastMgrVertex::CastMgrVertex(const CastMgrVertex &other)
  : m_valueType(other.m_valueType), m_dataType(other.m_dataType)
{}

CastMgrGraph::CastMgrGraph() 
{
}

CastMgrVertex* CastMgrGraph::addVertex(ValueType valueType, DataType dataType)
{
  if(!m_vert(valueType, dataType)) {
    m_vert(valueType, dataType) = new CastMgrVertex(valueType, dataType);
    Graph<CastMgrGraphType>::addVertex(m_vert(valueType, dataType));
  }
  return m_vert(valueType, dataType);
}

void CastMgrGraph::addEdge( CastMgrEdge *edge)
{
  ValueType src, dest;
  DataType srcdt, destdt;
  edge->m_caster->getCastTypes(dest, destdt, src, srcdt);

  edge->start = addVertex(src, srcdt); 
  edge->end = addVertex(dest, destdt); 
  Graph<CastMgrGraphType>::addEdge(edge);
}

std::ostream& CastMgrVertex::graphvizDump(std::ostream& out) const
{
  out << "[label=\"" << valueTypeName(m_valueType) << ", " 
      << dataTypeName[m_dataType] << "\"]";
  return out;
}

void CastManager::addCast(const VariantCast* caster, bool automatic) {
  m_casts.addEdge(new CastMgrEdge(caster, automatic));
}

void CastManager::init() 
{
  // for m_castStep, m_castDist
  CastMgrEdgeWeigher cast_weigher(false);
  CastMgrEdgeWeigher auto_weigher(true);

  CastMgrGraph::FirstStepMap step;
  CastMgrGraph::FirstStepMap::iterator S;

  CastMgrGraph::VertexPairMap<int> temp;
  CastMgrGraph::VertexPairMap<int>::iterator T;

  // @todo type check graphs for errors

  // initializes m_castStep to zeros

  m_casts.floydWarshall(cast_weigher, temp, &step); 
  for(S = step.begin(); S != step.end(); ++S) {
    CastMgrVertex &src = *(S->first.first);
    CastMgrVertex &dest = *(S->first.second);
    m_castStep(dest.m_valueType, dest.m_dataType, 
               src.m_valueType, src.m_dataType) = S->second->m_caster;
  }

  m_casts.floydWarshall(auto_weigher, temp, &step); 
  for(T = temp.begin(); T != temp.end(); ++T) {
    CastMgrVertex &src = *(T->first.first);
    CastMgrVertex &dest = *(T->first.second);
    if((src.m_dataType != HOST) || (dest.m_dataType != HOST)) continue;
    m_autoDist(dest.m_valueType, src.m_valueType) = T->second;
  }
}

void CastManager::doCast(Variant* dest, const Variant* src)
{
  DEBUG_ASSERT(dest != src);

  // should be the same size
  int size = dest->size();
  ValueType destVt = dest->valueType();
  DataType destDt = dest->dataType();

  // start as srcValue and cast step by step to destValueType type 
  ValueType srcVt = src->valueType();
  DataType srcDt = src->dataType();

  DEBUG_ASSERT(!(destVt == srcVt && srcDt == destDt));

  for(bool first = true;;first = false) {
    const VariantCast* caster = m_castStep(destVt, destDt, srcVt, srcDt);
    if(!caster) {
      DEBUG_ERROR("Unable to cast to " << valueTypeName(destVt) << " from " << valueTypeName(srcVt));
    }
    DEBUG_ASSERT(caster);

    caster->getDestTypes(srcVt, srcDt); // get results of next step in cast
    if((srcVt == destVt) && (srcDt == destDt)) {
      caster->doCast(dest, src);
      if(!first) delete src;
      break;
    } else {
      // make a temporary
      Variant *temp = variantFactory(srcVt, srcDt)->generate(size);
      caster->doCast(temp, src);
      if(!first) delete src;
      src = temp;
    }
  }
}

bool CastManager::doAllocCast(Variant*& dest, Variant *src, 
    ValueType valueType, DataType dataType)
{
  if(!src || src->typeMatches(valueType, dataType)) {
    dest = src;
    return false;
  }
  dest = variantFactory(valueType, dataType)->generate(src->size());
  doCast(dest, src);
  return true;
}

bool CastManager::doAllocCast(const Variant*& dest, const Variant* src, 
    ValueType valueType, DataType dataType)
{
  // do something devious, but it works as intended since src will not be
  // changed by doCast.
  return doAllocCast(const_cast<Variant *&>(dest), const_cast<Variant *>(src),
      valueType, dataType); 
}

int CastManager::castDist(ValueType destValueType, ValueType srcValueType)
{
  return m_autoDist(destValueType, srcValueType);
}

CastManager* CastManager::instance() 
{
  if(!m_instance) m_instance = new CastManager();
  return m_instance;
}

std::ostream& CastManager::graphvizDump(std::ostream& out) const
{
  SH::graphvizDump(out, m_casts);
  return out;
}

}
