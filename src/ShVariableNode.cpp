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
#include <algorithm>
#include "ShEnvironment.hpp"
#include "ShVariableNode.hpp"
#include "ShDebug.hpp"
#include "ShContext.hpp"
#include "ShProgramNode.hpp"
#include "ShEvaluate.hpp"

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

ShVariableNode::ShVariableNode(ShBindingType kind, int size, ShSemanticType type)
  : m_uniform(!ShContext::current()->parsing() && kind == SH_TEMP),
    m_kind(kind), m_specialType(type),
    m_size(size), m_id(m_maxID++), m_locked(0),
    m_values(0)
{
  if (m_uniform || m_kind == SH_CONST) addValues();
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
  detach_dependencies();
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

    update_dependents();
  }
}

int ShVariableNode::size() const
{
  return m_size;
}

void ShVariableNode::size(int s)
{
  SH_DEBUG_ASSERT(!m_values);
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
void ShVariableNode::setValue(int i, ValueType value)
{
  assert(m_values);
  if (i < 0 || i >= m_size) return;
  m_values[i] = value;

  if (m_uniform && !m_locked) {
    for (ShBoundIterator I = shBeginBound(); I != shEndBound(); ++I) {
      // TODO: Maybe pass in the backend unit to updateUniform
      if (I->second.node()) I->second.updateUniform(this);
    }

    update_dependents();
  }
}

ShVariableNode::ValueType ShVariableNode::getValue(int i) const
{
  assert(m_values);
  assert(i >= 0 && i < m_size);
  return m_values[i];
}

void ShVariableNode::addValues()
{
  if (m_values) return;
  m_values = new ValueType[m_size];
  for (int i = 0; i < m_size; i++) m_values[i] = 0.0;
}

void ShVariableNode::attach(const ShProgramNodePtr& evaluator)
{
  SH_DEBUG_ASSERT(uniform());
  // TODO: Check that the program really evaluates this variable.

  detach_dependencies();

  m_evaluator = evaluator;

  if (m_evaluator) {
    for (ShProgramNode::VarList::const_iterator I = m_evaluator->uniforms_begin();
         I != m_evaluator->uniforms_end(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->add_dependent(this);
    }
    
    update();
  }
}

void ShVariableNode::update()
{
  if (!m_evaluator) return;

  evaluate(m_evaluator);
}

void ShVariableNode::add_dependent(ShVariableNode* dep)
{
  if (std::find(m_dependents.begin(), m_dependents.end(), dep) != m_dependents.end()) return;
  m_dependents.push_back(dep);
}

void ShVariableNode::remove_dependent(ShVariableNode* dep)
{
  m_dependents.remove(dep);
}

void ShVariableNode::update_dependents()
{
  for (std::list<ShVariableNode*>::iterator I = m_dependents.begin();
       I != m_dependents.end(); ++I) {
    (*I)->update();
  }
}

void ShVariableNode::detach_dependencies()
{
  if (m_evaluator) {
    for (ShProgramNode::VarList::const_iterator I = m_evaluator->uniforms_begin();
         I != m_evaluator->uniforms_end(); ++I) {
      if ((*I).object() == this) continue;
      (*I)->remove_dependent(this);
    }
    m_evaluator = 0;
  }
}

int ShVariableNode::m_maxID = 0;

}
