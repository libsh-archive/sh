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
#include "SmShaderObject.hpp"
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

class BackendCode : public SH::ShBackendCode {
public:
  BackendCode();
  ~BackendCode();

  void upload();
  void bind();
  std::ostream& print(std::ostream& out);

  int label(const SH::ShBasicBlockPtr& block);

  typedef std::list<SmInstruction> SmInstList;
  
  void addBasicBlock(const SH::ShBasicBlockPtr& block);

private:
  int m_maxLabel;
  std::map<SH::ShBasicBlockPtr, int> m_labels;

  std::map<SH::ShVariableNodePtr, SMreg> m_registers; ///< Really Simple(TM) register allocation
  SmInstList m_instructions; ///< The actual code.
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
