#ifndef SH_GLTEXTURENAME_HPP
#define SH_GLTEXTURENAME_HPP

#include "GlBackend.hpp"
#include "ShMemory.hpp"


namespace shgl {

class GlTextureName : public SH::ShRefCountable {
public:
  GlTextureName(GLenum target);
  ~GlTextureName();

  GLuint value() const { return m_name; }
  GLenum target() const { return m_target; }
  unsigned int params() const { return m_params; }
  void params(unsigned int params);

  void addStorage(SH::ShStorage* storage);
  void removeStorage(SH::ShStorage* storage);

  typedef std::list<SH::ShStorage*> StorageList;
  StorageList::const_iterator beginStorages() const { return m_storages.begin(); }
  StorageList::const_iterator endStorages() const { return m_storages.end(); }

  typedef std::list<GlTextureName*> NameList;
  static NameList::const_iterator beginNames() { return m_names.begin(); }
  static NameList::const_iterator endNames() { return m_names.end(); }

  // Utility class to bind texture temporarily
  struct Binding {
    Binding(const SH::ShRefCount<const GlTextureName>& name);
    ~Binding();
  
    GLenum target;
    GLint last;
  };
  
private:
  GLenum m_target;
  GLuint m_name;
  StorageList m_storages;
  static NameList m_names;
  unsigned int m_params;
};

typedef SH::ShRefCount<GlTextureName> GlTextureNamePtr;

}

#endif
