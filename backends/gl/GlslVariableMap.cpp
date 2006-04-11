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
#include "GlslVariableMap.hpp"

namespace shgl {

using namespace SH;
using namespace std;

GlslBindingSpecs glslVertexInputBindingSpecs[] = {
  {GLSL_VAR_VERTEX, 1, SH_POSITION, false},
  {GLSL_VAR_NORMAL, 1, SH_VECTOR, false},
  {GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {GLSL_VAR_MULTITEXCOORD0, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD1, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD2, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD3, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD4, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD5, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD6, 1, SH_TEXCOORD, true},
  {GLSL_VAR_MULTITEXCOORD7, 1, SH_TEXCOORD, true},
  {GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslFragmentInputBindingSpecs[] = {
  {GLSL_VAR_FRAGCOORD, 1, SH_POSITION, false},
  {GLSL_VAR_COLOR, 1, SH_COLOR, false},
  {GLSL_VAR_SECONDARYCOLOR, 1, SH_COLOR, false},
  {GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslVertexOutputBindingSpecs[] = {
  {GLSL_VAR_POSITION, 1, SH_POSITION, false},
  {GLSL_VAR_FRONTCOLOR, 1, SH_COLOR, false},
  {GLSL_VAR_FRONTSECONDARYCOLOR, 1, SH_COLOR, false},
  {GLSL_VAR_TEXCOORD, 8, SH_TEXCOORD, true},
  {GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs glslFragmentOutputBindingSpecs[] = {
  {GLSL_VAR_FRAGDEPTH, 1, SH_POSITION, false},
  {GLSL_VAR_FRAGCOLOR, 1, SH_COLOR, true},
  {GLSL_VAR_NONE, 0, SH_ATTRIB, false}
};

GlslBindingSpecs* glslBindingSpecs(bool output, GlslProgramType unit)
{
  if (unit == GLSL_VP) {
    return output ? glslVertexOutputBindingSpecs : glslVertexInputBindingSpecs;
  } else {
    return output ? glslFragmentOutputBindingSpecs : glslFragmentInputBindingSpecs;
  }
}

GlslVariableMap::GlslVariableMap(ProgramNode* shader, 
                                 VarTransformMap* original_vars, 
                                 GlslProgramType unit)
  : m_shader(shader), m_original_vars(original_vars), m_unit(unit),
    m_nb_uniform_variables(0), m_nb_regular_variables(0)
{
  const GLubyte* extensions = glGetString(GL_EXTENSIONS);
  if (extensions) {
    string extstr(reinterpret_cast<const char*>(extensions));
    
    if (GLSL_FP == unit) {
      if ((extstr.find("ATI_draw_buffers") != string::npos) || 
          (extstr.find("ARB_draw_buffers") != string::npos)) {
        glslFragmentOutputBindingSpecs[1].binding = GLSL_VAR_FRAGDATA;

        GLint max_draw_buffers;
#ifdef GL_ATI_draw_buffers
        if (extstr.find("ATI_draw_buffers") != string::npos) {
          GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ATI, &max_draw_buffers));
        } else {
#endif
          GL_CHECK_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers));
#ifdef GL_ATI_draw_buffers
        }
#endif
        glslFragmentOutputBindingSpecs[1].max_bindings = max_draw_buffers;
      }
    }
  }

  if ((GLSL_VP == unit) && (shader->meta("opengl:matching") == "generic")) {
    allocate_generic_vertex_inputs();
  } else {
    allocate_builtin_inputs();
  }
  allocate_builtin_outputs();

  for (ProgramNode::PaletteList::const_iterator i = m_shader->begin_palettes();
       i != m_shader->end_palettes(); i++) {
    allocate_temp(*i);
  }
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_temps();
       i != m_shader->end_temps(); i++) {
    allocate_temp(*i);
  }
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_constants();
       i != m_shader->end_constants(); i++) {
    allocate_temp(*i);
  }
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_parameters();
       i != m_shader->end_parameters(); i++) {
    allocate_temp(*i);
  }
}

GlslVariableMap::~GlslVariableMap()
{
  delete m_original_vars; // we own this object
}

void GlslVariableMap::allocate_builtin(const VariableNodePtr& node,
                                       const GlslBindingSpecs* specs, 
                                       map<GlslVarBinding, set<int> >& bindings,
                                       BuiltInPass pass_type)
{
  if (m_varmap.find(node) != m_varmap.end()) return;

  for (const GlslBindingSpecs* s = specs; s->binding != GLSL_VAR_NONE; s++) {
    int index = -1;
    bool bind_variable = true;
    if (USER_INDEX == pass_type) {
      if (s->semantic_type != node->specialType()) continue;

      string semantic_index = node->meta("opengl:semantic_index");
      if (semantic_index.empty()) continue; // will be done in a later pass

      stringstream ss;
      ss << semantic_index;
      ss >> index;

      if (bindings[s->binding].find(index) != bindings[s->binding].end()) {
        // Index not available, skip this binding
        continue;
      }

      if (!GlslVariable::glslVarBindingInfo[s->binding].indexed) {
        // Special case for a MULTITEXCOORD with a semantic_index
        // since it's not handled by the array code (.indexed is false)
        switch (s->binding) {
        case GLSL_VAR_MULTITEXCOORD0: bind_variable = (index == 0); break;
        case GLSL_VAR_MULTITEXCOORD1: bind_variable = (index == 1); break;
        case GLSL_VAR_MULTITEXCOORD2: bind_variable = (index == 2); break;
        case GLSL_VAR_MULTITEXCOORD3: bind_variable = (index == 3); break;
        case GLSL_VAR_MULTITEXCOORD4: bind_variable = (index == 4); break;
        case GLSL_VAR_MULTITEXCOORD5: bind_variable = (index == 5); break;
        case GLSL_VAR_MULTITEXCOORD6: bind_variable = (index == 6); break;
        case GLSL_VAR_MULTITEXCOORD7: bind_variable = (index == 7); break;
        default:
          DEBUG_WARN(string("Variable '") << node->name() 
                        << "' was assigned a semantic_index of " 
                        << index << " but it is not of an indexed type.  "
                        << "Ignoring user-specified index.");
          break;
        }
        if (!bind_variable) continue;
        index = -1;
      }
    } else {
      if ((GENERIC == pass_type)  && !s->allow_generic) continue;

      if (EXACT_SEMANTIC == pass_type) {
        if (s->semantic_type != node->specialType()) continue;

        string semantic_index = node->meta("opengl:semantic_index");
        if (!semantic_index.empty()) {        
          DEBUG_WARN(string("Variable '") << node->name() 
                        << "' was assigned a semantic_index of " << semantic_index
                        << " but that index has already been used.  "
                        << "Ignoring user-specified index.");
        }
      }

      if (GlslVariable::glslVarBindingInfo[s->binding].indexed) {
        // Go through all indices and check if they're taken
        bind_variable = false;
        for (int j=0; j < s->max_bindings; ++j) {
          if (bindings[s->binding].find(j) == bindings[s->binding].end()) {
            index = j;
            bind_variable = true;
            break;
          }
        }
      } else {
        index = -1;
        bind_variable = (bindings[s->binding].find(index) == bindings[s->binding].end());
      }
    }

    if (bind_variable) {
      GlslVariable var(node);
      var.builtin(s->binding, index);
      map_insert(node, var);
      
      bindings[s->binding].insert(index);
      return;
    } else {
      // Continue looking for a good match
    }
  }
}

void GlslVariableMap::allocate_generic_vertex_input(const VariableNodePtr& node, int index)
{
  // Create the Glsl variable
  GlslVariable var(node);
  var.attribute(index);
  map_insert(node, var);

  // Add to the declaration list
  GlslVariableDeclaration decl;
  decl.declaration = var.declaration();
  decl.sh_name = (node)->name();
  m_attribute_declarations.push_back(decl);

  // Set user-visible metadata on the original object
  stringstream s;
  s << index;
  m_original_vars->get_original_variable(node)->meta("opengl:attribindex", s.str());
}

void GlslVariableMap::allocate_generic_vertex_inputs()
{
  // The position has to be at index 0
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); 
       i != m_shader->end_inputs(); i++) {
    if ((*i)->specialType() == SH_POSITION) {
      allocate_generic_vertex_input(*i, 0);
      break;
    }
  }
  
  int index = 1;
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); 
       i != m_shader->end_inputs(); i++) {
    if (!contains(*i)) {
      allocate_generic_vertex_input(*i, index++);
    }
  }
}

