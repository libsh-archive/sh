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
#ifndef SHARB_HPP
#define SHARB_HPP

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include "ShBackend.hpp"
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTextureNode.hpp"

namespace ShArb {

class ArbBackend;
struct ArbInst;
struct ArbReg;
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
  ArbCode(SH::ShRefCount<ArbBackend> backend, const SH::ShProgram& shader,
          const std::string& target);
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

  /// Generate code for DIV (either op2 is scalar or op1.size() == op2.size())
  void genDiv( const SH::ShVariable &dest, const SH::ShVariable &op1, 
      const SH::ShVariable &op2 );

  /// Generate code for a scalar/vector binary op where the scalar
  // should be promoted to a vector by duplicating components
  void genScalarVectorInst( const SH::ShVariable &dest, const SH::ShVariable &op1, 
      const SH::ShVariable &op2, int opcode );

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

  /// Allocate a texture, either data or cubemap
  void loadTexture(SH::ShTextureNodePtr texture);
  
  /// Allocate a data texture
  void loadDataTexture(SH::ShDataTextureNodePtr texture, unsigned int type = 0);

  /// Allocate a cube map
  void loadCubeTexture(SH::ShCubeTextureNodePtr cube);
  
  void bindSpecial(const SH::ShProgramNode::VarList::const_iterator& begin,
                   const SH::ShProgramNode::VarList::const_iterator& end,
                   const ArbBindingSpecs& specs, 
                   std::vector<int>& bindings,
                   ShArbRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::ShVariable& var,
                         bool collectingOp,
                         const SH::ShSwizzle& destSwiz) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const ArbInst& inst) const;
  
  SH::ShRefCount<ArbBackend> m_backend;
  SH::ShProgram m_shader;
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

  /// ARB Program ID we are bound to. 0 if code hasn't been uploaded yet.
  unsigned int m_programId;
};

typedef SH::ShRefCount<ArbCode> ArbCodePtr;

class ArbBackend : public SH::ShBackend {
public:
  ArbBackend();
  ~ArbBackend();
  
  std::string name() const;

  SH::ShBackendCodePtr generateCode(const std::string& kind, const SH::ShProgram& shader);

  int instrs(const std::string& target) { return m_instrs[target]; }
  int temps(const std::string& target) { return m_temps[target]; }
  int attribs(const std::string& target) { return m_attribs[target]; }
  int params(const std::string& target) { return m_params[target]; }
  int texs(const std::string& target) { return m_texs[target]; }

  void execute(const SH::ShProgram& program, SH::ShStream& dest);
  
private:
  class GlExtensions {
  public:
    GlExtensions();
    
    bool have(const std::string& name);

    void dump();
  private:
    std::set<std::string> m_extlist;
  };

  std::map<std::string, int> m_instrs; ///< Maximum number of instructions for each shader target
  std::map<std::string, int> m_temps; ///< Maximum number of temporaries for each shader target
  std::map<std::string, int> m_attribs; ///<Maximum number of attributes for each shader target
  std::map<std::string, int> m_params; ///< Maximum number of parameters for each shader target
  std::map<std::string, int> m_texs; ///< Maximum number of TEX instructions for each shader target
};

typedef SH::ShRefCount<ArbBackend> ArbBackendPtr;


/** Possible bindings for a register (see ARB spec).
 */
enum ShArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  SH_ARB_REG_PARAMLOC,
  SH_ARB_REG_PARAMENV,
  // Output
  SH_ARB_REG_RESULTCOL,

  // VERTEX
  // Input
  SH_ARB_REG_VERTEXPOS,
  SH_ARB_REG_VERTEXWGT,
  SH_ARB_REG_VERTEXNRM,
  SH_ARB_REG_VERTEXCOL,
  SH_ARB_REG_VERTEXFOG,
  SH_ARB_REG_VERTEXTEX,
  SH_ARB_REG_VERTEXMAT,
  SH_ARB_REG_VERTEXATR,
  // Output
  SH_ARB_REG_RESULTPOS,
  SH_ARB_REG_RESULTFOG,
  SH_ARB_REG_RESULTPTS, ///< Result point size
  SH_ARB_REG_RESULTTEX,

  // FRAGMENT
  // Input
  SH_ARB_REG_FRAGMENTCOL,
  SH_ARB_REG_FRAGMENTTEX,
  SH_ARB_REG_FRAGMENTFOG,
  SH_ARB_REG_FRAGMENTPOS,
  // Output
  SH_ARB_REG_RESULTDPT,

  SH_ARB_REG_NONE
};

/** An ARB register.
 */
struct ArbReg {
  ArbReg();
  ArbReg(ShArbRegType type, int index);

  ShArbRegType type;
  int index;

  union {
    struct {
      ShArbRegBinding binding;
      int bindingIndex;
    };
    float values[4];
  };

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);
  
  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const;
};

/** All the possible operations in the ARB spec.
 */
enum ArbOp {
  // VERTEX AND FRAGMENT
  
  // Vector
  SH_ARB_ABS,
  SH_ARB_FLR,
  SH_ARB_FRC,
  SH_ARB_LIT,
  SH_ARB_MOV,

  // Scalar
  SH_ARB_EX2,
  SH_ARB_LG2,
  SH_ARB_RCP,
  SH_ARB_RSQ,

  // Binary scalar
  SH_ARB_POW,

  // Binary vector
  SH_ARB_ADD,
  SH_ARB_DP3,
  SH_ARB_DP4,
  SH_ARB_DPH,
  SH_ARB_DST,
  SH_ARB_MAX,
  SH_ARB_MIN,
  SH_ARB_MUL,
  SH_ARB_SGE,
  SH_ARB_SLT,
  SH_ARB_SUB,
  SH_ARB_XPD,

  // Trinary
  SH_ARB_MAD,

  // Swizzling
  SH_ARB_SWZ,

  // VERTEX ONLY
  // Scalar
  SH_ARB_EXP,
  SH_ARB_LOG,
  
  // FRAGMENT ONLY
  // Scalar
  SH_ARB_COS,
  SH_ARB_SIN,
  SH_ARB_SCS,

  // Trinary
  SH_ARB_CMP,
  SH_ARB_LRP,

  // Sampling
  SH_ARB_TEX,
  SH_ARB_TXP,
  SH_ARB_TXB,

  // KIL
  SH_ARB_KIL
};

/** An ARB instruction.
 */
struct ArbInst {
  ArbInst(ArbOp op, const SH::ShVariable& dest)
    : op(op), dest(dest)
  {
  }

  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0)
    : op(op), dest(dest)
  {
    src[0] = src0;
  }

  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
  }
  ArbInst(ArbOp op, const SH::ShVariable& dest, const SH::ShVariable& src0,
          const SH::ShVariable& src1, const SH::ShVariable& src2)
    : op(op), dest(dest)
  {
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;
  }
  
  ArbOp op;
  SH::ShVariable dest;
  SH::ShVariable src[3];
};


}

#endif
