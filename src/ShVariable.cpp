#include "ShVariable.hpp"

namespace SH {

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
  return m_node->size();
}

std::string ShVariable::name() const
{
  return m_node->name();
}

void ShVariable::name(const std::string& name)
{
  m_node->name(name);
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

void ShVariable::getValues(ShVariableNode::ValueType dest[]) const
{
  for (int i = 0; i < size(); i++) {
    dest[i] = m_node->getValue(m_swizzle[i]);
  }
}

void ShVariable::setValues(ShVariableNode::ValueType values[])
{
  for (int i = 0; i < size(); i++) {
    m_node->setValue(m_swizzle[i], values[i]);
  }
}

}
