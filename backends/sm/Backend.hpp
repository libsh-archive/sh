#ifndef SHSM_BACKEND_HPP
#define SHSM_BACKEND_HPP

#include <string>
#include <map>
#include <iosfwd>
#include <list>
#include "ShBackend.hpp"
#include "ShVariableNode.hpp"
#include "ShBasicBlock.hpp"
#include "sm.hpp"
#include "ShRefCount.hpp"
#include "ShTextureNode.hpp"

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
  SmRegister(SmRegType type, int index)
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
  BackendCode(SH::ShRefCount<Backend> backend, const SH::ShShader& shader);
  ~BackendCode();

  void upload();
  void bind();
  void updateUniform(const SH::ShVariableNodePtr& uniform);
  
  std::ostream& print(std::ostream& out);

  int label(const SH::ShBasicBlockPtr& block);
  void addBasicBlock(const SH::ShBasicBlockPtr& block);

  void allocRegs();

private:
  SH::ShRefCount<Backend> m_backend;
  SH::ShShader m_shader;
  SMshader m_smShader;

  typedef std::list<SmInstruction> SmInstList;
  SmInstList m_instructions; ///< The actual code.

  /// Returns true if a register has been allocated to var.
  bool haveReg(const SH::ShVariableNodePtr& var);

  /// Allocates a register if there is none yet.
  SmRegister getReg(const SH::ShVariableNodePtr& var);
  
  /// Allocates a register if there is none yet.
  SMreg getSmReg(const SH::ShVariable& var);

  int m_maxCR;
  int m_maxTR;
  int m_maxIR;
  int m_maxOR;
  int m_maxTex;
  
  typedef std::map<SH::ShVariableNodePtr, SmRegister> RegMap;
  RegMap m_registers; ///< Really Simple(TM) register allocation

  SMreg* m_cR;
  SMreg* m_tR;
  SMreg* m_iR;
  SMreg* m_oR;

  typedef std::map<SH::ShTextureNodePtr, SMtexture> TextureNodeMap;

  TextureNodeMap m_textureMap;

  std::string printVar(const SH::ShVariable& var);
};

typedef SH::ShRefCount<BackendCode> BackendCodePtr;

class Backend : public SH::ShBackend {
public:
  Backend();
  ~Backend();
  std::string name() const;
  SH::ShBackendCodePtr generateCode(const SH::ShShader& shader);

private:
  void generateNode(BackendCodePtr& code, const SH::ShCtrlGraphNodePtr& node);
};

typedef SH::ShRefCount<Backend> BackendPtr;

}
#endif
