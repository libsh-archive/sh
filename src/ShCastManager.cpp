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

  ShCastMgrEdgeWeigher(bool needAuto = false, bool needPrec = false)
    : m_needAuto(needAuto), m_needPrec(needPrec) {}

  int operator()(const SH::ShCastMgrEdge &e) const
  {
    if(m_needAuto && !e.m_automatic) return LARGE;
    if(m_needPrec && !e.m_precedence) return LARGE;
    return 1;
  }

  bool m_needAuto;
  bool m_needPrec;
};
}

namespace SH {

ShCastManager* ShCastManager::m_instance = 0;

ShCastMgrEdge::ShCastMgrEdge(ShVariantCastPtr caster, bool automatic, bool precedence)
  : m_caster(caster), m_automatic(automatic), m_precedence(precedence)
{
}

ShCastMgrEdge::ShCastMgrEdge(const ShCastMgrEdge &other)
  : m_caster(other.m_caster), m_automatic(other.m_automatic), m_precedence(other.m_precedence)
{}

std::ostream& ShCastMgrEdge::graphvizDump(std::ostream& out) const
{
  if(m_automatic) out << "[style=\"bold\"";
  else out << "[style=\"dashed\"";

  out << ", arrow=\"rvee\"";

  if(m_precedence) out << "color=\"red\"]";
  else out << "]";
  return out;
}

ShCastMgrVertex::ShCastMgrVertex(int typeIndex)
  : m_typeIndex(typeIndex)
{}

void ShCastMgrGraph::addVertex(int typeIndex)
{
  if((unsigned int) typeIndex >= m_vert.size()) m_vert.resize(typeIndex + 1, 0);
  if(!m_vert[typeIndex]) {
    m_vert[typeIndex] = new ShCastMgrVertex(typeIndex);
    ShGraph<ShCastMgrGraphType>::addVertex(m_vert[typeIndex]);
  }
}

void ShCastMgrGraph::addEdge(int srcIndex, int destIndex, 
    ShCastMgrEdge *edge)
{
  addVertex(srcIndex);
  addVertex(destIndex);

  edge->start = m_vert[srcIndex];
  edge->end = m_vert[destIndex];
  ShGraph<ShCastMgrGraphType>::addEdge(edge);
}

std::ostream& ShCastMgrVertex::graphvizDump(std::ostream& out) const
{
  const ShTypeInfo* typeInfo = shTypeInfo(m_typeIndex); 

  out << "[label=\"" << typeInfo->name() << "\"]";
  return out;
}

void ShCastManager::addCast(int destIndex, 
    int srcIndex, ShVariantCastPtr caster, bool automatic, bool precedence) {
  m_casts.addEdge(srcIndex, destIndex, new ShCastMgrEdge(caster, automatic, precedence));
}

void ShCastManager::init() 
{
  // for m_castStep, m_castDist
  ShCastMgrEdgeWeigher cast_weigher(false, false);
  ShCastMgrEdgeWeigher auto_weigher(true, false);
  ShCastMgrEdgeWeigher prec_weigher(true, true);

  ShCastMgrGraph::FirstStepMap step;
  ShCastMgrGraph::FirstStepMap::iterator S;

  ShCastMgrGraph::VertexPairMap<int> temp;
  ShCastMgrGraph::VertexPairMap<int>::iterator T;

  // TODO check graphs for errors


  // TODO current code is ridiculous...just look at this  
  // ugly stuff below

  int numTypes = shNumTypes(); 
  m_castStep.clear();
  m_autoStep.clear();
  m_precDist.clear();
  m_castStep.resize(numTypes + 1);
  m_autoStep.resize(numTypes + 1);
  m_precDist.resize(numTypes + 1);
  for(int i = 0; i <= numTypes; ++i){
    m_castStep[i].resize(numTypes + 1, 0);
    m_autoStep[i].resize(numTypes + 1, 0);
    m_precDist[i].resize(numTypes + 1, 0);
  }

  m_casts.floydWarshall(cast_weigher, temp, &step); 
  for(S = step.begin(); S != step.end(); ++S) {
    int src = S->first.first->m_typeIndex;
    int dest = S->first.second->m_typeIndex;
    m_castStep[dest][src] = S->second->m_caster;
  }

  m_casts.floydWarshall(auto_weigher, temp, &step); 
  for(S = step.begin(); S != step.end(); ++S) {
    int src = S->first.first->m_typeIndex;
    int dest = S->first.second->m_typeIndex;
    m_autoStep[dest][src] = S->second->m_caster;
  }

  m_casts.floydWarshall(prec_weigher, temp, &step); 
  for(T = temp.begin(); T != temp.end(); ++T) {
    int src = T->first.first->m_typeIndex;
    int dest = T->first.second->m_typeIndex;
    m_precDist[dest][src] = T->second;
  }
}

ShVariantPtr ShCastManager::doCast(int destIndex, ShVariantPtr srcValue, bool autoOnly)
{
  // start as srcValue and cast step by step to destIndex type 
  ShVariantPtr result = srcValue; 
  for(int srcIndex = result->typeIndex(); srcIndex != destIndex; 
      srcIndex = result->typeIndex()) {
    ShVariantCastPtr caster;

    if(autoOnly) caster = m_autoStep[destIndex][srcIndex];
    else caster = m_castStep[destIndex][srcIndex];

    SH_DEBUG_ASSERT(caster);
    result = (*caster)(result);
  }
  return result;
}

ShVariantCPtr ShCastManager::doCast(int destIndex, ShVariantCPtr srcValue, bool autoOnly)
{
  ShVariantCPtr result = srcValue;
  for(int srcIndex = result->typeIndex(); srcIndex != destIndex; 
      srcIndex = result->typeIndex()) {
    ShVariantCastPtr caster;

    if(autoOnly) caster = m_autoStep[destIndex][srcIndex];
    else caster = m_castStep[destIndex][srcIndex];

    SH_DEBUG_ASSERT(caster);
    result = (*caster)(result);
  }
  return result;
}

int ShCastManager::castDist(int destIndex, int srcIndex)
{
  return m_precDist[destIndex][srcIndex];
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
