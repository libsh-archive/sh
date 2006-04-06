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
#ifndef GLSLCODE_HPP
#define GLSLCODE_HPP

#include <map>
#include <string>
#include <vector>

#include "Glsl.hpp"
#include "GlBackend.hpp"
#include "ShVariableNode.hpp"
#include "ShProgram.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTransformer.hpp"
#include "GlslVariableMap.hpp"
#include "GlslSet.hpp"

namespace shgl {

struct TextureInfo {
  int index;
  bool preset;
};

struct GlslMapping {
  SH::ShOperation op;
  const char* code;
};

struct GlslLine {
  int indent;
  std::string code;
};

class GlslCode : public SH::ShBackendCode {
public:
  GlslCode(const SH::ShProgramNodeCPtr& program, const std::string& target,
          TextureStrategy* texture);
  virtual ~GlslCode();

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var) { return true; }
  virtual void freeRegister(const SH::ShVariableNodePtr& var) {}

  virtual void upload();
  virtual void bind();
  virtual void unbind();
  virtual void update();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);

  void set_bound(GLhandleARB program);
  void upload_uniforms();
  void bind_generic_attributes(GLhandleARB glsl_program);
  
  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  std::ostream& describe_bindings(std::ostream& out);
  
  /// Actually generate the code
  void generate();

  GlslProgramType glsl_unit() const { return m_unit; }
  GLhandleARB glsl_shader();
  std::string target() const { return m_target; }

  void bind_textures();
  
private:
  static GlslSet* m_fallback_set;
  
  TextureStrategy* m_texture;
  // NOTE: These two pointer are deliberately not smart pointers
  // so that the circular referenece between a program and
  // its compiled code is broken
  SH::ShProgramNode* m_shader; // internally visible shader ShTransformered to fit this target (GLSL)
  SH::ShProgramNode* m_originalShader; // original shader (should alway use this for external (e.g. globals))
  GlslProgramType m_unit;
  std::string m_target;

  GLhandleARB m_arb_shader; /// shader program uploaded to the GPU

  std::vector<GlslLine> m_lines; /// raw lines of code (unindented)
  int m_indent;
  GlslVariableMap* m_varmap;
  std::map<SH::ShTextureNodePtr, TextureInfo> m_texture_units;
  int m_nb_textures;

  /// The long tuple splits applied to this shader before compilation.
  SH::ShTransformer::VarSplitMap m_splits;

  // Linked GLSL Program that contains this shader and is currently
  // bound, if any
  GLhandleARB m_bound;

  // Data about the current GLSL target vendor
  enum Vendor {
    VENDOR_ATI = 0,
    VENDOR_NVIDIA,
    VENDOR_OTHER,
    VENDOR_UNKNOWN
  } m_vendor;

  /// Add a line to the source code (m_lines)
  void append_line(const std::string& line, bool append_semicolon = true);

  /// Print the declaration of a glsl variable along with its Sh name
  std::string print_decl(const GlslVariableDeclaration& decl);

  /// Generate code for this node and those following it.
  void gen_structural_node(const SH::ShStructuralNodePtr& node);

  /// Generate code for a single Sh statement.
  void emit(const SH::ShStatement& stmt);
  void emit_ati_workaround(const SH::ShStatement& stmt, double real_answer, const char* code);
  void emit_cbrt(const SH::ShStatement& stmt);
  void emit_comment(const SH::ShStatement& stmt);
  void emit_cond(const SH::ShStatement& stmt);
  void emit_discard(const SH::ShStatement& stmt, const std::string& function);
  void emit_exp10(const SH::ShStatement& stmt);
  void emit_hyperbolic(const SH::ShStatement& stmt);
  void emit_lit(const SH::ShStatement& stmt);
  void emit_log(const SH::ShStatement& stmt);
  void emit_log10(const SH::ShStatement& stmt);
  void emit_logic(const SH::ShStatement& stmt);
  void emit_noise(const SH::ShStatement& stmt);
  void emit_pal(const SH::ShStatement& stmt);
  void emit_pow(const SH::ShVariable& dest, const SH::ShVariable& a, const SH::ShVariable& b);
  void emit_prod(const SH::ShStatement& stmt);
  void emit_sum(const SH::ShStatement& stmt);
  void emit_texture(const SH::ShStatement& stmt);
  
  void table_substitution(const SH::ShStatement& stmt, const char* code_ptr);
  bool handle_table_operation(const SH::ShStatement& stmt);
  
  std::string resolve(const SH::ShVariable& v, int index = -1, int size = 0) const;
  std::string resolve(const SH::ShVariable& v, const SH::ShVariable& index) const;
  std::string resolve_constant(double constant, const SH::ShVariable& var, int size = 0) const;

  void update_float_uniform(const SH::ShVariableNodePtr& node, const GLint location);
  void update_int_uniform(const SH::ShVariableNodePtr& node, const GLint location);
  void real_update_uniform(const SH::ShVariableNodePtr& uniform, const std::string& name);

  SH::ShVariableNodePtr allocate_constant(const SH::ShStatement& stmt, double constant, int size = 0) const;
  SH::ShVariableNodePtr allocate_temp(const SH::ShStatement& stmt, int size = 0) const;
  void allocate_textures();
};

typedef SH::ShPointer<GlslCode> GlslCodePtr;

}

#endif
