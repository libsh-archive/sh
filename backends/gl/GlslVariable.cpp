// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
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
  {"gl_MultiTexCoord0", 4, false},
  {"gl_MultiTexCoord1", 4, false},
  {"gl_MultiTexCoord2", 4, false},
  {"gl_MultiTexCoord3", 4, false},
  {"gl_MultiTexCoord4", 4, false},
  {"gl_MultiTexCoord5", 4, false},
  {"gl_MultiTexCoord6", 4, false},
  {"gl_MultiTexCoord7", 4, false},
  {"gl_FragCoord", 4, false},
  {"gl_Position", 4, false},
  {"gl_FrontColor", 4, false},
  {"gl_FrontSecondaryColor", 4, false},
  {"gl_FragDepth", 1, false},
  {"gl_FragColor", 4, false},
  {"gl_FragData", 4, true}
};

GlslVariable::GlslVariable()
  : m_attribute(-1), m_builtin(false), m_texture(false), m_palette(false), 
    m_uniform(false), m_name(""), m_size(0), m_dims(SH_TEXTURE_1D), m_length(0), 
    m_kind(SH_TEMP), m_type(SH_FLOAT), m_semantic_type(SH_ATTRIB), m_values("")
{
}

GlslVariable::GlslVariable(const GlslVariable& v)
  : m_attribute(v.m_attribute), m_builtin(v.m_builtin), m_texture(v.m_texture),
    m_palette(v.m_palette), m_uniform(v.m_uniform), m_name(v.m_name), 
    m_size(v.m_size), m_dims(v.m_dims), m_length(v.m_length), m_kind(v.m_kind), 
    m_type(v.m_type), m_semantic_type(v.m_semantic_type), m_values(v.m_values)
{
}

GlslVariable::GlslVariable(const ShVariableNodePtr& v)
  : m_attribute(-1), m_builtin(!v->meta("opengl:state").empty()), 
    m_texture(SH_TEXTURE == v->kind()), m_palette(SH_PALETTE == v->kind()),
    m_uniform(m_texture || m_palette || v->uniform()),
    m_name(v->meta("opengl:state")), m_size(v->size()), m_kind(v->kind()), 
    m_type(v->valueType()), m_semantic_type(v->specialType())
{
  if (v->hasValues()) {
    m_values = v->getVariant()->encodeArray();
    replace(m_values.begin(), m_values.end(), ';', ',');

    // Scalar constants are not assigned a variable
    if ((1 == m_size) && (SH_CONST == m_kind)) {
      m_name = type_string() + "(" + m_values + ")";
      m_builtin = true;
    }
  }
  if (m_texture) {
    ShTextureNodePtr texture = shref_dynamic_cast<ShTextureNode>(v);
    m_dims = texture->dims();
  } else if (m_palette) {
    ShPaletteNodePtr palette = shref_dynamic_cast<ShPaletteNode>(v);
    m_length = palette->palette_length();
  }
}

string GlslVariable::declaration() const
{
  if (m_builtin) return "";

  stringstream s;
  string type = type_string();

  if (m_kind == SH_CONST) {
    s << "const ";
  }

  s << type << " " << m_name;

  if (m_palette) {
    s << "[" << m_length << "]";
  }

  if (!m_values.empty() && !m_uniform) {
    s << " = " << type << "(" << m_values << ")";
  }

  return s.str();
}

void GlslVariable::name(int i, enum GlslProgramType unit)
{
  stringstream varname;
  if (m_uniform) {
    if (m_texture) {
      varname << "tex";
    } else if (m_palette) {
      varname << "pal";
    } else {
      varname << "uni";
    }
    varname << "_" << (unit == SH_GLSL_FP ? "fp" : "vp");
  } 
  else {
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
    case SH_BINDINGTYPE_END:
      SH_DEBUG_ASSERT(0); // Error
      break;
    }
  }
  varname << "_" << i;

  m_name = varname.str();
  m_builtin = false;
}

void GlslVariable::attribute(int index)
{
  SH_DEBUG_ASSERT(SH_FLOAT == m_type); // only float inputs are allowed
  stringstream name;
  name << "attrib" << index;
  m_name = name.str();
  m_attribute = index;
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
  if (m_texture) {
    stringstream s;
    s << "sampler";
    switch (m_dims) {
    case SH_TEXTURE_1D:
      s << "1D";
      break;
    case SH_TEXTURE_2D:
      s << "2D";
      break;
    case SH_TEXTURE_3D:
      s << "3D";
      break;
    case SH_TEXTURE_CUBE:
      s << "Cube";
      break;
    case SH_TEXTURE_RECT:
      s << "2DRect";
      break;
    }
    return s.str();
  }
  else {
    return glsl_typename(m_type, m_size);
  }
}

}
