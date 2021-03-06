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
#ifndef SHARBCODE_HPP
#define SHARBCODE_HPP

#include "GlBackend.hpp"
#include "Transformer.hpp"
#include "VariableNode.hpp"
#include "Variable.hpp"
#include "CtrlGraph.hpp"
#include "TextureNode.hpp"
#include "Program.hpp"
#include "Swizzle.hpp"
#include "RefCount.hpp"
#include "Structural.hpp"
#include "ArbLimits.hpp"
#include "ArbReg.hpp"
#include "ArbInst.hpp"

#include <map>
#include <set>

namespace shgl {

class ArbCode;
class ArbBindingSpecs;
class ArbMapping;

// Filters for code emission and environment setup
const unsigned int ARB_ANY   = 0x0000; // All targets
const unsigned int ARB_FP    = 0x0001; // ARB_fragment_program
const unsigned int ARB_VP    = 0x0002; // ARB_vertex_program
const unsigned int ARB_NVFP  = 0x0004; // NV_fragment_program_option
const unsigned int ARB_NVFP2 = 0x0008; // NV_fragment_program2
const unsigned int ARB_ATIDB = 0x0010; // ATI_draw_buffers option
const unsigned int ARB_NVVP2 = 0x0020; // NV_vertex_program2_option
const unsigned int ARB_NVVP3 = 0x0040; // NV_vertex_program3
const unsigned int ARB_VEC1  = 0x0080; // Maximum source has length 1
const unsigned int ARB_VEC2  = 0x0100; // Maximum source has length 2
const unsigned int ARB_VEC3  = 0x0200; // Maximum source has length 3
const unsigned int ARB_VEC4  = 0x0400; // Maximum source has length 4
const unsigned int ARB_ARBDB = 0x0800; // ATI_draw_buffers option
const unsigned int ARB_END   = 0x1000; // Not a filter. End of
                                         // table.

class ArbCode : public SH::BackendCode {
public:
  ArbCode(const SH::ProgramNodeCPtr& program, const std::string& target,
          TextureStrategy* texture);
  virtual ~ArbCode();

  virtual bool allocateRegister(const SH::VariableNodePtr& var);
  virtual void freeRegister(const SH::VariableNodePtr& var);
  
  virtual void upload();
  virtual void bind();
  virtual void unbind();
  virtual void update();
  virtual void updateUniform(const SH::VariableNodePtr& uniform);
  
  std::ostream& print(std::ostream& out);
  std::ostream& describe_interface(std::ostream& out);
  std::ostream& describe_bindings(std::ostream& out);

  /// Actually generate the code, and do register allocation.
  void generate();
  
private:

  /// Generate code for this node and those following it.
  void genNode(SH::CtrlGraphNode* node);

  /// Generate code for this structural node and those contained by
  /// it.
  /// Right now this is geared specifically at NV_fragment_program2.
  void genStructNode(const SH::StructuralNodePtr& node);

  /// Generate code for a conditional break
  void push_break(SH::Variable& cond, bool negate);

  /// Generate code for an opening if statement
  void push_if(SH::Variable& cond, bool negate);

  /// Generate code for a single Sh statement.
  void emit(const SH::Statement& stmt);

  /// Special code cases
  void emit_div(const SH::Statement& stmt);
  void emit_sqrt(const SH::Statement& stmt);
  void emit_lerp(const SH::Statement& stmt);
  void emit_dot2(const SH::Statement& stmt);
  void emit_eq(const SH::Statement& stmt);
  void emit_ceil(const SH::Statement& stmt);
  void emit_mod(const SH::Statement& stmt);
  void emit_trig(const SH::Statement& stmt);
  void emit_invtrig(const SH::Statement& stmt);
  void emit_atan(const SH::Statement& stmt);
  void emit_tan(const SH::Statement& stmt);
  void emit_exp(const SH::Statement& stmt);
  void emit_hyperbolic(const SH::Statement& stmt);
  void emit_log(const SH::Statement& stmt);
  void emit_norm(const SH::Statement& stmt);
  void emit_sgn(const SH::Statement& stmt);
  void emit_tex(const SH::Statement& stmt);
  void emit_nvcond(const SH::Statement& stmt);
  void emit_cmul(const SH::Statement& stmt);
  void emit_csum(const SH::Statement& stmt);
  void emit_kil(const SH::Statement& stmt);
  void emit_pal(const SH::Statement& stmt);
  void emit_lit(const SH::Statement& stmt);
  void emit_rnd(const SH::Statement& stmt);
  void emit_cbrt(const SH::Statement& stmt);
  void emit_ret(const SH::Statement& stmt);
  void emit_comment(const SH::Statement& stmt);

