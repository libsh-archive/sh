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

namespace SH {

const char* ShVariableSpecialTypeName[] = {
  "ShAttrib",
  "ShPoint",
  "ShVector",
  "ShNormal",
  "ShColor",
  "ShTexCoord",
  "ShPosition"
};

ShVariableNode::ShVariableNode(ShVariableKind kind, int size, ShVariableSpecialType type)
  : m_uniform(!ShEnvironment::insideShader && kind != SH_VAR_TEXTURE), m_kind(kind), m_specialType(type),
    m_size(size), m_id(m_maxID++), m_values(0)
{
  if (m_kind != SH_VAR_TEXTURE && (m_uniform || m_kind == SH_VAR_CONST)) {
    m_values = new ValueType[size];
    for (int i = 0; i < size; i++) m_values[i] = 0.0;
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
  return (m_values != NULL);
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

ShVariableSpecialType ShVariableNode::specialType() const
{
  return m_specialType;
}

std::string ShVariableNode::nameOfType() const {
  std::ostringstream os;
  os << ShVariableSpecialTypeName[ m_specialType ] << m_size;
  return os.str();
}

void ShVariableNode::specialType(ShVariableSpecialType type)
{
  m_specialType = type;
}

// TODO: also have an n-length set value, since updating the uniforms
// will otherwise be horribly inefficient.
void ShVariableNode::setValue(int i, ValueType value)
{
  assert(m_values);
  if (i < 0 || i >= m_size) return;
  m_values[i] = value;

  for (ShEnvironment::BoundShaderMap::iterator I = ShEnvironment::boundShaders().begin();
       I != ShEnvironment::boundShaders().end(); ++I) {
    // TODO: Why not tell code() directly?
    if (I->second) I->second->updateUniform(this);
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
