#include <sstream>
#include <cassert>
#include "ShEnvironment.hpp"
#include "ShVariableNode.hpp"

namespace SH {

ShVariableNode::ShVariableNode(ShVariableKind kind, int size)
  : m_uniform(!ShEnvironment::insideShader && kind != SH_VAR_TEXTURE), m_kind(kind), m_size(size),
    m_id(m_maxID++), m_values(0)
{
  if (m_kind != SH_VAR_TEXTURE && (m_uniform || m_kind == SH_VAR_CONST)) {
    m_values = new ValueType[size];
  }
  switch (m_kind) {
  case SH_VAR_INPUT:
    assert(ShEnvironment::shader);
    ShEnvironment::shader->inputs.push_back(this);
    break;
  case SH_VAR_OUTPUT:
    assert(ShEnvironment::shader);
    ShEnvironment::shader->outputs.push_back(this);
    break;
  default:
    // Do nothing
    break;
  }
}

ShVariableNode::~ShVariableNode()
{
  delete [] m_values;
  m_values = 0;
}

bool ShVariableNode::uniform() const
{
  return m_uniform;
}

bool ShVariableNode::hasValues() const
{
  return m_values;
}

int ShVariableNode::size() const
{
  return m_size;
}

std::string ShVariableNode::name() const
{
  std::ostringstream stream;

  // Special case for constants
  if (m_kind == SH_VAR_CONST) {
    if (m_size == 1) {
      stream << m_values[0];
    } else {
      stream << "(";
      for (int i = 0; i < m_size; i++) {
        if (i > 0) stream << ", ";
        stream << m_values[i];
      }
      stream << ")";
    }
    stream << std::ends;
    return stream.str();
  }

  if (!m_name.empty()) return m_name;
  
  switch (m_kind) {
  case SH_VAR_INPUT:
    stream << "i";
    break;
  case SH_VAR_OUTPUT:
    stream << "o";
    break;
  case SH_VAR_TEMP:
    stream << "t";
    break;
  case SH_VAR_CONST:
    stream << "c";
    break;
  case SH_VAR_TEXTURE:
    stream << "tex";
    break;
  }

  stream << m_id;
  stream << std::ends;

  return stream.str();
}

void ShVariableNode::name(const std::string& name)
{
  m_name = name;
}

ShVariableKind ShVariableNode::kind() const
{
  return m_kind;
}

// TODO: also have an n-length set value, since updating the uniforms
// will otherwise be horribly inefficient.
void ShVariableNode::setValue(int i, ValueType value)
{
  assert(m_values);
  if (i < 0 || i >= m_size) return;
  m_values[i] = value;

  for (int s = 0; s < shShaderKinds; s++) {
    if (ShEnvironment::boundShader[s]) ShEnvironment::boundShader[s]->updateUniform(this);
  }
}

ShVariableNode::ValueType ShVariableNode::getValue(int i) const
{
  assert(m_values);
  assert(i >= 0 && i < m_size);
  return m_values[i];
}

int ShVariableNode::m_maxID = 0;

}
