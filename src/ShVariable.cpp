#include "ShVariable.hpp"

namespace SH {

ShVariable::ShVariable(const ShVariableNodePtr& node)
  : m_node(node)
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

}
