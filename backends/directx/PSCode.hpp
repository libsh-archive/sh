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
#ifndef PSCODE_HPP
#define PSCODE_HPP

#include "DxBackend.hpp"
#include "ShTransformer.hpp"
#include "ShVariableNode.hpp"
#include "ShVariable.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTextureNode.hpp"
#include "ShProgram.hpp"
#include "ShSwizzle.hpp"
#include "ShRefCount.hpp"
#include "ShStructural.hpp"
#include "PSLimits.hpp"
#include "PSReg.hpp"
#include "PSInst.hpp"

namespace shdx {

class PSCode;
class PSBindingSpecs;
class PSMapping;

// Filters for code emission and environment setup
const unsigned int SH_PS_ANY    = 0x0000; // All targets
const unsigned int SH_PS_VS_1_1 = 0x0001; // Version 1.1 Vertex shaders
const unsigned int SH_PS_VS_2_0 = 0x0002; // Version 2.0 Vertex shaders
const unsigned int SH_PS_VS_3_0 = 0x0004; // Version 3.0 Vertex shaders
const unsigned int SH_PS_PS_1_1 = 0x0008; // Version 1.1 Pixel shaders
const unsigned int SH_PS_PS_1_2 = 0x0010; // Version 1.2 Pixel shaders
const unsigned int SH_PS_PS_1_3 = 0x0020; // Version 1.3 Pixel shaders
const unsigned int SH_PS_PS_1_4 = 0x0040; // Version 1.4 Pixel shaders
const unsigned int SH_PS_PS_2_0 = 0x0080; // Version 2.0 Pixel shaders
const unsigned int SH_PS_PS_3_0 = 0x0100; // Version 3.0 Pixel shaders
const unsigned int SH_PS_VEC1   = 0x0200; // Maximum source has length 1
const unsigned int SH_PS_VEC2   = 0x0400; // Maximum source has length 2
const unsigned int SH_PS_VEC3   = 0x0800; // Maximum source has length 3
const unsigned int SH_PS_VEC4   = 0x1000; // Maximum source has length 4
const unsigned int SH_PS_END    = 0x2000; // Not a filter. End of table.

class PSCode : public SH::ShBackendCode {
public:
  PSCode(const SH::ShProgramNodeCPtr& program, const std::string& target,
          TextureStrategy* textures);
  virtual ~PSCode();

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var);
  virtual void freeRegister(const SH::ShVariableNodePtr& var);
  
  virtual void upload();
  virtual void bind();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);
  
  std::ostream& print(std::ostream& out);
  std::ostream& printInputOutputFormat(std::ostream& out);

  /// Actually generate the code, and do register allocation.
  void generate();
  
private:

  /// Generate code for this node and those following it.
  void genNode(SH::ShCtrlGraphNodePtr node);

  /// Generate code for this structural node and those contained by
  /// it.
  /// Right now this is geared specifically at NV_fragment_program2.
  void genStructNode(const SH::ShStructuralNodePtr& node);

  /// Generate code for a single Sh statement.
  void emit(const SH::ShStatement& stmt);

  /// Special code cases
  void emit_div(const SH::ShStatement& stmt);
  void emit_sqrt(const SH::ShStatement& stmt);
  void emit_lerp(const SH::ShStatement& stmt);
  void emit_dot2(const SH::ShStatement& stmt);
  void emit_eq(const SH::ShStatement& stmt);
  void emit_ceil(const SH::ShStatement& stmt);
  void emit_mod(const SH::ShStatement& stmt);
  void emit_trig(const SH::ShStatement& stmt);
  void emit_invtrig(const SH::ShStatement& stmt);
  void emit_tan(const SH::ShStatement& stmt);
  void emit_exp(const SH::ShStatement& stmt);
  void emit_log(const SH::ShStatement& stmt);
  void emit_norm(const SH::ShStatement& stmt);
  void emit_sgn(const SH::ShStatement& stmt);
  void emit_tex(const SH::ShStatement& stmt);
  void emit_nvcond(const SH::ShStatement& stmt);
  void emit_cmul(const SH::ShStatement& stmt);
  void emit_csum(const SH::ShStatement& stmt);
  void emit_kil(const SH::ShStatement& stmt);
  
  /// Allocate registers, after the code has been generated
  void allocRegs();

  /// Allocate an input register, if necessary.
  void allocInputs(const PSLimits& limits);
  
  /// Allocate an output register, if necessary.
  void allocOutputs(const PSLimits& limits);
  
  /// Allocate a uniform register, if necessary.
  void allocParam(const PSLimits& limits, const SH::ShVariableNodePtr& node);

  /// Allocate constants (called by allocRegs)
  void allocConsts(const PSLimits& limits);

  /// Allocate temporary registers (called by allocRegs)
  void allocTemps(const PSLimits& limits);

  /// Allocate textures (called by allocRegs)
  void allocTextures(const PSLimits& limits);

  /// Make sure the textures are bound and sync'd for this code to run.
  void bindTextures();

  /// Make sure this texture is bound and sync'd for this code to run.
  void bindTexture(const SH::ShTextureNodePtr& node);
    
  void bindSpecial(const SH::ShProgramNode::VarList::const_iterator& begin,
                   const SH::ShProgramNode::VarList::const_iterator& end,
                   const PSBindingSpecs& specs, 
                   std::vector<int>& bindings,
                   PSRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::ShVariable& var,
                         bool collectingOp,
                         const SH::ShSwizzle& destSwiz) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const PSInst& inst) const;

  int getLabel(SH::ShCtrlGraphNodePtr node);
  
  TextureStrategy* m_textures;
  SH::ShProgramNodePtr m_shader; // internally visible shader ShTransformered to fit this target (PS)
  SH::ShProgramNodeCPtr m_originalShader; // original shader (should alway use this for external (e.g. globals))
  std::string m_unit;

  typedef std::vector<PSInst> PSInstList;
  PSInstList m_instructions;

  /// A list of temporary register indices which are available.
  std::list<int> m_tempRegs;

  /// The number of temporary registers used in this shader.
  int m_numTemps;

  /// The number of input registers used in this shader.
  int m_numInputs;

  /// The number of output registers used in this shader.
  int m_numOutputs;

  /// The number of parameter registers used in this shader.
  int m_numParams;

  /// The number of constant 4-tuples used in this shader.
  int m_numConsts;

  /// The number of distinct textures used in this shader.
  int m_numTextures;

  typedef std::map<SH::ShVariableNodePtr,
                   SH::ShPointer<PSReg> > RegMap;
  RegMap m_registers;

  typedef std::list< SH::ShPointer<PSReg> > RegList;
  RegList m_reglist;

  std::vector<int> m_outputBindings;
  std::vector<int> m_inputBindings;

  /// The long tuple splits applied to this shader before compilation.
  SH::ShTransformer::VarSplitMap m_splits;

  /// PS Program ID we are bound to. 0 if code hasn't been uploaded yet.
  unsigned int m_programId;

  static PSMapping table[];

  // Extensions and language alternatives available. See list above
  unsigned int m_environment;

  typedef std::map<SH::ShCtrlGraphNodePtr, int> LabelMap;
  LabelMap m_label_map; 
  int m_max_label;
};

typedef SH::ShPointer<PSCode> PSCodePtr;


}

#endif
