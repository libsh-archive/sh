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
#ifndef GLSLVARIABLEMAP_HPP
#define GLSLVARIABLEMAP_HPP

#include "Glsl.hpp"
#include "ShVariableNode.hpp"
#include "ShProgramNode.hpp"
#include "GlslVariable.hpp"
#include <map>
#include <string>
#include <list>

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
  
  std::string resolve(const SH::ShVariable& v, int src_size);

  typedef std::list<std::string> DeclarationList;

  DeclarationList::const_iterator begin() const;
  DeclarationList::const_iterator end() const;
  
private:
  SH::ShProgramNode* m_shader;
  GlslProgramType m_unit;

  unsigned m_nb_variables;
  std::map<SH::ShVariableNodePtr, GlslVariable> m_varmap; /// maps a variable node to a variable
  
  DeclarationList m_declarations;

  std::map<GlslVarBinding, int> m_input_bindings;
  std::map<GlslVarBinding, int> m_output_bindings;

  void allocate_builtin(const SH::ShVariableNodePtr& node,
                        const GlslBindingSpecs* specs, std::map<GlslVarBinding, int>& bindings,
                        bool generic);
  void allocate_builtin_inputs();
  void allocate_builtin_outputs();
  
  void allocate_input(const SH::ShVariableNodePtr& node);
  void allocate_output(const SH::ShVariableNodePtr& node);
  void allocate_temp(const SH::ShVariableNodePtr& node);

  std::string swizzle(const SH::ShVariable& v, int dest_size, int src_size) const;
};

}

#endif /* GLSLVARIABLEMAP_HPP */