  /// Allocate registers, after the code has been generated
  void allocRegs();

  /// Allocate an input register, if necessary.
  /// Calls the relivent generic or semantic input alloc function.
  void allocInputs(const ArbLimits& limits);

  /// Allocate inputs with "generic" bindings.
  /// Only relivent to vertex shaders for this backend.
  void allocGenericInputs(const ArbLimits& limits);

  /// Allocate inputs with "semantic" bindings.
  /// Only relivent to vertex shaders for this backend.
  void allocSemanticInputs(const ArbLimits& limits);
  
  /// Allocate an output register, if necessary.
  void allocOutputs(const ArbLimits& limits);
  
  /// Allocate a uniform register, if necessary.
  void allocParam(const ArbLimits& limits, const SH::VariableNodePtr& node);

  /// Allocate a palette of uniform registers, if necessary.
  void allocPalette(const ArbLimits& limits, const SH::PaletteNodePtr& node);

  /// Allocate constants (called by allocRegs)
  void allocConsts(const ArbLimits& limits);

  /// Allocate temporary registers (called by allocRegs)
  /// half indicates whether to allocate the SH_HALF or the SH_FLOAT temporaries 
  /// on this pass
  void allocTemps(const ArbLimits& limits, bool half);

  /// Allocate textures (called by allocRegs)
  void allocTextures(const ArbLimits& limits);

  /// Make sure the textures are bound and sync'd for this code to run.
  void bindTextures();

  /// Make sure this texture is bound and sync'd for this code to run.
  void bindTexture(const SH::TextureNodePtr& node);
    
  void bindSpecial(const SH::ProgramNode::VarList::const_iterator& begin,
                   const SH::ProgramNode::VarList::const_iterator& end,
                   bool is_output, std::map<ArbRegBinding, std::set<int> >& bindings,
                   ArbRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::Variable& var,
                         bool collectingOp,
                         const SH::Swizzle& destSwiz,
                         bool do_swiz) const;

  std::ostream& print_indent(std::ostream& out) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const ArbInst& inst) const;

  int getLabel(SH::CtrlGraphNode* node);
  
  TextureStrategy* m_texture;
  // NOTE: These two pointer are deliberately not smart pointers
  // so that the circular referenece between a program and
  // its compiled code is broken
  SH::ProgramNode* m_shader; // internally visible shader Transformered to fit this target (ARB)
  SH::ProgramNode* m_originalShader; // original shader (should alway use this for external (e.g. globals))
  std::string m_unit;

  typedef std::vector<ArbInst> ArbInstList;
  ArbInstList m_instructions;

  /// A list of temporary register indices which are available.
  std::list<int> m_tempRegs;

  /// The number of temporary registers used in this shader.
  int m_numTemps;

  /// The number of half-float temporary registers used in this shader.
  int m_numHalfTemps;

  /// The number of input registers used in this shader.
  int m_numInputs;

  /// The number of output registers used in this shader.
  int m_numOutputs;

  /// The number of parameter registers used in this shader.
  int m_numParams;

  /// The number of parameter bindings used in this shader (can be
  /// shared by multiple parameters).
  int m_numParamBindings;

  /// The number of constant 4-tuples used in this shader.
  int m_numConsts;

  /// The number of distinct textures used in this shader.
  int m_numTextures;

  typedef std::map<SH::VariableNodePtr,
                   SH::Pointer<ArbReg> > RegMap;
  RegMap m_registers;

  typedef std::list< SH::Pointer<ArbReg> > RegList;
  RegList m_reglist;

  std::map<ArbRegBinding, std::set<int> > m_outputBindings;
  std::map<ArbRegBinding, std::set<int> > m_inputBindings;

  /// Map any new variables that we create back to their originals in
  /// m_originalShader.
  SH::VarTransformMap m_originalVarsMap;

  //@todo remove m_splits and m_converts with dependent uniforms 
  
  /// The long tuple splits applied to this shader before compilation.
  SH::Transformer::VarSplitMap m_splits;

  /// The conversions done to change types not handled in hardware into
  // floating point types
  //
  // @todo may want more intelligent conversion if hardware 
  SH::Transformer::ValueTypeMap m_convertMap;

  /// ARB Program ID we are bound to. 0 if code hasn't been uploaded yet.
  GLuint m_programId;

  static ArbMapping table[];

  // Extensions and language alternatives available. See list above
  unsigned int m_environment;

  typedef std::map<SH::CtrlGraphNode*, int> LabelMap;
  LabelMap m_label_map; 
  int m_max_label;

  // For array lookup
  SH::Variable m_address_register;

  unsigned int m_indent;
};

typedef SH::Pointer<ArbCode> ArbCodePtr;


}

#endif
