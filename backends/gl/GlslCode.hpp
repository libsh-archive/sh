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
#ifndef GLSLCODE_HPP
#define GLSLCODE_HPP

#include "Glsl.hpp"
#include "GlBackend.hpp"
#include "ShVariableNode.hpp"
#include "ShProgram.hpp"
#include "ShCtrlGraph.hpp"
#include "GlslVariableMap.hpp"

namespace shgl {

class GlslCode : public SH::ShBackendCode {
public:
  GlslCode(const SH::ShProgramNodeCPtr& program, const std::string& target,
          TextureStrategy* texture);
  virtual ~GlslCode();

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var) { return false; }
  virtual void freeRegister(const SH::ShVariableNodePtr& var) {}

  virtual void upload();
  virtual void bind();
  virtual void update();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);

  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  
  /// Actually generate the code
  void generate();

private:
  TextureStrategy* m_texture;
  // NOTE: These two pointer are deliberately not smart pointers
  // so that the circular referenece between a program and
  // its compiled code is broken
  SH::ShProgramNode* m_shader; // internally visible shader ShTransformered to fit this target (GLSL)
  SH::ShProgramNode* m_originalShader; // original shader (should alway use this for external (e.g. globals))
  enum GlslProgramType m_unit;
  std::string m_target;

  static GLhandleARB m_arb_program; /// program to which all shaders are attached
  GLhandleARB m_arb_shader; /// shader program uploaded to the GPU

  std::vector<std::string> m_lines; /// raw lines of code (unindented)
  GlslVariableMap* m_varmap;

  bool m_uploaded; /// true if the program has already been uploaded to the GPU

  void optimize(const SH::ShProgramNodeCPtr& shader);

  /// Generate code for this node and those following it.
  void gen_node(SH::ShCtrlGraphNodePtr node);
  
  /// Generate code for a single Sh statement.
  void emit(const SH::ShStatement &stmt);
  
  std::string resolve(const SH::ShVariable& v, int src_size = 0) const;

  void print_infolog(GLhandleARB obj); /// Debug: print the OpenGL SL compiler log
  void print_shader_source(); /// Debug: retrieve and print shader source code from GPU

  void updateFloatUniform(const SH::ShVariableNodePtr& node, const GLint location);
  void updateIntUniform(const SH::ShVariableNodePtr& node, const GLint location);
};

typedef SH::ShPointer<GlslCode> GlslCodePtr;

}

#endif
