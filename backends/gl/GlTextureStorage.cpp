#include "GlTextureStorage.hpp"

namespace shgl {

using namespace SH;

GlTextureStorage::GlTextureStorage(ShMemory* memory, GLenum target,
                                   GLenum format, GLint internalFormat,
                                   int width, int height, int depth,
                                   GlTextureNamePtr name)
  : ShStorage(memory), m_name(name), m_target(target), m_format(format),
    m_internalFormat(internalFormat),
    m_width(width), m_height(height), m_depth(depth)
{
  m_name->addStorage(this);
}

GlTextureStorage::~GlTextureStorage()
{
  m_name->removeStorage(this);
}

class HostGlTextureTransfer : public ShTransfer {
  HostGlTextureTransfer()
    : ShTransfer("host", "opengl:texture")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    const ShHostStorage* host = dynamic_cast<const ShHostStorage*>(from);
    GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

    SH_DEBUG_PRINT("host->opengl:texture: Transferring from host "
                   << host->data() << " to texture " << texture->name());

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texture->texName());
    
    switch(texture->target()) {
    case GL_TEXTURE_1D:
      SH_GL_CHECK_ERROR(glTexImage1D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), 0, texture->format(), GL_FLOAT,
                                     host->data()));
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_RECTANGLE_NV:
      SH_GL_CHECK_ERROR(glTexImage2D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), texture->height(), 0, texture->format(),
                                     GL_FLOAT, host->data()));
      break;
    case GL_TEXTURE_3D:
      SH_GL_CHECK_ERROR(glTexImage3D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), texture->height(),
                                     texture->depth(), 0, texture->format(),
                                     GL_FLOAT, host->data()));
      break;
    default:
      SH_DEBUG_WARN("Texture target " << texture->target() << " not handled by GL backend");
      break;
    }
    return true;
  }
  
  int cost()
  {
    // Texture downloads are expensive!
    return 100;
  }

  static HostGlTextureTransfer* instance;
};

HostGlTextureTransfer* HostGlTextureTransfer::instance = new HostGlTextureTransfer();

}
