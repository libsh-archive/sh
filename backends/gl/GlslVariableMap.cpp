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

GlslVariableMap::GlslVariableMap(ShProgramNode* shader, GlslProgramType unit)
  : m_shader(shader), m_unit(unit),
    m_nb_regular_variables(0), m_nb_varying_variables(0),
    m_gl_Color_allocated(false), 
    m_gl_FragColor_allocated(false), 
    m_gl_FragCoord_allocated(false), 
    m_gl_FragDepth_allocated(false), 
    m_gl_FrontColor_allocated(false), 
    m_gl_FrontSecondaryColor_allocated(false), 
    m_gl_Normal_allocated(false),
    m_gl_Position_allocated(false), 
    m_gl_SecondaryColor_allocated(false),
    m_gl_Vertex_allocated(false) 
{
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

void GlslVariableMap::allocate_input(const ShVariableNodePtr& node)
{
  bool allocated = false;

  GlslVariable var(node);

  // Common to both units
  if (var.semantic_type() == SH_COLOR) {
    if (!m_gl_Color_allocated) {
      var.name("gl_Color");
      m_gl_Color_allocated = true;
      allocated = true;
    } else if (!m_gl_SecondaryColor_allocated) {
      var.name("gl_SecondaryColor");
      m_gl_SecondaryColor_allocated = true;
      allocated = true;
    }
  } else if (m_unit == SH_GLSL_VP) {
    // Speficic to the vertex unit
    switch (var.semantic_type()) {
    case SH_POSITION:
      if (!m_gl_Vertex_allocated) {
	var.name("gl_Vertex");
	m_gl_Vertex_allocated = true;
	allocated = true;
      }
      break;
    case SH_NORMAL:
      if (!m_gl_Normal_allocated) {
	var.name("gl_Normal");
	m_gl_Normal_allocated = true;
	allocated = true;
      }
      break;
    case SH_TEXCOORD:
      // TODO: use gl_MultiTexCoord*
      break;
    default:
      break; // No default allocation
    }
  } else {
    // Specific to the fragment unit
    switch (var.semantic_type()) {
    case SH_POSITION:
      if (!m_gl_FragCoord_allocated) {
	var.name("gl_FragCoord");
	m_gl_FragCoord_allocated = true;
	allocated = true;
      }
      break;
    case SH_TEXCOORD:
      // TODO: use gl_TexCoord[]
      break;
    default:
      break; // No default allocation
    }    
  }

  if (allocated) m_varmap[node] = var;
}

void GlslVariableMap::allocate_output(const ShVariableNodePtr& node)
{
  bool allocated = false;

  GlslVariable var(node);

  if (m_unit == SH_GLSL_VP) {
    // Speficic to the vertex unit
    switch (var.semantic_type()) {
    case SH_POSITION:
      if (!m_gl_Position_allocated) {
	var.name("gl_Position");
	m_gl_Position_allocated = true;
	allocated = true;
      }
      break;
    case SH_COLOR:
      if (!m_gl_FrontColor_allocated) {
	var.name("gl_FrontColor");
	m_gl_FrontColor_allocated = true;
	allocated = true;
      } else if (!m_gl_FrontSecondaryColor_allocated) {
	var.name("gl_FrontSecondaryColor");
	m_gl_FrontSecondaryColor_allocated = true;
	allocated = true;
      }
      break;
    case SH_TEXCOORD:
      // TODO: use gl_TexCoord[]
      break;
    default:
      break; // No default allocation
    }
  } else {
    // Specific to the fragment unit
    switch (var.semantic_type()) {
    case SH_POSITION:
      if (!m_gl_FragDepth_allocated) {
	var.name("gl_FragDepth");
	m_gl_FragDepth_allocated = true;
	allocated = true;
      }
      break;
    case SH_COLOR:
      if (!m_gl_FragColor_allocated) {
	var.name("gl_FragColor");
	m_gl_FragColor_allocated = true;
	allocated = true;
      }
      break;
    default:
      break; // No default allocation
    }    
  }

  if (allocated) m_varmap[node] = var;
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
