#include "ShProgram.hpp"
#include "ShBackend.hpp"

namespace SH {

ShProgram::ShProgram()
  : ShMetaForwarder(0),
    m_node(0)
{
}

ShProgram::ShProgram(const std::string& target)
  : ShMetaForwarder(0),
    m_node(new ShProgramNode(target))
{
  real_meta(m_node.object());
}

ShProgram::ShProgram(const ShProgram& other)
  : ShMetaForwarder(other.m_node.object()),
    m_node(other.m_node)
{
}

ShProgram::ShProgram(const ShProgramNodePtr& node)
  : ShMetaForwarder(node.object()),
    m_node(node)
{
}

ShProgram& ShProgram::operator=(const ShProgram& other)
{
  m_node = other.m_node;
  real_meta(m_node.object());
  return *this;
}

}
