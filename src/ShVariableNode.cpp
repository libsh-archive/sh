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

const char* ShVariableKindName[] = {
  "Input",
  "Output",
  "InOut",
  "Temp",
  "Constant",
  "Texture",
  "Stream"
};

const char* ShVariableSpecialTypeName[] = {
  "Attrib",
  "Point",
  "Vector",
  "Normal",
  "Color",
  "TexCoord",
  "Position"
};

ShVariableNode::ShVariableNode(ShVariableKind kind, int size, ShVariableSpecialType type)
  : m_uniform(!ShEnvironment::insideShader && kind != SH_TEXTURE && kind != SH_STREAM),
    m_kind(kind), m_specialType(type),
    m_size(size), m_id(m_maxID++), m_values(0),
    m_internal(false)
{
  if (m_uniform || m_kind == SH_CONST) {
    m_values = new ValueType[size];
    for (int i = 0; i < size; i++) m_values[i] = 0.0;
  }
  switch (m_kind) {
  case SH_INPUT:
    assert(ShEnvironment::shader);
    ShEnvironment::shader->inputs.push_back(this);
    break;
  case SH_OUTPUT:
    assert(ShEnvironment::shader);
    ShEnvironment::shader->outputs.push_back(this);
    break;
  case SH_INOUT:
    assert(ShEnvironment::shader);
    ShEnvironment::shader->outputs.push_back(this);
    ShEnvironment::shader->inputs.push_back(this);
    break;
  default:
    // Do nothing
    break;
  }

  switch (type) {
  case SH_ATTRIB:
    range(0, 1.0);
    break;
  case SH_POINT:
  case SH_VECTOR:
  case SH_NORMAL:
  case SH_POSITION:
    range(-1.0, 1.0);
    break;
  case SH_TEXCOORD:
  case SH_COLOR:
    range(0, 1.0);
    break;
  default:
    range(0, 1.0);
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
  if (m_kind == SH_CONST) {
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
  case SH_INPUT:
    stream << "i";
    break;
  case SH_OUTPUT:
    stream << "o";
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

void ShVariableNode::name(const std::string& name)
{
  m_name = name;
}

bool ShVariableNode::hasName()
{
  return !m_name.empty();
}

void ShVariableNode::range(ShVariableNode::ValueType low, ShVariableNode::ValueType high)
{
  m_lowBound = low;
  m_highBound = high;
}

ShVariableNode::ValueType ShVariableNode::lowBound() const
{
  return m_lowBound;
}

ShVariableNode::ValueType ShVariableNode::highBound() const
{
  return m_highBound;
}

void ShVariableNode::internal(bool setting)
{
  m_internal = setting;
}

bool ShVariableNode::internal() const
{
  return m_internal;
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
  // TODO indicate ValueType properly
  os << "Sh" << ShVariableKindName[ m_kind ] << ShVariableSpecialTypeName[ m_specialType ] << m_size << "f";
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

  if (m_uniform) {
    for (ShEnvironment::BoundShaderMap::iterator I = ShEnvironment::boundShaders().begin();
         I != ShEnvironment::boundShaders().end(); ++I) {
      // TODO: Why not tell code() directly?
      if (I->second) I->second->updateUniform(this);
    }
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
