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

/// Information about the VarBinding members
struct {
  char* name;
  bool indexed;
} glslVarBindingInfo[] = {
  {"gl_TexCoord", true},
  {"gl_Color", false},
  {"gl_SecondaryColor", false},
  {"gl_Vertex", false},
  {"gl_Normal", false},
  {"gl_FragCoord", false},
  {"gl_Position", false},
  {"gl_FrontColor", false},
  {"gl_FrontSecondaryColor", false},
  {"gl_FragDepth", false},
  {"gl_FragColor", false},
  {"<nil>", false}
};

GlslBindingSpecs glslVertexInputBindingSpecs[] = {
  {SH_GLSL_VAR_VERTEX, 1, SH_POSITION, false},
  {SH_GLSL_VAR_NORMAL, 1, SH_NORMAL, false},
  {SH_GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, true}
};

GlslBindingSpecs glslFragmentInputBindingSpecs[] = {
  {SH_GLSL_VAR_FRAGCOORD, 1, SH_POSITION, false},
  {SH_GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB, true}
};

GlslBindingSpecs glslVertexOutputBindingSpecs[] = {
  {SH_GLSL_VAR_POSITION, 1, SH_POSITION, false},
  {SH_GLSL_VAR_FRONTCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_FRONTSECONDARYCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB}
};

GlslBindingSpecs glslFragmentOutputBindingSpecs[] = {
  {SH_GLSL_VAR_FRAGDEPTH, 1, SH_POSITION, false},
  {SH_GLSL_VAR_FRAGCOLOR, 1, SH_COLOR, false},
  {SH_GLSL_VAR_NONE, 0, SH_ATTRIB}
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
  : m_shader(shader), m_unit(unit), m_nb_regular_variables(0), m_nb_varying_variables(0)
{
  allocate_builtin_inputs();
  allocate_builtin_outputs();

  for (ShProgramNode::VarList::const_iterator i = m_shader->inputs_begin();
       i != m_shader->inputs_end(); i++) {
    allocate_input(*i);
  }
  for (ShProgramNode::VarList::const_iterator i = m_shader->outputs_begin();
       i != m_shader->outputs_end(); i++) {
    allocate_output(*i);
  }
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
      std::ostringstream name;
      name << glslVarBindingInfo[s->binding].name;
      
      if (glslVarBindingInfo[s->binding].indexed) {
        name << '[' << bindings[s->binding] << ']';
      }
      var.name(name.str());
      m_varmap[node] = var;

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

void GlslVariableMap::allocate_input(const ShVariableNodePtr& node)
{
}

void GlslVariableMap::allocate_output(const ShVariableNodePtr& node)
{
}

void GlslVariableMap::allocate_temp(const ShVariableNodePtr& node)
{
  GlslVariable var(node);
  if (var.varying()) {
    var.name(m_nb_varying_variables++);
  } else {
    var.name(m_nb_regular_variables++);
  }
  m_varmap[node] = var;

  // Since this variable is not built-in, it must be declared
  if (var.varying()) {
    m_varying_declarations.push_back(var.declaration());
  } else {
    m_regular_declarations.push_back(var.declaration());
  }
}

GlslVariable GlslVariableMap::resolve(const ShVariableNodePtr& v)
{
  if (m_varmap.find(v) == m_varmap.end()) {
    allocate_temp(v);
  }
  return m_varmap[v];
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::regular_begin() const
{ 
  return m_regular_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::regular_end() const
{ 
  return m_regular_declarations.end();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::varying_begin() const
{ 
  return m_varying_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::varying_end() const
{
  return m_varying_declarations.end();
}

}
