#ifndef SHARB_HPP
#define SHARB_HPP

#include <string>
#include <vector>
#include <list>
#include <set>
#include "ShBackend.hpp"
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"

namespace ShArb {

class ArbBackend;
class ArbInst;
class ArbReg;
struct ArbBindingSpecs;

/** Possible register types in the ARB spec.
 */
enum ShArbRegType {
  SH_ARB_REG_ATTRIB,
  SH_ARB_REG_PARAM,
  SH_ARB_REG_TEMP,
  SH_ARB_REG_ADDRESS,
  SH_ARB_REG_OUTPUT,
  SH_ARB_REG_CONST,
  SH_ARB_REG_TEXTURE
};

class ArbCode : public SH::ShBackendCode {
public:
  ArbCode(SH::ShRefCount<ArbBackend> backend, const SH::ShShader& shader);
  virtual ~ArbCode();

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var);
  virtual void freeRegister(const SH::ShVariableNodePtr& var);
  
  virtual void upload();
  virtual void bind();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);
  
  std::ostream& print(std::ostream& out);

  /// Actually generate the code, and do register allocation.
  void generate();
  
private:

  /// Generate code for this node and those following it.
  void genNode(SH::ShCtrlGraphNodePtr node);

  /// Allocate registers, after the code has been generated
  void allocRegs();

  /// Allocate an input register, if necessary.
  void allocInputs();
  
  /// Allocate an output register, if necessary.
  void allocOutputs();
  
  /// Allocate a uniform register, if necessary.
  void allocParam(SH::ShVariableNodePtr node);

  /// Allocate constants (called by allocRegs)
  void allocConsts();

  /// Allocate temporary registers (called by allocRegs)
  void allocTemps();

  /// Allocate textures (called by allocRegs)
  void allocTextures();

  void bindSpecial(const SH::ShShaderNode::VarList::const_iterator& begin,
                   const SH::ShShaderNode::VarList::const_iterator& end,
                   const ArbBindingSpecs& specs, 
                   std::vector<int>& bindings,
                   ShArbRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::ShVariable& var) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const ArbInst& inst) const;
  
  SH::ShRefCount<ArbBackend> m_backend;
  SH::ShShader m_shader;

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
};

typedef SH::ShRefCount<ArbCode> ArbCodePtr;

class ArbBackend : public SH::ShBackend {
public:
  ArbBackend();
  ~ArbBackend();
  
  std::string name() const;

  SH::ShBackendCodePtr generateCode(const SH::ShShader& shader);

  int instrs(int kind) { return m_instrs[kind]; }
  int temps(int kind) { return m_temps[kind]; }
  int attribs(int kind) { return m_attribs[kind]; }
  int params(int kind) { return m_params[kind]; }
  int texs(int kind) { return m_texs[kind]; }
  
private:
  int m_instrs[2]; ///< Maximum number of instructions for each shader kind
  int m_temps[2]; ///< Maximum number of temporaries for each shader kind
  int m_attribs[2]; ///<Maximum number of attributes for each shader kind
  int m_params[2]; ///< Maximum number of parameters for each shader kind
  int m_texs[2]; ///< Maximum number of TEX instructions for each shader kind
};

typedef SH::ShRefCount<ArbBackend> ArbBackendPtr;

}

#endif
