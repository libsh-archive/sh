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
  : m_shader(shader), m_unit(unit), m_nb_variables(0)
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
      int index = -1;
      if (GlslVariable::glslVarBindingInfo[s->binding].indexed) {
	index = bindings[s->binding];
      }
      var.builtin(s->binding, index);
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
  var.name(m_nb_variables++);
  m_varmap[node] = var;

  // Since this variable is not built-in, it must be declared
  m_declarations.push_back(var.declaration());
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::begin() const
{ 
  return m_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::end() const
{ 
  return m_declarations.end();
}

string GlslVariableMap::resolve(const ShVariable& v, int src_size)
{
  if (m_varmap.find(v.node()) == m_varmap.end()) {
    allocate_temp(v.node());
  }

  GlslVariable var(m_varmap[v.node()]);
  string s = var.name() + swizzle(v, var.size(), src_size);
  if (v.neg()) {
    s =  string("-(") + s + ")";
  }
  return s;
}

string GlslVariableMap::swizzle(const ShVariable& v, int dest_size, int src_size) const
{
  ShSwizzle swizzle = v.swizzle();

  if (dest_size != src_size) {
    // Fit this variable to the variable it's assigned to using the proper write mask
    int* s = new int[dest_size];
    for (int i = 0; i < dest_size; i++) s[i] = i;
    swizzle = ShSwizzle(dest_size, src_size, s);
    delete s;
    swizzle = v.swizzle() * swizzle;
  }

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

}
