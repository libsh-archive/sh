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
#include <cassert>
#include "ShEnvironment.hpp"
#include "ShVariableNode.hpp"
#include "ShDebug.hpp"
#include "ShContext.hpp"
#include "ShCloak.hpp"
#include "ShCloakFactory.hpp"

namespace SH {

const char* ShBindingTypeName[] = {
  "Input",
  "Output",
  "InOut",
  "",
  "Constant",
  "Texture",
  "Stream"
};

const char* ShSemanticTypeName[] = {
  "Attrib",
  "Point",
  "Vector",
  "Normal",
  "Color",
  "TexCoord",
  "Position"
};

ShVariableNode::ShVariableNode(ShBindingType kind, int size, int typeIndex, ShSemanticType type)
  : m_uniform(!ShContext::current()->parsing() && kind != SH_TEXTURE && kind != SH_STREAM),
    m_kind(kind), m_specialType(type),
    m_size(size), m_typeIndex(typeIndex), 
    m_id(m_maxID++), m_locked(0),
    m_cloak(0)
{
  if (m_uniform || m_kind == SH_CONST) {
    m_cloak = shTypeInfo(m_typeIndex)->cloakFactory()->generate(m_size);
  }
  programVarListInit();
}

ShVariableNode::ShVariableNode(const ShVariableNode& old, ShBindingType newKind,
          ShSemanticType newType, int newSize, int newTypeIndex, 
          bool updateVarList, bool keepUniform)
  : ShMeta(old), 
    m_uniform(old.m_uniform), m_kind(newKind), m_specialType(newType),
    m_size(newSize), m_typeIndex(newTypeIndex),
    m_id(m_maxID++), m_locked(0),
    m_cloak(0)
{
  if(!keepUniform) m_uniform = false;

  if(m_uniform || m_kind == SH_CONST) {
    m_cloak = shTypeInfo(m_typeIndex)->cloakFactory()->generate(m_size);
  }
  if(updateVarList) programVarListInit();
}

ShVariableNode::~ShVariableNode()
{
  /* TODO using a smart pointer now - probably shouldn't be though, so leave
   * these here just in case
  delete m_cloak; 
  m_cloak = 0;
  */
}

ShVariableNodePtr ShVariableNode::clone(ShBindingType newKind, 
    bool updateVarList, bool keepUniform) const
{
  return new ShVariableNode(*this, newKind, m_specialType, m_size, m_typeIndex, 
      updateVarList, keepUniform);
}

ShVariableNodePtr ShVariableNode::clone(ShBindingType newKind, 
    ShSemanticType newType, bool updateVarList, bool keepUniform) const
{
  return new ShVariableNode(*this, newKind, newType, m_size, m_typeIndex, 
      updateVarList, keepUniform);
}

ShVariableNodePtr ShVariableNode::clone(ShBindingType newKind, 
    ShSemanticType newType, int newSize, 
    bool updateVarList, bool keepUniform) const
{
  return new ShVariableNode(*this, newKind, newType, newSize, m_typeIndex, 
      updateVarList, keepUniform);
}

ShVariableNodePtr ShVariableNode::clone(int newTypeIndex,
    bool updateVarList, bool keepUniform) const
{
  return new ShVariableNode(*this, m_kind, m_specialType, m_size, newTypeIndex, 
      updateVarList, keepUniform);
}

bool ShVariableNode::uniform() const
{
  return m_uniform;
}

bool ShVariableNode::hasValues() const
{
  return (m_cloak);
}

void ShVariableNode::lock()
{
  m_locked++;
}

void ShVariableNode::unlock()
{
  m_locked--;
  if (m_locked == 0 && m_uniform) {
    
    for (ShBoundIterator I = shBeginBound(); I != shEndBound(); ++I) {
      // TODO: Maybe pass in the backend unit to updateUniform
      if (I->second.node()) I->second.updateUniform(this);
    }
  }
}

int ShVariableNode::size() const
{
  return m_size;
}

void ShVariableNode::size(int s)
{
  SH_DEBUG_ASSERT(!m_cloak);
  m_size = s;
}

void ShVariableNode::name(const std::string& n)
{
  this->ShMeta::name(n);
}

std::string ShVariableNode::name() const
{

  if (has_name()) return this->ShMeta::name();
  
  std::ostringstream stream;
  
  // Special case for constants
  if (m_kind == SH_CONST) {
    return m_cloak->encode();
  }

  switch (m_kind) {
  case SH_INPUT:
    stream << "i";
    break;
  case SH_OUTPUT:
    stream << "o";
    break;
  case SH_INOUT:
    stream << "io";
    break;
  case SH_TEMP:
    stream << "t";
    break;
  case SH_CONST:
    stream << "c";
    break;
  case SH_TEXTURE:
    stream << "tex";
    break;
  case SH_STREAM:
    stream << "str";
    break;
  }

  stream << m_id;

  return stream.str();
}

void ShVariableNode::rangeCloak(ShCloakCPtr low, ShCloakCPtr high) 
{
  // TODO slow, possibly?
  int indices[m_size];
  std::fill_n(indices, m_size, 0);
  ShSwizzle swiz(1, m_size, indices); 
  meta("lowBound", low->get(false, swiz)->encode());
  meta("highBound", high->get(false, swiz)->encode());
}

void ShVariableNode::rangeCloak(ShCloakCPtr low, ShCloakCPtr high, bool neg, const ShSwizzle &writemask)
{
  ShTypeInfoPtr typeInfo = shTypeInfo(m_typeIndex);
  ShCloakFactoryCPtr factory = typeInfo->cloakFactory();

  std::string oldLo, oldHi;
  oldLo = meta("lowBound");
  oldHi = meta("highBound");

  ShCloakPtr newLo, newHi; 
  if(oldLo.empty() || oldHi.empty()) { // TODO they should be either both empty or both not
    newLo = factory->generateLowBound(m_size, m_specialType);
    newHi = factory->generateHighBound(m_size, m_specialType);
  } else {
    newLo = factory->generate(oldLo);
    newHi = factory->generate(oldHi);
  }

  newLo->set(low, neg, writemask);
  newHi->set(high, neg, writemask);

  meta("lowBound", newLo->encode());
  meta("highBound", newHi->encode());
}

ShCloakPtr ShVariableNode::lowBoundCloak() const
{
  ShCloakFactoryCPtr factory = shTypeInfo(m_typeIndex)->cloakFactory();
  std::string metaLow = meta("lowBound");

  return (metaLow.empty() ? 
    factory->generateLowBound(m_size, m_specialType) 
    : factory->generate(metaLow));
}

ShCloakPtr ShVariableNode::highBoundCloak() const
{
  ShCloakFactoryCPtr factory = shTypeInfo(m_typeIndex)->cloakFactory();
  std::string metaHigh = meta("highBound");

  return (metaHigh.empty() ? 
    factory->generateHighBound(m_size, m_specialType) 
    : factory->generate(metaHigh));
}

ShBindingType ShVariableNode::kind() const
{
  return m_kind;
}

ShSemanticType ShVariableNode::specialType() const
{
  return m_specialType;
}

std::string ShVariableNode::nameOfType() const {
  std::ostringstream os;
  // TODO indicate ValueType properly
  os << "Sh" << ShBindingTypeName[ m_kind ] << ShSemanticTypeName[ m_specialType ] << m_size << "f";
  return os.str();
}

void ShVariableNode::specialType(ShSemanticType type)
{
  m_specialType = type;
}

// TODO: also have an n-length set value, since updating the uniforms
// will otherwise be horribly inefficient.
void ShVariableNode::setCloak(ShCloakCPtr other)
{
  assert(m_cloak);
  m_cloak->set(other);
  uniformUpdate();
}

void ShVariableNode::setCloak(ShCloakCPtr other, bool neg, const ShSwizzle &writemask)
{
  assert(m_cloak);
  m_cloak->set(other, neg, writemask);
  uniformUpdate();
}

ShCloakCPtr ShVariableNode::cloak() const
{
  return m_cloak;
}

void ShVariableNode::uniformUpdate() 
{
  if (m_uniform && !m_locked) {
    for (ShBoundIterator I = shBeginBound(); I != shEndBound(); ++I) {
      // TODO: Maybe pass in the backend unit to updateUniform
      if (I->second.node()) I->second.updateUniform(this);
    }
  }
}

void ShVariableNode::programVarListInit() 
{
  switch (m_kind) {
  case SH_INPUT:
    assert(ShContext::current()->parsing());
    ShContext::current()->parsing()->inputs.push_back(this);
    break;
  case SH_OUTPUT:
    assert(ShContext::current()->parsing());
    ShContext::current()->parsing()->outputs.push_back(this);
    break;
  case SH_INOUT:
    assert(ShContext::current()->parsing());
    ShContext::current()->parsing()->outputs.push_back(this);
    ShContext::current()->parsing()->inputs.push_back(this);
    break;
  default:
    // Do nothing
    break;
  }
}

int ShVariableNode::m_maxID = 0;

}
