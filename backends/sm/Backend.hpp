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
  SHSM_REG_INPUT,
  SHSM_REG_OUTPUT,
  SHSM_REG_TEMP,
  SHSM_REG_CONST,
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


class BackendCode : public SH::ShBackendCode {
public:
  BackendCode(const SH::ShShader& shader);
  ~BackendCode();

  void upload();
  void bind();
  std::ostream& print(std::ostream& out);

  int label(const SH::ShBasicBlockPtr& block);
  void addBasicBlock(const SH::ShBasicBlockPtr& block);

  void allocRegs();

private:
  SH::ShShader m_shader;
  SMshader m_smShader;
  
  int m_maxLabel;
  std::map<SH::ShBasicBlockPtr, int> m_labels;

  typedef std::list<SmInstruction> SmInstList;
  SmInstList m_instructions; ///< The actual code.

  SmRegister getReg(const SH::ShVariableNodePtr& var);
  SMreg getSmReg(const SH::ShVariableNodePtr& var);

  int m_maxCR;
  int m_maxTR;
  int m_maxIR;
  int m_maxOR;
  typedef std::map<SH::ShVariableNodePtr, SmRegister> RegMap;
  RegMap m_registers; ///< Really Simple(TM) register allocation

  SMreg* m_cR;
  SMreg* m_tR;
  SMreg* m_iR;
  SMreg* m_oR;

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

}
#endif
