#include "GlTextureName.hpp"

namespace shgl {

using namespace SH;

GlTextureName::GlTextureName(GLenum target)
  : m_target(target)
{
  glGenTextures(1, &m_name);
  m_names.push_back(this);
}

GlTextureName::~GlTextureName()
{
  m_names.erase(std::remove(m_names.begin(), m_names.end(), this));
  glDeleteTextures(1, &m_name);
}

void GlTextureName::addStorage(SH::ShStorage* storage)
{
  m_storages.push_back(storage);
}

void GlTextureName::removeStorage(SH::ShStorage* storage)
{
  m_storages.erase(std::remove(m_storages.begin(), m_storages.end(), storage));
}

void GlTextureName::params(unsigned int params)
{
  Binding binding(this);

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

GlTextureName::NameList GlTextureName::m_names = GlTextureName::NameList();

GlTextureName::Binding::Binding(const ShRefCount<const GlTextureName>& name)
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
    SH_DEBUG_WARN("Texture target not handled by GL backend");
    break;
  }
    
  SH_GL_CHECK_ERROR(glBindTexture(target, name->value()));
}

GlTextureName::Binding::~Binding()
{
  SH_GL_CHECK_ERROR(glBindTexture(target, last));
}


}
