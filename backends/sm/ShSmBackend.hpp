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
#ifndef SHSM_BACKEND_HPP
#define SHSM_BACKEND_HPP

#include <string>
#include <map>
#include <iosfwd>
#include <list>
#include <sm/sm.hpp>
#include "ShBackend.hpp"
#include "ShVariableNode.hpp"
#include "ShBasicBlock.hpp"
#include "ShRefCount.hpp"
#include "ShTextureNode.hpp"
#include "ShCtrlGraph.hpp"
#include "ShTransformer.hpp"

namespace ShSm {

struct SmInstruction {
  SmInstruction(const Operation& op, const SH::ShVariable& dest)
    : op(op), dest(dest), src1(0), src2(0), src3(0)
  {}
  SmInstruction(const Operation& op, const SH::ShVariable& dest, const SH::ShVariable& src1)
    : op(op), dest(dest), src1(src1), src2(0), src3(0)
  {}
  SmInstruction(const Operation& op, const SH::ShVariable& dest, const SH::ShVariable& src1,
                const SH::ShVariable& src2)
    : op(op), dest(dest), src1(src1), src2(src2), src3(0)
  {}
  SmInstruction(const Operation& op, const SH::ShVariable& dest, const SH::ShVariable& src1,
                const SH::ShVariable& src2, const SH::ShVariable& src3)
    : op(op), dest(dest), src1(src1), src2(src2), src3(src3)
  {}
  Operation op;
  SH::ShVariable dest;
  SH::ShVariable src1, src2, src3;
};

enum SmRegType {
  SHSM_REG_INPUT = 0,
  SHSM_REG_OUTPUT = 1,
  SHSM_REG_TEMP = 2,
  SHSM_REG_CONST = 3,
  SHSM_REG_TEXTURE = 4,
};

struct SmRegister {
  SmRegister() {}
  SmRegister(SmRegType type, int index, std::string name = "")
    : type(type), index(index)
  {
  }
  SmRegType type;
  int index;

  std::string print() const;
};

class Backend;

class BackendCode : public SH::ShBackendCode {
public:
  BackendCode(SH::ShRefCount<Backend> backend, const SH::ShProgram& shader,
              const std::string& target);
  virtual ~BackendCode();

  virtual bool allocateRegister(const SH::ShVariableNodePtr& var);
  virtual void freeRegister(const SH::ShVariableNodePtr& var);
  
  virtual void upload();
  virtual void bind();
  virtual void updateUniform(const SH::ShVariableNodePtr& uniform);

  /// Actually generate the code, and do register allocation.
  void generate();
  
  std::ostream& print(std::ostream& out);

  int label(const SH::ShBasicBlockPtr& block);
  void addBasicBlock(const SH::ShBasicBlockPtr& block);

  void allocRegs();

  const std::string& target() const { return m_target; }

private:
  SH::ShRefCount<Backend> m_backend;
  SH::ShProgram m_shader;
  SH::ShProgram m_originalShader;
  SMshader m_smShader;
  std::string m_target;

  typedef std::vector<SmInstruction> SmInstList;
  SmInstList m_instructions; ///< The actual code.

  /// Generates an instruction for a scalar/vector binary op
  // where the scalar must be promoted to a vector by duplicating 
  // components.
  void genScalarVectorInst( SH::ShVariable dest, SH::ShVariable op1, 
    SH::ShVariable op2, Operation opcode ); 

  /// Returns true if a register has been allocated to var.
  bool haveReg(const SH::ShVariableNodePtr& var);

  /// Allocates a register if there is none yet, except for temporaries
  SmRegister getReg(const SH::ShVariableNodePtr& var);
  
  /// Allocates a register if there is none yet, except for temporaries
  SMreg getSmReg(const SH::ShVariable& var);

  /// A list of temporary register indices which are available.
  std::list<int> m_tempRegs;
  
  int m_maxCR;
  int m_maxTR;
  int m_maxIR;
  int m_maxOR;
  int m_maxTex;

  typedef std::map<SH::ShVariableNodePtr, SmRegister> RegMap;
  RegMap m_registers;

  SMreg* m_cR;
  SMreg* m_tR;
  SMreg* m_iR;
  SMreg* m_oR;

  typedef std::map<SH::ShTextureNodePtr, SMtexture> TextureNodeMap;

  TextureNodeMap m_textureMap;

  SH::ShTransformer::VarSplitMap m_splits;

  std::string printVar(const SH::ShVariable& var);
};

typedef SH::ShRefCount<BackendCode> BackendCodePtr;

class Backend : public SH::ShBackend {
public:
  Backend();
  ~Backend();
  std::string name() const;
  SH::ShBackendCodePtr generateCode(const std::string& target, const SH::ShProgram& shader);

  void execute(const SH::ShProgram& program, SH::ShStream& dest);

private:
  void generateNode(BackendCodePtr& code, const SH::ShCtrlGraphNodePtr& node);
};

typedef SH::ShRefCount<Backend> BackendPtr;

}
#endif
