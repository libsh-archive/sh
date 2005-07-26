// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

ShCastMgrVertex::ShCastMgrVertex(const ShCastMgrVertex &other)
  : m_valueType(other.m_valueType), m_dataType(other.m_dataType)
{}

ShCastMgrGraph::ShCastMgrGraph() 
{
}

ShCastMgrVertex* ShCastMgrGraph::addVertex(ShValueType valueType, ShDataType dataType)
{
  if(!m_vert(valueType, dataType)) {
    m_vert(valueType, dataType) = new ShCastMgrVertex(valueType, dataType);
    ShGraph<ShCastMgrGraphType>::addVertex(m_vert(valueType, dataType));
  }
  return m_vert(valueType, dataType);
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
  out << "[label=\"" << shValueTypeName(m_valueType) << ", " 
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

  m_casts.floydWarshall(cast_weigher, temp, &step); 
  for(S = step.begin(); S != step.end(); ++S) {
    ShCastMgrVertex &src = *(S->first.first);
    ShCastMgrVertex &dest = *(S->first.second);
    m_castStep(dest.m_valueType, dest.m_dataType, 
               src.m_valueType, src.m_dataType) = S->second->m_caster;
  }

  m_casts.floydWarshall(auto_weigher, temp, &step); 
  for(T = temp.begin(); T != temp.end(); ++T) {
    ShCastMgrVertex &src = *(T->first.first);
    ShCastMgrVertex &dest = *(T->first.second);
    if((src.m_dataType != SH_HOST) || (dest.m_dataType != SH_HOST)) continue;
    m_autoDist(dest.m_valueType, src.m_valueType) = T->second;
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

  SH_DEBUG_ASSERT(!(destVt == srcVt && srcDt == destDt));

  for(bool first = true;;first = false) {
    const ShVariantCast* caster = m_castStep(destVt, destDt, srcVt, srcDt);
    if(!caster) {
      SH_DEBUG_ERROR("Unable to cast to " << shValueTypeName(destVt) << " from " << shValueTypeName(srcVt));
    }
    SH_DEBUG_ASSERT(caster);

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
  return m_autoDist(destValueType, srcValueType);
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