void GlslVariableMap::allocate_builtin_inputs()
{
  // Pass 1: bind variables with user-specified index (opengl:semantic_index)
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); i != m_shader->end_inputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, USER_INDEX);
  }

  // Pass 2: bind variables matching to their exact semantic type
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); i != m_shader->end_inputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, EXACT_SEMANTIC);
  }
  
  // Pass 3: generic binding for the remaining variables
  int input_nb = 0;
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_inputs(); i != m_shader->end_inputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(false, m_unit), m_input_bindings, GENERIC);
    
    // warn if the input could not be bound to a built-in variable during the third pass
    if (m_varmap.find(*i) == m_varmap.end()) {
      cerr << "Could not bind " << (isInteger((*i)->valueType()) ? "int" : "float") 
           << " input " << input_nb << " to a built-in variable." << endl;
    }
    input_nb++;
  }
}

void GlslVariableMap::allocate_builtin_outputs()
{
  // Pass 1: bind variables with user-specified index (opengl:semantic_index)
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_outputs(); i != m_shader->end_outputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, USER_INDEX);
  }

  // Pass 2: bind variables matching to their exact semantic type
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_outputs(); i != m_shader->end_outputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, EXACT_SEMANTIC);
  }

  // Pass 3: generic binding for the remaining variables
  int output_nb = 0;
  for (ProgramNode::VarList::const_iterator i = m_shader->begin_outputs(); i != m_shader->end_outputs(); i++) {
    allocate_builtin(*i, glslBindingSpecs(true, m_unit), m_output_bindings, GENERIC);

    // warn if the output could not be bound to a built-in variable during the third pass
    if (m_varmap.find(*i) == m_varmap.end()) {
      cerr << "Could not bind " << (isInteger((*i)->valueType()) ? "int" : "float") 
	   << " output " << output_nb << " to a built-in variable." << endl;
    }
    output_nb++;
  }
}

