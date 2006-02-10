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
#ifndef GLSLVARIABLEMAP_HPP
#define GLSLVARIABLEMAP_HPP

#include "Glsl.hpp"
#include "ShVariableNode.hpp"
#include "ShProgramNode.hpp"
#include "ShInternals.hpp"
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

struct GlslVariableDeclaration {
  std::string declaration;
  std::string sh_name;
};

class GlslVariableMap {
public:
  typedef std::list<GlslVariableDeclaration> DeclarationList;
  typedef std::list<SH::ShVariableNodePtr> NodeList;

private:
  typedef std::map<const SH::ShVariableNodePtr, GlslVariable> VarMap;

  SH::ShProgramNode* m_shader;
  SH::ShVarTransformMap* m_original_vars;
  GlslProgramType m_unit;

  unsigned m_nb_uniform_variables;
  unsigned m_nb_regular_variables;  
  VarMap m_varmap;
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
  void allocate_generic_vertex_input(const SH::ShVariableNodePtr& node, int index);
  void allocate_generic_vertex_inputs();
  void allocate_temp(const SH::ShVariableNodePtr& node);
  
  void map_insert(const SH::ShVariableNodePtr& node, GlslVariable var);

  std::string real_resolve(const SH::ShVariable& v, const std::string& array_index, int index);
  std::string swizzle(const SH::ShVariable& v, int var_size) const;
  std::string repeat_scalar(const std::string& name, SH::ShValueType type, int size) const;

public:
  GlslVariableMap(SH::ShProgramNode* shader, SH::ShVarTransformMap* original_vars, 
                  GlslProgramType unit);
  ~GlslVariableMap();

  std::string resolve(const SH::ShVariable& v, int index = -1);
  std::string resolve(const SH::ShVariable& v, const SH::ShVariable& index);

  const GlslVariable& variable(const SH::ShVariableNodePtr& node)
  {
    return m_varmap[node];
  }

  bool contains(const SH::ShVariableNodePtr& node) const
  {
    return (m_varmap.find(node) != m_varmap.end());
  }

  /// An alternate form of contains that also returns the associated variable
  /// The variable pointer is returned in the second parameter, if found
  /// This version is only necessary since GlslCode needs very fast access to
  /// this map when updating uniforms and thus we can't afford the double-
  /// search involved in calling contains followed by variable.
  bool contains(const SH::ShVariableNodePtr& node, const GlslVariable*& var) const
  {
    VarMap::const_iterator i = m_varmap.find(node);
    if (i != m_varmap.end()) {
      var = &i->second;
      return true;
    }
    else {
      return false;
    }
  }

  DeclarationList::const_iterator attribute_begin() const;
  DeclarationList::const_iterator attribute_end() const;
  DeclarationList::const_iterator uniform_begin() const;
  DeclarationList::const_iterator uniform_end() const;
  DeclarationList::const_iterator regular_begin() const;
  DeclarationList::const_iterator regular_end() const;
  
  NodeList::iterator node_begin() { return m_nodes.begin(); }
  NodeList::iterator node_end() { return m_nodes.end(); }
};

}

#endif /* GLSLVARIABLEMAP_HPP */
