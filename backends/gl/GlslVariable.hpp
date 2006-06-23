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
  SH_GLSL_VAR_FRAGDATA,

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
  const int attribute() const { return m_attribute; }
  const SH::ShSemanticType& semantic_type() const { return m_semantic_type; }
  const int size() const { return m_size; }
  const bool builtin() const { return m_builtin; }
  const bool uniform() const { return m_uniform; }
  const bool texture() const { return m_texture; }

  void name(int i, enum GlslProgramType unit); /// for regular variables
  void attribute(int index); /// for generic vertex inputs
  void builtin(GlslVarBinding binding, int index); /// for built-in variables
  
  // ARB to GLSL state translation tables
  struct ArbToGlslEntry
  {
    char *from;
    char *to;
    bool to_is_array;
    const ArbToGlslEntry *subtable;
  };

private:
  int m_attribute; /// index of the attribute (-1 if it's not a generic attribute)
  bool m_builtin; /// if true, it won't be declared or initialized
  bool m_texture;
  bool m_palette;
  bool m_uniform;

  std::string m_name;

  int m_size;
  bool m_texture_shadow; /// if m_texture is true, tells whether the texture is a shadow texture
  SH::ShTextureDims m_dims; // if m_texture == true
  std::size_t m_length; // if m_palette == true

  SH::ShBindingType m_kind;
  SH::ShValueType m_type;
  SH::ShSemanticType m_semantic_type;

  std::string m_values;
  
  std::string type_string() const;

  /// Translates an arb state value to a glsl state value.
  /// Returns the original string if something fails.
  static std::string translate_state_arb_to_glsl
    (const std::string &value, const ArbToGlslEntry *table);

  /// Handy utilities for above
  static const ArbToGlslEntry * find
    (const std::string &value, const ArbToGlslEntry *table);
  
  /// Returns an empty string if no array operator was found
  static std::string parse_state_array
    (char &cur_delim, std::string &cur_string);
  
};

}

#endif /* GLSLVARIABLE_HPP */