void GlslVariableMap::allocate_temp(const VariableNodePtr& node)
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
    GlslVariableDeclaration decl;
    decl.declaration = var.declaration();
    decl.sh_name = node->name();
    if (var.uniform()) {
      m_uniform_declarations.push_back(decl);
    } else {
      m_regular_declarations.push_back(decl);
    }
  }
}

void GlslVariableMap::map_insert(const VariableNodePtr& node, GlslVariable var)
{
  m_nodes.push_back(node);
  m_varmap[node] = var;
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::attribute_begin() const
{ 
  return m_attribute_declarations.begin();
}

GlslVariableMap::DeclarationList::const_iterator GlslVariableMap::attribute_end() const
{ 
  return m_attribute_declarations.end();
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

string GlslVariableMap::real_resolve(const Variable& v, const string& array_index, int index)
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
        // Force the swizzle if the variable is built-in
        // This avoids an error if they declare state metadata for the "wrong" sized Sh variable
	      s += swizzle(v, var.size(), var.builtin());
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

string GlslVariableMap::resolve(const Variable& v, const Variable& index)
{
  return real_resolve(v, resolve(index), -1);
}

string GlslVariableMap::resolve(const Variable& v, int index)
{
  return real_resolve(v, "", index);
}

string GlslVariableMap::swizzle(const Variable& v, int var_size, bool force) const
{
  Swizzle swizzle = v.swizzle();

  if (!force && swizzle.identity() && (swizzle.size() == var_size)) {
    return ""; // no need for a swizzle
  }

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
      DEBUG_ASSERT(0); // Invalid swizzle
      return "";
    }
  }

  return "." + ss.str();
}

string GlslVariableMap::repeat_scalar(const string& name, ValueType type, int size) const
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

}
