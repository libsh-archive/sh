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
#include "GlslVariable.hpp"

namespace shgl {

using namespace SH;
using namespace std;

/// Information about the VarBinding members
const GlslVarBindingInfo GlslVariable::glslVarBindingInfo[] = {
  {"gl_TexCoord", 4, true},
  {"gl_Color", 4, false},
  {"gl_SecondaryColor", 4, false},
  {"gl_Vertex", 4, false},
  {"gl_Normal", 3, false},
  {"gl_FragCoord", 4, false},
  {"gl_Position", 4, false},
  {"gl_FrontColor", 4, false},
  {"gl_FrontSecondaryColor", 4, false},
  {"gl_FragDepth", 1, false},
  {"gl_FragColor", 4, false},
};

GlslVariable::GlslVariable()
  : m_builtin(false), m_uniform(false), m_name(""), m_size(0),
    m_kind(SH_TEMP), m_type(SH_FLOAT), m_semantic_type(SH_ATTRIB),
    m_values("")
{
}

GlslVariable::GlslVariable(const GlslVariable& v)
  : m_builtin(v.m_builtin), m_uniform(v.m_uniform), m_name(v.m_name), 
    m_size(v.m_size), m_kind(v.m_kind), m_type(v.m_type), 
    m_semantic_type(v.m_semantic_type), m_values(v.m_values)
{
}

GlslVariable::GlslVariable(const ShVariableNodePtr& v)
  : m_builtin(false), m_uniform(v->uniform()), m_size(v->size()),
    m_kind(v->kind()), m_type(v->valueType()), 
    m_semantic_type(v->specialType())
{
  if (v->hasValues()) {
    m_values = v->getVariant()->encodeArray();
  }
}

string GlslVariable::declaration() const
{
  if (m_builtin) return "";

  string ret;
  string type = type_string();

  if (m_kind == SH_CONST) {
    ret += "const ";
  }
  ret += type + " " + m_name;
  if (!m_values.empty() && !m_uniform) {
    ret += " = " + type + "(" + m_values + ")";
  }

  return ret;
}

void GlslVariable::name(int i, enum GlslProgramType unit)
{
  stringstream varname;
  if (m_uniform) {
    varname << "uni";
    varname << (unit == SH_GLSL_FP ? "fp" : "vp");
  } else {
    switch (m_kind) {
    case SH_INPUT:
      varname << "var_i";
      break;
    case SH_OUTPUT:
      varname << "var_o";
      break;
    case SH_INOUT:
      varname << "var_io";
      break;
    case SH_TEMP:
      varname << "var_t";
      break;
    case SH_CONST:
      varname << "var_c";
      break;
    case SH_TEXTURE:
      varname << "var_tex";
      break;
    case SH_STREAM:
      varname << "var_s";
      break;
    case SH_PALETTE:
      varname << "var_p";
      break;
    default:
      varname << "var";
    }
  }
  varname << "_" << i;

  m_name = varname.str();
  m_builtin = false;
}

void GlslVariable::builtin(GlslVarBinding binding, int index)
{
  stringstream name;
  name << glslVarBindingInfo[binding].name;
  if (index > -1) {
    name << '[' << index << ']';
  }

  m_name = name.str();
  m_size = glslVarBindingInfo[binding].size;
  m_builtin = true;
}

string GlslVariable::type_string() const
{
  stringstream s;
  if (shIsInteger(m_type)) {
    if (m_size > 1) {
      s << "ivec";
    } else {
      return "int";
    }
  } else {
    if (m_size > 1) {
      s << "vec";
    } else {
      return "float";
    }
  }

  s << m_size;
  return s.str();
}

}
