#ifndef ARBCODE_HPP
#define ARBCODE_HPP

#include "GlBackend.hpp"
#include "ShTransformer.hpp"
#include "ShVariableNode.hpp"
#include "ShVariable.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTextureNode.hpp"
#include "ShProgram.hpp"
#include "ShSwizzle.hpp"
#include "ShRefCount.hpp"
#include "ArbLimits.hpp"
#include "ArbReg.hpp"
#include "ArbInst.hpp"

namespace shgl {

class ArbCode;
class ArbBindingSpecs;
class ArbMapping;

// Filters for code emission and environment setup
const unsigned int SH_ARB_ANY   = 0x000; // All targets
const unsigned int SH_ARB_FP    = 0x001; // ARB_fragment_program
const unsigned int SH_ARB_VP    = 0x002; // ARB_vertex_program
const unsigned int SH_ARB_NVFP  = 0x004; // NV_fragment_program_option
const unsigned int SH_ARB_NVFP2 = 0x008; // NV_fragment_program2
const unsigned int SH_ARB_ATIDB = 0x010; // ATI_draw_buffers
const unsigned int SH_ARB_NVVP2 = 0x020; // NV_vertex_program2_option
const unsigned int SH_ARB_NVVP3 = 0x040; // NV_vertex_program3
const unsigned int SH_ARB_VEC1  = 0x080; // Maximum source has length 1
const unsigned int SH_ARB_VEC2  = 0x100; // Maximum source has length 2
const unsigned int SH_ARB_VEC3  = 0x200; // Maximum source has length 3
const unsigned int SH_ARB_VEC4  = 0x400; // Maximum source has length 4
const unsigned int SH_ARB_END   = 0x800; // Not a filter. End of
                                         // table.

class ArbCode : public SH::ShBackendCode {
public:
  ArbCode(const SH::ShProgramNodeCPtr& program, const std::string& target,
          TextureStrategy* textures);
  virtual ~ArbCode();

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
  
  /// Allocate registers, after the code has been generated
  void allocRegs();

  /// Allocate an input register, if necessary.
  void allocInputs(const ArbLimits& limits);
  
  /// Allocate an output register, if necessary.
  void allocOutputs(const ArbLimits& limits);
  
  /// Allocate a uniform register, if necessary.
  void allocParam(const ArbLimits& limits, const SH::ShVariableNodePtr& node);

  /// Allocate constants (called by allocRegs)
  void allocConsts(const ArbLimits& limits);

  /// Allocate temporary registers (called by allocRegs)
  void allocTemps(const ArbLimits& limits);

  /// Allocate textures (called by allocRegs)
  void allocTextures(const ArbLimits& limits);

  /// Make sure the textures are bound and sync'd for this code to run.
  void bindTextures();

  /// Make sure this texture is bound and sync'd for this code to run.
  void bindTexture(const SH::ShTextureNodePtr& node);
    
  void bindSpecial(const SH::ShProgramNode::VarList::const_iterator& begin,
                   const SH::ShProgramNode::VarList::const_iterator& end,
                   const ArbBindingSpecs& specs, 
                   std::vector<int>& bindings,
                   ArbRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::ShVariable& var,
                         bool collectingOp,
                         const SH::ShSwizzle& destSwiz) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const ArbInst& inst) const;
  
  TextureStrategy* m_textures;
  SH::ShProgramNodePtr m_shader; // internally visible shader ShTransformered to fit this target (ARB)
  SH::ShProgramNodeCPtr m_originalShader; // original shader (should alway use this for external (e.g. globals))
  std::string m_unit;

  typedef std::vector<ArbInst> ArbInstList;
  ArbInstList m_instructions;

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
                   SH::ShPointer<ArbReg> > RegMap;
  RegMap m_registers;

  typedef std::list< SH::ShPointer<ArbReg> > RegList;
  RegList m_reglist;

  std::vector<int> m_outputBindings;
  std::vector<int> m_inputBindings;

  /// The long tuple splits applied to this shader before compilation.
  SH::ShTransformer::VarSplitMap m_splits;

  /// ARB Program ID we are bound to. 0 if code hasn't been uploaded yet.
  unsigned int m_programId;

  static ArbMapping table[];

  // Extensions and language alternatives available. See list above
  unsigned int m_environment;
};

typedef SH::ShPointer<ArbCode> ArbCodePtr;


}

#endif
