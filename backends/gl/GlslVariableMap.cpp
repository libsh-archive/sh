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
#include "GlslVariableMap.hpp"

namespace shgl {

using namespace SH;
using namespace std;

GlslBindingSpecs glslVertexInputBindingSpecs[] = {
  {SH_GLSL_VAR_VERTEX, 1, SH_POSITION, false},
  {SH_GLSL_VAR_NORMAL, 1, SH_NORMAL, false},
  {SH_GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_MULTITEXCOORD0, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD1, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD2, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD3, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD4, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD5, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD6, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_MULTITEXCOORD7, 1, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslFragmentInputBindingSpecs[] = {
  {SH_GLSL_VAR_FRAGCOORD, 1, SH_POSITION, false},
  {SH_GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslVertexOutputBindingSpecs[] = {
  {SH_GLSL_VAR_POSITION, 1, SH_POSITION, false},
  {SH_GLSL_VAR_FRONTCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_FRONTSECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslFragmentOutputBindingSpecs[] = {
  {SH_GLSL_VAR_FRAGDEPTH, 1, SH_POSITION, false},
  {SH_GLSL_VAR_FRAGCOLOR, 1, SH_COLOR, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs* glslBindingSpecs(bool output, GlslProgramType unit)
{
  if (unit == SH_GLSL_VP) {
    return output ? glslVertexOutputBindingSpecs : glslVertexInputBindingSpecs;
  } else {
    return output ? glslFragmentOutputBindingSpecs : glslFragmentInputBindingSpecs;
  }
}

GlslVariableMap::GlslVariableMap(ShProgramNode* shader, GlslProgramType unit)
  : m_shader(shader), m_unit(unit), m_nb_uniform_variables(0),
    m_nb_regular_variables(0)
{
  const GLubyte* extensions = glGetString(GL_EXTENSIONS);
  if (extensions) {
    string extstr(reinterpret_cast<const char*>(extensions));
    
    if (SH_GLSL_FP == unit) {
      if ((extstr.find("ATI_draw_buffers") != string::npos) || 
          (extstr.find("ARB_draw_buffers") != string::npos)) {
	glslFragmentOutputBindingSpecs[1].binding = SH_GLSL_VAR_FRAGDATA;
	glslFragmentOutputBindingSpecs[1].max_bindings = 4; // TODO: get the real number from gl_MaxDrawBuffers
      }
    }
  }

  allocate_builtin_inputs();
  allocate_builtin_outputs();

  for (ShProgramNode::PaletteList::const_iterator i = m_shader->palettes_begin();
       i != m_shader->palettes_end(); i++) {
    allocate_temp(*i);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->temps_begin();
       i != m_shader->temps_end(); i++) {
    allocate_temp(*i);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->constants_begin();
       i != m_shader->constants_end(); i++) {
    allocate_temp(*i);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->uniforms_begin();
       i != m_shader->uniforms_end(); i++) {
    allocate_temp(*i);
  }
}

void GlslVariableMap::allocate_builtin(const ShVariableNodePtr& node,
                                       const GlslBindingSpecs* specs, map<GlslVarBinding, int>& bindings,
                                       bool generic)
{
  if (m_varmap.find(node) != m_varmap.end()) return;

  for (const GlslBindingSpecs* s = specs; s->binding != SH_GLSL_VAR_NONE; s++) {
    if (s->semantic_type == node->specialType() || (generic && s->allow_generic)) {
      if (bindings[s->binding] >= s->max_bindings) continue;

      GlslVariable var(node);
      int index = -1;
      if (GlslVariable::glslVarBindingInfo[s->binding].indexed) {
	index = bindings[s->binding];
      }
      var.builtin(s->binding, index);
      map_insert(node, var);

      bindings[s->binding]++;
      return;
    }
  }
}

void GlslVariableMap::allocate_builtin_inputs()
{
  for (ShProgramNode::VarList::const_iterator i = m_shader->inputs_begin(); i != m_shader->inputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, false);
  }

  int input_nb = 0;
  for (ShProgramNode::VarList::const_iterator i = m_shader->inputs_begin(); i != m_shader->inputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, true);
    
    // warn if the input could not be bound to a built-in variable during the second pass
    if (m_varmap.find(*i) == m_varmap.end()) {
      cerr << "Could not bind " << (shIsInteger((*i)->valueType()) ? "int" : "float") 
	   << " input " << input_nb << " to a built-in variable." << endl;
    }
    input_nb++;
  }
}

void GlslVariableMap::allocate_builtin_outputs()
{
  for (ShProgramNode::VarList::const_iterator i = m_shader->outputs_begin(); i != m_shader->outputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, false);
  }

  int output_nb = 0;
  for (ShProgramNode::VarList::const_iterator i = m_shader->outputs_begin(); i != m_shader->outputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, true);

    // warn if the output could not be bound to a built-in variable during the second pass
    if (m_varmap.find(*i) == m_varmap.end()) {
      cerr << "Could not bind " << (shIsInteger((*i)->valueType()) ? "int" : "float") 
	   << " output " << output_nb << " to a built-in variable." << endl;
    }
    output_nb++;
  }
}

void GlslVariableMap::allocate_temp(const ShVariableNodePtr& node)
{
  if (m_varmap.find(node) != m_varmap.end()) return;

  GlslVariable var(node);
  
  if (!var.builtin()) {
    if (var.uniform()) {
      var.name(m_nb_uniform_variables++, m_unit);
    } else {
      var.name(m_nb_regular_variables++, m_unit);
    }
  }

  map_insert(node, var);

  if (!var.builtin()) {
    if (var.uniform()) {
      m_uniform_declarations.push_back(var.declaration());
    } else {
      m_regular_declarations.push_back(var.declaration());
    }
  }
}

void GlslVariableMap::map_insert(const ShVariableNodePtr& node, GlslVariable var)
{
  m_nodes.push_back(node);
  m_varmap[node] = var;
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::uniform_begin() const
{ 
  return m_uniform_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::uniform_end() const
{ 
  return m_uniform_declarations.end();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::regular_begin() const
{ 
  return m_regular_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::regular_end() const
{ 
  return m_regular_declarations.end();
}

string GlslVariableMap::real_resolve(const ShVariable& v, const string& array_index, int index)
{
  if (m_varmap.find(v.node()) == m_varmap.end()) {
    allocate_temp(v.node());
  }

  GlslVariable var(m_varmap[v.node()]);
  string s = var.name();
  if (!array_index.empty()) {
    s += "[" + array_index + "]";
  }
 
  if (!var.texture()) {
    if (-1 == index) {      
      if (1 == var.size()) {
	// Scalars cannot be swizzled
	s = repeat_scalar(s, v.valueType(), v.swizzle().size());
      } else {
	s += swizzle(v, var.size());
      }
    } else if ((0 == index) && (v.size() > 1)) {
      s += ".x";
    } else if (1 == index) {
      s += ".y";
    } else if (2 == index) {
      s += ".z";
    } else if (3 == index) {
      s += ".w";
    }

    if (v.neg()) {
      s =  string("-") + s;
    }
  }

  return s;
}

string GlslVariableMap::resolve(const ShVariable& v, const ShVariable& index)
{
  return real_resolve(v, resolve(index), -1);
}

string GlslVariableMap::resolve(const ShVariable& v, int index)
{
  return real_resolve(v, "", index);
}

string GlslVariableMap::swizzle(const ShVariable& v, int var_size) const
{
  ShSwizzle swizzle = v.swizzle();

  if (swizzle.identity() && (swizzle.size() == var_size)) return ""; // no need for a swizzle

  stringstream ss;
  for (int i=0; i < v.size(); i++) {
    switch (swizzle[i]) {
    case 0:
      ss << "x";
      break;
    case 1:
      ss << "y";
      break;
    case 2:
      ss << "z";
      break;
    case 3:
      ss << "w";
      break;
    default:
      SH_DEBUG_ASSERT(0); // Invalid swizzle
      return "";
    }
  }

  return "." + ss.str();
}

string GlslVariableMap::repeat_scalar(const string& name, ShValueType type, int size) const
{
  if (1 == size) return name; // no need for a swizzle

  stringstream s;
  s << glsl_typename(type, size);

  s << "(";
  for (int i=0; i < size; i++) {
    if (i > 0) s << ", ";
    s << name;
  }
  s << ")";

  return s.str();
}

const GlslVariable& GlslVariableMap::variable(const ShVariableNodePtr& node)
{
  return m_varmap[node];
}

bool GlslVariableMap::contains(const ShVariableNodePtr& node) const
{
  return (m_varmap.find(node) != m_varmap.end());
}

}
