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

namespace SH {

ShVariable::ShVariable()
  : m_node(0), m_swizzle(0), m_neg(false)
{
}

ShVariable::ShVariable(const ShVariableNodePtr& node)
  : m_node(node), m_swizzle(node ? node->size() : 0), m_neg(false)
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

std::string ShVariable::name() const
{
  return m_node->name();
}

void ShVariable::name(const std::string& name)
{
  m_node->name(name);
}

void ShVariable::range(ShVariableNode::ValueType low, ShVariableNode::ValueType high)
{
  m_node->range(low, high);
}

ShVariableNode::ValueType ShVariable::lowBound() const
{
  return m_node->lowBound();
}

ShVariableNode::ValueType ShVariable::highBound() const
{
  return m_node->highBound();
}

void ShVariable::internal(bool setting)
{
  m_node->internal(setting);
}

bool ShVariable::internal() const
{
  return m_node->internal();
}

const ShSwizzle& ShVariable::swizzle() const
{
  return m_swizzle;
}

const ShVariableNodePtr& ShVariable::node() const
{
  return m_node;
}

ShVariableNodePtr& ShVariable::node() 
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

void ShVariable::getValues(ShVariableNode::ValueType dest[]) const
{
  for (int i = 0; i < size(); i++) {
    dest[i] = m_node->getValue(m_swizzle[i]);
    if (m_neg) dest[i] = -dest[i];
  }
}

ShVariableNode::ValueType ShVariable::getValue(int i) const
{
  ShVariableNode::ValueType value = m_node->getValue(m_swizzle[i]);
  if (m_neg) value = -value;
  return value;
}

void ShVariable::setValues(ShVariableNode::ValueType values[])
{
  for (int i = 0; i < size(); i++) {
    m_node->setValue(m_swizzle[i], values[i]);
  }
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


std::ostream& operator<<(std::ostream& _out, const ShVariable& shVariableToPrint){
 
  if (!shVariableToPrint.m_node){
    _out<<"[null]";
    return _out;
  }

  _out<<'[';

  if(shVariableToPrint.size() > 0)
    _out<<shVariableToPrint.m_node->getValue(shVariableToPrint.swizzle()[0]);

  for (int k = 1; k < shVariableToPrint.size(); k++) {
    _out<<", ";
    _out<<shVariableToPrint.m_node->getValue(shVariableToPrint.swizzle()[k]);
   }

  _out<<']';
  return _out;
}


ShVariable ShVariable::operator-() const
{
  return ShVariable(m_node, m_swizzle, !m_neg);
}

}
