#ifndef SHBACKEND_HPP
#define SHBACKEND_HPP

#include <vector>
#include <iosfwd>
#include <string>
#include "ShRefCount.hpp"
#include "ShShader.hpp"
#include "ShVariableNode.hpp"

namespace SH  {

class ShBackendCode : public ShRefCountable {
public:
  virtual ~ShBackendCode();

  /// Used by a register allocater to signal that a register should be
  /// allocated to var. Return true iff the allocation succeeded.
  virtual bool allocateRegister(const ShVariableNodePtr& var) = 0;

  /// Used by the register allocator to signal that the register used
  /// by var can be used by other registers in future
  /// allocateRegister() calls.
  virtual void freeRegister(const ShVariableNodePtr& var) = 0;
  
  /// Upload this shader code to the GPU.
  virtual void upload() = 0;

  /// Bind this shader code after it has been uploaded.
  virtual void bind() = 0;

  /// Update the value of a uniform parameter after it has changed.
  virtual void updateUniform(const ShVariableNodePtr& uniform) = 0;

  virtual std::ostream& print(std::ostream& out) = 0;
};

typedef ShRefCount<ShBackendCode> ShBackendCodePtr;

class ShBackend : public ShRefCountable {
public:
  virtual ~ShBackend();
  virtual std::string name() const = 0;

  /// Generate the backend code for a particular shader. Ensure that
  /// ShEnvironment::shader is the same as shader before calling this,
  /// since extra variables may be declared inside this function!
  virtual ShBackendCodePtr generateCode(const ShShader& shader) = 0;

  typedef std::vector< ShRefCount<ShBackend> > ShBackendList;

  static ShBackendList::iterator begin();
  static ShBackendList::iterator end();

  static ShRefCount<ShBackend> lookup(const std::string& name);

protected:
  ShBackend();
  
private:
  static ShBackendList* m_backends;
};

typedef ShRefCount<ShBackend> ShBackendPtr;

}

#endif
