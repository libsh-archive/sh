#ifndef SHBACKEND_HPP
#define SHBACKEND_HPP

#include <vector>
#include <iosfwd>
#include <string>
#include "ShRefCount.hpp"
#include "ShShader.hpp"

namespace SH  {

class ShBackendCode : public ShRefCountable {
public:
  virtual ~ShBackendCode();
  virtual void upload() = 0;
  virtual void bind() = 0;

  virtual std::ostream& print(std::ostream& out) = 0;
};

typedef ShRefCount<ShBackendCode> ShBackendCodePtr;

class ShBackend : public ShRefCountable {
public:
  virtual ~ShBackend();
  virtual std::string name() const = 0;
  
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
