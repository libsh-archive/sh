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
  allocate_builtin_inputs();
  allocate_builtin_outputs();

  for (ShProgramNode::VarList::const_iterator i = m_shader->temps_begin();
       i != m_shader->temps_end(); i++) {
    allocate_temp(*i);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->constants_begin();
       i != m_shader->constants_end(); i++) {
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
  for (ShProgramNode::VarList::const_iterator i = m_shader->inputs_begin(); i != m_shader->inputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, true);
  }
}

void GlslVariableMap::allocate_builtin_outputs()
{
  for (ShProgramNode::VarList::const_iterator i = m_shader->outputs_begin(); i != m_shader->outputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, false);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->outputs_begin(); i != m_shader->outputs_end(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, true);
  }
}

void GlslVariableMap::allocate_temp(const ShVariableNodePtr& node)
{
  GlslVariable var(node);
  if (var.uniform()) {
    var.name(m_nb_uniform_variables++, m_unit);
  } else {
    var.name(m_nb_regular_variables++, m_unit);
  }
  map_insert(node, var);

  // Since this variable is not built-in, it must be declared
  if (var.uniform()) {
    m_uniform_declarations.push_back(var.declaration());
  } else {
    m_regular_declarations.push_back(var.declaration());
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

string GlslVariableMap::resolve(const ShVariable& v, int index)
{
  if (m_varmap.find(v.node()) == m_varmap.end()) {
    allocate_temp(v.node());
  }

  GlslVariable var(m_varmap[v.node()]);
  string s = var.name();
  
  if (!var.texture()) {
    if (-1 == index) {
      s += swizzle(v, var.size());
    } else if (0 == index) {
	s += ".x";
    } else if (1 == index) {
	s += ".y";
    } else if (2 == index) {
      s += ".z";
    } else if (3 == index) {
      s += ".w";
    }

    if (v.neg()) {
      s =  string("-(") + s + ")";
    }
  }

  return s;
}

string GlslVariableMap::swizzle(const ShVariable& v, int dest_size) const
{
  ShSwizzle swizzle = v.swizzle();

  if (swizzle.identity() && (swizzle.size() == dest_size)) return ""; // no need for a swizzle

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

const GlslVariable& GlslVariableMap::variable(const ShVariableNodePtr& node)
{
  return m_varmap[node];
}

bool GlslVariableMap::contains(const ShVariableNodePtr& node) const
{
  return (m_varmap.find(node) != m_varmap.end());
}

}
