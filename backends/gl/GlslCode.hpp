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

struct GlslOpCodeVecs
{
  GlslOpCodeVecs(const GlslMapping& mapping);

  GlslOpCodeVecs() {}
  bool operator<(const GlslOpCodeVecs &other) {
    return op < other.op;
  }

  SH::ShOperation op;
  std::vector<int> index;
  std::vector<std::string> frag;
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

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var) { return false; }
  virtual void freeRegister(const SH::ShVariableNodePtr& var) {}

  virtual void upload();
  virtual void bind();
  virtual void unbind();
  virtual void update();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);

  void set_bound(GLhandleARB program);
  void upload_uniforms();
  
  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  
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

  /// Add a line to the source code (m_lines)
  void append_line(const std::string& line, bool append_semicolon = true);

  /// Generate code for this node and those following it.
  void gen_structural_node(const SH::ShStructuralNodePtr& node);

  /// Generate code for a single Sh statement.
  void emit(const SH::ShStatement& stmt);
  void emit_cbrt(const SH::ShStatement& stmt);
  void emit_discard(const SH::ShStatement& stmt);
  void emit_exp(const SH::ShStatement& stmt, double power);
  void emit_lit(const SH::ShStatement& stmt);
  void emit_log(const SH::ShStatement& stmt, double base);
  void emit_logic(const SH::ShStatement& stmt);
  void emit_pal(const SH::ShStatement& stmt);
  void emit_prod(const SH::ShStatement& stmt);
  void emit_sum(const SH::ShStatement& stmt);
  void emit_texture(const SH::ShStatement& stmt);
  
  void table_substitution(const SH::ShStatement& stmt, GlslOpCodeVecs codeVecs);
  
  std::string resolve(const SH::ShVariable& v, int index = -1) const;
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