#include <GL/gl.h>
#include "ShMemory.hpp"
#include "ShArb.hpp"
#include "ShArbMemory.hpp"
#include "ShDebug.hpp"
#include <iostream>

namespace ShArb {

using namespace SH;

TextureName::TextureName(GLenum target)
  : m_target(target)
{
  glGenTextures(1, &m_name);
  m_names.push_back(this);
}

TextureName::~TextureName()
{
  m_names.erase(std::remove(m_names.begin(), m_names.end(), this));
  glDeleteTextures(1, &m_name);
}

void TextureName::addStorage(TextureStorage* storage)
{
  m_storages.push_back(storage);
}

void TextureName::removeStorage(TextureStorage* storage)
{
  m_storages.erase(std::remove(m_storages.begin(), m_storages.end(), storage));
}

TextureName::NameList TextureName::m_names = TextureName::NameList();

TextureStorage::TextureStorage(ShMemory* memory, GLenum target,
                               GLenum format, GLint internalFormat,
                               int width, int height, int depth,
                               TextureNamePtr name)
  : ShStorage(memory), m_name(name), m_target(target), m_format(format),
    m_internalFormat(internalFormat),
    m_width(width), m_height(height), m_depth(depth)
{
  m_name->addStorage(this);
}

TextureStorage::~TextureStorage()
{
  m_name->removeStorage(this);
}

// Utility class to bind texture temporarily
struct BindTexture {
  BindTexture(const ShRefCount<const TextureName>& name)
  {
    target = name->target();
    
    switch(target) {
    case GL_TEXTURE_1D:
      SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_1D, &last));
      break;
    case GL_TEXTURE_2D:
      SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last));
      break;
    case GL_TEXTURE_CUBE_MAP:
      SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &last));
      break;
    case GL_TEXTURE_RECTANGLE_NV:
      SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_NV, &last));
      break;
    case GL_TEXTURE_3D:
      SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_3D, &last));
      break;
    default:
      SH_DEBUG_WARN("Texture target not handled by ARB backend");
      break;
    }
    
    SH_GL_CHECK_ERROR(glBindTexture(target, name->value()));
  }

  ~BindTexture()
  {
    SH_GL_CHECK_ERROR(glBindTexture(target, last));
  }
  
  GLenum target;
  GLint last;
};

void TextureName::params(unsigned int params)
{
  BindTexture binding(this);

  if (params & SH_LOOKUP_NEAREST) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  } else if (params & SH_LOOKUP_LINEAR) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  }
  // TODO: SH_FILTER (Mipmapping)
  if (params & SH_WRAP_CLAMP) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP));
  } else if (params & SH_WRAP_REPEAT) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT));
  }
  
  m_params = params;
}

class HostTextureTransfer : public ShTransfer {
  HostTextureTransfer()
    : ShTransfer("host", "arb:texture")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    const ShHostStorage* host = dynamic_cast<const ShHostStorage*>(from);
    TextureStorage* texture = dynamic_cast<TextureStorage*>(to);

    SH_DEBUG_PRINT("host->arb:texture: Transferring from host "
                   << host->data() << " to texture " << texture->name());
    
    BindTexture binding(texture->texName());
    
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
      SH_DEBUG_WARN("Texture target " << texture->target() << " not handled by ARB backend");
      break;
    }
    return true;
  }
  
  int cost()
  {
    // Texture downloads are expensive!
    return 100;
  }

  static HostTextureTransfer* instance;
};

HostTextureTransfer* HostTextureTransfer::instance = new HostTextureTransfer();

}
