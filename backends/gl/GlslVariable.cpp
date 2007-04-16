// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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


// State translation tables
const GlslVariable::ArbToGlslEntry arb_to_glsl_matrix_table[] = {
  {"modelview",  "gl_ModelViewMatrix",           false, 0},
  {"projection", "gl_ProjectionMatrix",          false, 0},
  {"mvp",        "gl_ModelViewProjectionMatrix", false, 0},
  {"texture",    "gl_TextureMatrix",             true , 0},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_face_material_table[] = {
  {"ambient",   "ambient",   false, 0},
  {"diffuse",   "diffuse",   false, 0},
  {"specular",  "specular",  false, 0},
  {"emission",  "emission",  false, 0},
  {"shininess", "shininess", false, 0},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_material_table[] = {
  {"ambient",   "gl_FrontMaterial.ambient",   false, 0},
  {"diffuse",   "gl_FrontMaterial.diffuse",   false, 0},
  {"specular",  "gl_FrontMaterial.specular",  false, 0},
  {"emission",  "gl_FrontMaterial.emission",  false, 0},
  {"shininess", "gl_FrontMaterial.shininess", false, 0},
  {"front",     "gl_FrontMaterial",           false, arb_to_glsl_face_material_table},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_light_table[] = {
  {"ambient",   "ambient",    false, 0},
  {"diffuse",   "diffuse",    false, 0},
  {"specular",  "specular",   false, 0},
  {"position",  "position",   false, 0},
  // NOTE: Attenuations are stored in separate variables on GLSL so we
  // can't map them directly here... perhaps a special case if necessary.
  {"half",      "halfVector", false, 0},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_lightmodel_table[] = {
  {"ambient", "ambient", false, 0},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_fog_table[] = {
  {"color", "color", false, 0},
  // As above, other fog parameters are combined in ARB and separate in
  // GLSL making the mapping non-trivial.
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_depth_table[] = {
  {"range", "vec3(gl_DepthRange.near, gl_DepthRange.far, gl_DepthRange.diff)", false, 0},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_state_table[] = {
  {"matrix",                   "", false, arb_to_glsl_matrix_table},
  {"material",                 "", false, arb_to_glsl_material_table},
  {"light",      "gl_LightSource", true,  arb_to_glsl_light_table},
  {"lightmodel",  "gl_LightModel", false, arb_to_glsl_lightmodel_table},
  {"fog",                "gl_Fog", false, arb_to_glsl_fog_table},
  {"depth",                    "", false, arb_to_glsl_depth_table},
  {0, 0, false}   // Indicates end of list!
};
const GlslVariable::ArbToGlslEntry arb_to_glsl_table[] = {
  {"state", "", false, arb_to_glsl_state_table},
  {0, 0, false}   // Indicates end of list!
};


GlslVariable::GlslVariable()
  : m_attribute(-1), m_builtin(false), m_texture(false), m_palette(false), 
    m_uniform(false), m_name(""), m_size(0), m_texture_shadow(false), m_dims(SH_TEXTURE_1D), m_length(0), 
    m_kind(SH_TEMP), m_type(SH_FLOAT), m_semantic_type(SH_ATTRIB), m_values("")
{
}

GlslVariable::GlslVariable(const GlslVariable& v)
  : m_attribute(v.m_attribute), m_builtin(v.m_builtin), m_texture(v.m_texture),
    m_palette(v.m_palette), m_uniform(v.m_uniform), m_name(v.m_name), 
    m_size(v.m_size), m_texture_shadow(v.m_texture_shadow), m_dims(v.m_dims), m_length(v.m_length), m_kind(v.m_kind), 
    m_type(v.m_type), m_semantic_type(v.m_semantic_type), m_values(v.m_values)
{
}

GlslVariable::GlslVariable(const VariableNodePtr& v)
  : m_attribute(-1), m_builtin(!v->meta("opengl:state").empty()), 
    m_texture(SH_TEXTURE == v->kind()), m_palette(SH_PALETTE == v->kind()),
    m_uniform(m_texture || m_palette || v->uniform()),
    m_name(v->meta("opengl:state")), m_size(v->size()), m_kind(v->kind()), 
    m_type(v->valueType()), m_semantic_type(v->specialType())
{
  // Translate any ARB state to GLSL state
  m_name = translate_state_arb_to_glsl(m_name, arb_to_glsl_table);

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
    TextureNodePtr texture = shref_dynamic_cast<TextureNode>(v);
    m_dims = texture->dims();
    m_texture_shadow = (texture->meta("opengl:textype") == "shadow");
  } else if (m_palette) {
    PaletteNodePtr palette = shref_dynamic_cast<PaletteNode>(v);
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
    varname << "_" << (unit == GLSL_FP ? "fp" : "vp");
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
    case BINDINGTYPE_END:
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
    if(m_texture_shadow) {
      s << "Shadow";
    }
    return s.str();
  }
  else {
    return glsl_typename(m_type, m_size);
  }
}

const GlslVariable::ArbToGlslEntry * GlslVariable::find
    (const std::string &value, const ArbToGlslEntry *table)
{
  if (!table) return 0;
  while (table->from) {
    if (value == table->from) {
      return table;
    }
    ++table;
  }
  return table;
}

std::string GlslVariable::parse_state_array
    (char &cur_delim, std::string &cur_string)
{
  // If the next cur_delimeter is an array, parse that out too
  if (cur_delim == '[') {
    std::string::size_type end = cur_string.find(']');
    if (end != std::string::npos) {      
      std::string array_value(cur_string, 0, end);
      // Assume that the next character after the terminator is a delim (or nothing)!
      if (end+1 < cur_string.size()) {
        cur_delim = cur_string[end+1];
        cur_string.assign(cur_string, end+2, std::string::npos);
      } else {
        cur_delim = ' ';
        cur_string.clear();
      }
      return array_value;
    } else {
      throw SH::Exception("Unterminated array index in opengl:state metadata");
    }
  } else {
    return std::string();
  }
}


std::string GlslVariable::translate_state_arb_to_glsl
  (const std::string &value, const ArbToGlslEntry *table)
{
  // Early-out
  if (value.empty()) return value;

  // Split at the given delimeters
  const char delims[] = " \n.[]";
  std::string::size_type split = value.find_first_of(delims);
  std::string left(value, 0, split);
  char delim = ' ';
  std::string right;
  if (split != std::string::npos) {
    delim = value[split];
    right.assign(value, split+1, std::string::npos);
  }

  // Check if we have an array index operator
  std::string array_value = parse_state_array(delim, right);
  std::string matrix_row_value;

  // Anything to process?
  if (!left.empty()) {
    std::string left_translated = left;
    std::string right_translated = right;
    bool use_array = !array_value.empty();

    // Look for our given left value
    const ArbToGlslEntry *entry = find(left, table);
    if (entry && entry->from) {
      left_translated.assign(entry->to);
      use_array = entry->to_is_array;

      // Ugly special cases!
      if (table == arb_to_glsl_matrix_table) {
        // By default, transpose matrix, since we need to access by ROWS
        bool inverse = false;
        bool transpose = true;

        // Look for .transpose, .inverse, etc.
        if (delim == '.') {
          std::string::size_type split = right.find('.');
          std::string modifier(right, 0, split);

          bool found = true;
          if (modifier == "inverse") {
            inverse = !inverse;
          } else if (modifier == "transpose") {
            transpose = !transpose;
          } else if (modifier == "invtrans") {
            inverse = !inverse;
            transpose = !transpose;
          } else {
            found = false;
          }

          // Comsume what we found
          if (found) {
            if (split < right.size()) {
              delim = right[split];
              right.assign(right, split+1, std::string::npos);
            } else {
              delim = ' ';
              right.clear();
            }
          }
        }

        // Add the correct GLSL names
        if (inverse)   left_translated += "Inverse";
        if (transpose) left_translated += "Transpose";

        // Look for a .row[x]
        if (delim == '.') {
          if (right.substr(0, 3) == "row") {
            // Consume and convert it
            if (right.size() > 3) {
              delim = right[3];
              right.assign(right, 4, std::string::npos);
            } else {
              delim = ' ';
              right.clear();
            }

            // Grab the array index (should hopefully exist!!)
            matrix_row_value = parse_state_array(delim, right);
          }
        }
      }

      // Translate the right hand portion using our new sub-table
      right_translated = translate_state_arb_to_glsl(right, entry->subtable);      
    } else {
      SH_DEBUG_WARN("No matching GLSL state found for ARB state '" << left << "'");
      // Translate the right hand portion using our current table
      right_translated = translate_state_arb_to_glsl(right, table);
    }

    // Form this part of the translation
    std::ostringstream oss;
    oss << left_translated;
    
    if (use_array) {
      // Add the GLSL array indexing operator
      oss << '[' << array_value << ']';
    }

    if (!matrix_row_value.empty()) {
      // Add the GLSL column indexing operator (we assume that we've
      // transposed already and thus this will actually access the row).
      oss << '[' << matrix_row_value << ']';
    }

    if (!right_translated.empty()) {
      if (!left_translated.empty()) {
        // Add the GLSL struct operator
        oss << '.';
      }
      oss << right_translated;
    }

    std::string translated = oss.str();
    return translated;
  } else {
    // Translate the right hand portion using our current table
    return translate_state_arb_to_glsl(right, table);
  }
}


}
