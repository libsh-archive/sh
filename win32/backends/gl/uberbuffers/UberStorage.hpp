#ifndef SHUBERSTORAGE_HPP
#define SHUBERSTORAGE_HPP

#include "GlBackend.hpp"
#include "ShRefCount.hpp"
#include "ShMemory.hpp"
#include "GlTextureName.hpp"

namespace shgl {

class HostUberTransfer;
class UberStorage : public SH::ShStorage {
public:
  UberStorage(int context,
              SH::ShMemory* memory, const GlTextureNamePtr& name,
              int width, int height, int pitch);
  ~UberStorage();
  
  void bindAsTexture();
  void bindAsAux(unsigned int n);
  void unbind();
  
  enum Binding {
    SH_UBER_UNBOUND,
    SH_UBER_TEXTURE,
    SH_UBER_AUX
  };

  Binding binding() const;
  GlTextureNamePtr textureName() const;
  int auxTarget() const; // returns -1 if binding != aux
  
  int width() const { return m_width; }
  int height() const { return m_height; }
  int pitch() const { return m_pitch; }
  int context() const { return m_context; }
  unsigned int mem() const { return m_mem; }

  std::string id() const { return "uberbuffer"; }
 
  // we need a dummy framebuffer object
  static int temp_fb[4];
  static int allocfb(int, bool bForce = false);
 
  void clearBuffer(float* col);

private:

  static bool m_firstTime;
  
  /// manipulate memory objects
  unsigned int alloc(int bForce=0);
  unsigned int remove();

  int m_context;
  
  int m_width, m_height, m_pitch;
          
  // the uber buffer
  unsigned int m_mem;

  Binding m_binding;
  
  GlTextureNamePtr m_textureName;
  int m_auxTarget; // m_binding == SH_UBER_AUX
  
  friend class HostUberTransfer;
  friend class UberUberTransfer;
};

typedef SH::ShPointer<UberStorage> UberStoragePtr;

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
