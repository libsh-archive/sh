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

#include "Glsl.hpp"
#include "ShVariableType.hpp"
#include "ShVariableNode.hpp"
#include <string>

namespace shgl {

/// Possible bindings for an input/output variable
enum GlslVarBinding {
  // Output for vertex, input for fragment
  SH_GLSL_VAR_TEXCOORD,

  // Inputs to both units
  SH_GLSL_VAR_COLOR,
  SH_GLSL_VAR_SECONDARYCOLOR,

  // Vertex inputs
  SH_GLSL_VAR_VERTEX,
  SH_GLSL_VAR_NORMAL,
  SH_GLSL_VAR_MULTITEXCOORD0,
  SH_GLSL_VAR_MULTITEXCOORD1,
  SH_GLSL_VAR_MULTITEXCOORD2,
  SH_GLSL_VAR_MULTITEXCOORD3,
  SH_GLSL_VAR_MULTITEXCOORD4,
  SH_GLSL_VAR_MULTITEXCOORD5,
  SH_GLSL_VAR_MULTITEXCOORD6,
  SH_GLSL_VAR_MULTITEXCOORD7,
  
  // Fragment inputs
  SH_GLSL_VAR_FRAGCOORD,

  // Vertex outputs
  SH_GLSL_VAR_POSITION,
  SH_GLSL_VAR_FRONTCOLOR,
  SH_GLSL_VAR_FRONTSECONDARYCOLOR,

  // Fragment outputs
  SH_GLSL_VAR_FRAGDEPTH,
  SH_GLSL_VAR_FRAGCOLOR,

  SH_GLSL_VAR_NONE
};

struct GlslVarBindingInfo {
  char* name;
  int size;
  bool indexed;
};

class GlslVariable {
public:
  static const GlslVarBindingInfo glslVarBindingInfo[SH_GLSL_VAR_NONE];

  GlslVariable(const SH::ShVariableNodePtr& v); /// Usual constructor
  GlslVariable(); /// Default constructor (for STL container initialization)
  GlslVariable(const GlslVariable& v); /// Copy constructor
  
  std::string declaration() const;

  const std::string& name() const { return m_name; }
  const SH::ShSemanticType& semantic_type() const { return m_semantic_type; }
  const int size() const { return m_size; }
  const bool uniform() const { return m_uniform; }
  const bool texture() const { return m_texture; }

  void name(int i, enum GlslProgramType unit); /// for regular variables
  void builtin(GlslVarBinding binding, int index); /// for built-in variables
  
private:
  bool m_builtin; /// if true, it won't be declared or initialized
  bool m_texture;
  bool m_uniform;
  std::string m_name;
  union {
    int m_size;
    SH::ShTextureDims m_dims; // if m_texture == true
  };
  SH::ShBindingType m_kind;
  SH::ShValueType m_type;
  SH::ShSemanticType m_semantic_type;
  std::string m_values;
  
  std::string type_string() const;
};

}

#endif /* GLSLVARIABLE_HPP */
