// Sh: A GPU metaprogramming language.
//
// Copyright 2005 Serious Hack Inc.
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
  const GlslVariable& variable(const SH::ShVariableNodePtr& node);
  bool contains(const SH::ShVariableNodePtr& node) const;

  typedef std::list<std::string> DeclarationList;

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
  
  DeclarationList m_uniform_declarations;
  DeclarationList m_regular_declarations;

  std::map<GlslVarBinding, int> m_input_bindings;
  std::map<GlslVarBinding, int> m_output_bindings;

  void allocate_builtin(const SH::ShVariableNodePtr& node,
                        const GlslBindingSpecs* specs, std::map<GlslVarBinding, int>& bindings,
                        bool generic);
  void allocate_builtin_inputs();
  void allocate_builtin_outputs();
  void allocate_temp(const SH::ShVariableNodePtr& node);
  
  void map_insert(const SH::ShVariableNodePtr& node, GlslVariable var);

  std::string swizzle(const SH::ShVariable& v, int var_size) const;
  std::string repeat_scalar(const std::string& name, SH::ShValueType type, int size) const;
};

}

#endif /* GLSLVARIABLEMAP_HPP */
