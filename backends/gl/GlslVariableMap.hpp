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
#ifndef GLSLVARIABLEMAP_HPP
#define GLSLVARIABLEMAP_HPP

#include "Glsl.hpp"
#include "ShVariableNode.hpp"
#include "ShProgramNode.hpp"
#include "GlslVariable.hpp"
#include <map>
#include <string>
#include <list>
#include <string>

namespace shgl {

struct GlslBindingSpecs {
  GlslVarBinding binding;
  int max_bindings;
  SH::ShSemanticType semantic_type;
  bool allow_generic;
};

class GlslVariableMap {
public:
  GlslVariableMap(SH::ShProgramNode* shader, GlslProgramType unit);
  
  std::string resolve(const SH::ShVariable& v, int index = -1);
  std::string resolve(const SH::ShVariable& v, const SH::ShVariable& index);
  const GlslVariable& variable(const SH::ShVariableNodePtr& node);
  bool contains(const SH::ShVariableNodePtr& node) const;

  typedef std::list<std::string> DeclarationList;

  DeclarationList::const_iterator attribute_begin() const;
  DeclarationList::const_iterator attribute_end() const;
  DeclarationList::const_iterator uniform_begin() const;
  DeclarationList::const_iterator uniform_end() const;
  DeclarationList::const_iterator regular_begin() const;
  DeclarationList::const_iterator regular_end() const;

  typedef std::list<SH::ShVariableNodePtr> NodeList;
  
  NodeList::iterator node_begin() { return m_nodes.begin(); }
  NodeList::iterator node_end() { return m_nodes.end(); }

private:
  SH::ShProgramNode* m_shader;
  GlslProgramType m_unit;

  unsigned m_nb_uniform_variables;
  unsigned m_nb_regular_variables;
  std::map<const SH::ShVariableNodePtr, GlslVariable> m_varmap;
  NodeList m_nodes;
  
  DeclarationList m_attribute_declarations;
  DeclarationList m_uniform_declarations;
  DeclarationList m_regular_declarations;

  std::map<GlslVarBinding, int> m_input_bindings;
  std::map<GlslVarBinding, int> m_output_bindings;

  void allocate_builtin(const SH::ShVariableNodePtr& node,
                        const GlslBindingSpecs* specs, std::map<GlslVarBinding, int>& bindings,
                        bool generic);
  void allocate_builtin_inputs();
  void allocate_builtin_outputs();
  void allocate_generic_vertex_inputs();
  void allocate_temp(const SH::ShVariableNodePtr& node);
  
  void map_insert(const SH::ShVariableNodePtr& node, GlslVariable var);

  std::string real_resolve(const SH::ShVariable& v, const std::string& array_index, int index);
  std::string swizzle(const SH::ShVariable& v, int var_size) const;
  std::string repeat_scalar(const std::string& name, SH::ShValueType type, int size) const;
};

}

#endif /* GLSLVARIABLEMAP_HPP */
