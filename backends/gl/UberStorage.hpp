#ifndef SHUBERSTORAGE_HPP
#define SHUBERSTORAGE_HPP

#include "ShRefCount.hpp"
#include "ShMemory.hpp"

namespace shgl {

class HostUberTransfer;
class UberStorage : public SH::ShStorage {
public:
  UberStorage(SH::ShMemory* memory, 
                int width, int height, int pitch);

  // TODO
  void bindAsTexture(GLenum target);
  void bindAsAux(int n);
  
  int width() const { return m_width; }
  int height() const { return m_height; }
  int pitch() const { return m_pitch; }
  unsigned int mem()const{ return m_mem; }

  std::string id() const { return "uberbuffer"; }
 
  // we need a dummy framebuffer object
  static int temp_fb[4];
 
private:
  
  /// manipulate memory objects
  unsigned int alloc(int bForce=0);
  unsigned int remove();

  int m_width, m_height, m_pitch;
          
  // the uber buffer
  unsigned int m_mem;

  friend class HostUberTransfer;
  friend class UberUberTransfer;
};

typedef SH::ShRefCount<UberStorage> UberStoragePtr;

class UberUberTransfer : public SH::ShTransfer {
public:
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);

  int cost()
  {
    return 10;
  }

private:
  UberUberTransfer()
    : ShTransfer("uberbuffer", "uberbuffer")
  {
  }
  
  static UberUberTransfer* instance;
};


class UberHostTransfer : public SH::ShTransfer {
public:
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);
  int cost()
  {
    return 100;
  }

private:
  UberHostTransfer()
    : ShTransfer("uberbuffer", "host")
  {
  }
  
  static UberHostTransfer* instance;
};


class HostUberTransfer : public SH::ShTransfer {
public:
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);
  int cost()
  {
    return 100;
  }

private:
  HostUberTransfer()
    : ShTransfer("host", "uberbuffer")
  {
  }
  
  static HostUberTransfer* instance;
};

}

#endif 
