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
#ifndef GLSLVARIABLE_HPP
#define GLSLVARIABLE_HPP

#include "ShVariableType.hpp"
#include "ShVariableNode.hpp"
#include <string>

namespace shgl {

class GlslVariable {
public:
  GlslVariable(const SH::ShVariableNodePtr& v); /// Usual constructor
  GlslVariable(); /// Default constructor (for STL container initialization)
  GlslVariable(const GlslVariable& v); /// Copy constructor
  
  std::string declaration() const;

  bool varying() const { return (m_kind == SH::SH_INPUT) || (m_kind == SH::SH_OUTPUT) || (m_kind == SH::SH_INOUT); }
  
  const std::string& name() const { return m_name; }
  const SH::ShSemanticType& semantic_type() const { return m_semantic_type; }

  void name(int i); /// for regular variables
  void name(const std::string& name); /// for built-in variables
  
private:
  bool m_builtin; /// if true, it won't be declared or initialized
  std::string m_name;
  int m_size;
  SH::ShBindingType m_kind;
  SH::ShValueType m_type;
  SH::ShSemanticType m_semantic_type;
  std::string m_values;
  
  std::string type_string() const;
};

}

#endif /* GLSLVARIABLE_HPP */
