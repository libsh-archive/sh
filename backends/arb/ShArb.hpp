#ifndef SHARB_HPP
#define SHARB_HPP

#include <string>
#include <vector>
#include "ShBackend.hpp"
#include "ShRefCount.hpp"
#include "ShCtrlGraph.hpp"

namespace ShArb {

class ArbBackend;
class ArbInst;

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

private:

  /// Generate code for this node and those following it.
  void genNode(SH::ShCtrlGraphNodePtr node);

  /// Allocate registers, after the code has been generated
  void allocRegs();
  
  SH::ShRefCount<ArbBackend> m_backend;
  SH::ShShader m_shader;

  typedef std::vector<ArbInst> ArbInstList;
  ArbInstList m_instructions;
};

typedef SH::ShRefCount<ArbCode> ArbCodePtr;

class ArbBackend : public SH::ShBackend {
public:
  ArbBackend();
  ~ArbBackend();
  
  std::string name() const;

  SH::ShBackendCodePtr generateCode(const SH::ShShader& shader);

};

typedef SH::ShRefCount<ArbBackend> ArbBackendPtr;

}

#endif
