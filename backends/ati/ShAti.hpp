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
#ifndef SHATI_HPP
#define SHATI_HPP

#include <string>
#include <vector>
#include <list>
#include <set>
#include "ShBackend.hpp"
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTextureNode.hpp"
#include "ShFramebuffer.hpp"
#include "ShUberbuffer.hpp"
#include "ShArrayData.hpp"

namespace ShAti {

class AtiBackend;
struct AtiInst;
struct AtiReg;
struct AtiBindingSpecs;

/** Possible register types in the ATI spec.
 */
enum ShAtiRegType {
  SH_ATI_REG_ATTRIB,
  SH_ATI_REG_PARAM,
  SH_ATI_REG_TEMP,
  SH_ATI_REG_ADDRESS,
  SH_ATI_REG_OUTPUT,
  SH_ATI_REG_CONST,
  SH_ATI_REG_TEXTURE
};

class AtiCode : public SH::ShBackendCode { 
public:
  AtiCode(SH::ShRefCount<AtiBackend> backend, const SH::ShProgram& shader, int kind);
  virtual ~AtiCode();

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
  void genDiv( SH::ShVariable dest, SH::ShVariable op1, SH::ShVariable op2 );

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
                   const AtiBindingSpecs& specs, 
                   std::vector<int>& bindings,
                   ShAtiRegType type, int& num);

  /// Output a use of a variable.
  std::ostream& printVar(std::ostream& out, bool dest, const SH::ShVariable& var,
                         bool collectingOp,
                         const SH::ShSwizzle& destSwiz) const;

  /// Check whether inst is a sampling instruction. If so, output it
  /// and return true. Otherwise, output nothing and return false.
  bool printSamplingInstruction(std::ostream& out, const AtiInst& inst) const;
  
  SH::ShRefCount<AtiBackend> m_backend;
  SH::ShProgram m_shader;
  int m_kind;

  typedef std::vector<AtiInst> AtiInstList;
  AtiInstList m_instructions;

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

  typedef std::map<SH::ShVariableNodePtr, AtiReg> RegMap;
  RegMap m_registers;

  std::vector<int> m_outputBindings;
  std::vector<int> m_inputBindings;

  /// ATI Program ID we are bound to. 0 if code hasn't been uploaded yet.
  unsigned int m_programId;
};

typedef SH::ShRefCount<AtiCode> AtiCodePtr;

class AtiBackend : public SH::ShBackend {
public:
  AtiBackend();
  ~AtiBackend();
  
  std::string name() const;

  SH::ShBackendCodePtr generateCode(int kind, const SH::ShProgram& shader);
  virtual void bindFramebuffer();
  virtual void deleteUberbuffer(SH::ShUberbufferPtr ub);

  /// Allocate uber buffers (called by allocRegs)
  void allocFramebuffer(SH::ShFramebufferPtr fb);

  /// Allocate uber buffers (called by allocRegs)
  void allocUberbuffer(SH::ShUberbufferPtr ub);
  // TODO add flag to force allocation
  
  /// Checks for uber buffer errors and prints them
  void printUbErrors();

  void init2(void);
  void render(SH::ShVertexArray& array);
  void render_planar(SH::ShVertexArray& array);

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

typedef SH::ShRefCount<AtiBackend> AtiBackendPtr;

}

#endif
