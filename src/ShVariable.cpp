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
#include <iostream>

#include "ShVariable.hpp"
#include "ShCloak.hpp"

namespace SH {

ShVariable::ShVariable()
  : ShMetaForwarder(0),
    m_node(0), m_swizzle(0), m_neg(false)
{
}

ShVariable::ShVariable(const ShVariableNodePtr& node)
  : ShMetaForwarder(node.object()),
    m_node(node), m_swizzle(node ? node->size() : 0), m_neg(false)
{
}

ShVariable::ShVariable(const ShVariableNodePtr& node,
                       const ShSwizzle& swizzle,
                       bool neg)
  : ShMetaForwarder(node.object()),
    m_node(node), m_swizzle(swizzle), m_neg(neg)
{
}

bool ShVariable::null() const
{
  return !m_node;
}

bool ShVariable::uniform() const
{
  return m_node->uniform();
}

bool ShVariable::hasValues() const
{
  return m_node->hasValues();
}

int ShVariable::size() const
{
  return m_swizzle.size();
}

int ShVariable::typeIndex() const 
{
  if(!m_node) return 0;
  return m_node->typeIndex();
}

void ShVariable::rangeCloak(ShCloakCPtr low, ShCloakCPtr high)
{
  m_node->rangeCloak(low, high, m_neg, m_swizzle);
}

ShCloakPtr ShVariable::lowBoundCloak() const
{
  return m_node->lowBoundCloak()->get(m_neg, m_swizzle);
}

ShCloakPtr ShVariable::highBoundCloak() const
{
  return m_node->highBoundCloak()->get(m_neg, m_swizzle);
}

const ShSwizzle& ShVariable::swizzle() const
{
  return m_swizzle;
}

const ShVariableNodePtr& ShVariable::node() const
{
  return m_node;
}

bool ShVariable::neg() const
{
  return m_neg;
}

bool& ShVariable::neg()
{
  return m_neg;
}

ShCloakPtr ShVariable::cloak() const
{
  return m_node->cloak()->get(m_neg, m_swizzle);
}

ShCloakPtr ShVariable::cloak(int index) const
{
  return m_node->cloak()->get(m_neg, m_swizzle * ShSwizzle(size(), index)); 
}

void ShVariable::setCloak(ShCloakCPtr other, bool neg, const ShSwizzle &writemask)
{
  m_node->setCloak(other, neg ^ m_neg, m_swizzle * writemask);
}

void ShVariable::setCloak(ShCloakCPtr other, int index) 
{
  m_node->setCloak(other, m_neg, m_swizzle * ShSwizzle(size(), index));
}

void ShVariable::setCloak(ShCloakCPtr other)
{
  m_node->setCloak(other, m_neg, m_swizzle);
}

ShVariable ShVariable::operator()() const
{
  return ShVariable(m_node, m_swizzle, m_neg);
}

ShVariable ShVariable::operator()(int i1) const
{
  return ShVariable(m_node, m_swizzle * ShSwizzle(size(), i1), m_neg);
}

ShVariable ShVariable::operator()(int i1, int i2) const
{
  return ShVariable(m_node, m_swizzle * ShSwizzle(size(), i1, i2), m_neg);
}

ShVariable ShVariable::operator()(int i1, int i2, int i3) const
{
  return ShVariable(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3), m_neg);
}

ShVariable ShVariable::operator()(int i1, int i2, int i3, int i4) const
{
  return ShVariable(m_node, m_swizzle * ShSwizzle(size(), i1, i2, i3, i4), m_neg);
}

ShVariable ShVariable::operator()(int n, int indices[]) const
{
  return ShVariable(m_node, m_swizzle * ShSwizzle(size(), n, indices), m_neg);
}

std::ostream& operator<<(std::ostream& _out, const ShVariable& shVariableToPrint) {
 
  if (!shVariableToPrint.m_node){
    _out<<"[null]";
    return _out;
  }

  _out<<'[';
  _out << shVariableToPrint.cloak()->encode();
  _out<<']';
  return _out;
}


ShVariable ShVariable::operator-() const
{
  return ShVariable(m_node, m_swizzle, !m_neg);
}

bool ShVariable::operator==(const ShVariable& other) const
{
  return m_node == other.m_node && m_swizzle == other.m_swizzle && m_neg == other.m_neg;
}


}
