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

class ArbCode : public SH::ShBackendCode {
public:
  ArbCode(const SH::ShProgram& shader, const std::string& target,
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

  /// Generate code for DIV (either op2 is scalar or op1.size() == op2.size())
  void genDiv( const SH::ShVariable &dest, const SH::ShVariable &op1, 
      const SH::ShVariable &op2 );

  /// Generate code for a Dot Product 

  /// Generate code for a scalar/vector binary op where the scalar
  /// should be promoted to a vector by duplicating components
  void genScalarVectorInst( const SH::ShVariable &dest, const SH::ShVariable &op1, 
      const SH::ShVariable &op2, int opcode );
 
  /// Generate code for polynomial approximations of trig functions.
  void genTrigInst( const SH::ShVariable &dest, const SH::ShVariable& src,
      int opcode );

  void genDot( const SH::ShVariable &dest, const SH::ShVariable& src0,
      const SH::ShVariable& src1);

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
  SH::ShProgram m_shader; // internally visible shader ShTransformered to fit this target (ARB)
  SH::ShProgram m_originalShader; // original shader (should alway use this for external (e.g. globals))
  std::string m_target;

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

  typedef std::map<SH::ShVariableNodePtr, ArbReg> RegMap;
  RegMap m_registers;

  std::vector<int> m_outputBindings;
  std::vector<int> m_inputBindings;

  /// The long tuple splits applied to this shader before compilation.
  SH::ShTransformer::VarSplitMap m_splits;

  /// ARB Program ID we are bound to. 0 if code hasn't been uploaded yet.
  unsigned int m_programId;
};

typedef SH::ShPointer<ArbCode> ArbCodePtr;


}

#endif
