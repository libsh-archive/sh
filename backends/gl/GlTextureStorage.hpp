#ifndef SH_GLTEXTURESTORAGE_HPP
#define SH_GLTEXTURESTORAGE_HPP

#include "GlBackend.hpp"
#include "ShMemory.hpp"
#include "GlTextureName.hpp"

namespace shgl {

class GlTextureStorage : public SH::ShStorage {
public:
  GlTextureStorage(int context,
                   SH::ShMemory* memory, GLenum target,
                   GLenum format, GLint internalFormat,
                   int width, int height, int depth,
                   GlTextureNamePtr name);
  
  ~GlTextureStorage();
  
  std::string id() const { return "opengl:texture"; }
  
  GLuint name() const { return m_name->value(); }
  const GlTextureNamePtr& texName() const { return m_name; }
  GLenum target() const { return m_target; }
  GLenum format() const { return m_format; }
  GLint internalFormat() const { return m_internalFormat; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int depth() const { return m_depth; }
  int context() const { return m_context; }
  
private:
  int m_context;
  GlTextureNamePtr m_name;
  GLenum m_target;
  GLenum m_format;
  GLint m_internalFormat;
  int m_width, m_height, m_depth;
  
  unsigned int m_params;

  GlTextureStorage(const GlTextureStorage&);
  GlTextureStorage& operator=(const GlTextureStorage&);
  
  // TODO: Mipmapping?
};

typedef SH::ShRefCount<GlTextureStorage> GlTextureStoragePtr;

}

#endif
