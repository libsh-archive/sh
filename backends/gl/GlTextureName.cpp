// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#include "GlTextureName.hpp"

namespace shgl {

using namespace SH;

GlTextureName::GlTextureName(GLenum target)
  : m_target(target),
    m_params(0, SH::ShTextureTraits::SH_FILTER_NONE,
             SH::ShTextureTraits::SH_WRAP_CLAMP, SH::ShTextureTraits::SH_CLAMPED),
    m_managed(true)
{
  glGenTextures(1, &m_name);
  m_names.push_back(this);
}

GlTextureName::GlTextureName(GLenum target, GLuint name)
  : m_target(target),
    m_name(name),
    m_params(0, SH::ShTextureTraits::SH_FILTER_NONE,
             SH::ShTextureTraits::SH_WRAP_CLAMP, SH::ShTextureTraits::SH_CLAMPED),
    m_managed(false)
{
  m_names.push_back(this);
}

GlTextureName::~GlTextureName()
{
  m_names.erase(std::remove(m_names.begin(), m_names.end(), this));
  if (m_managed) {
    glDeleteTextures(1, &m_name);
  }
}

void GlTextureName::addStorage(SH::ShStorage* storage)
{
  m_storages.push_back(storage);
}

void GlTextureName::removeStorage(SH::ShStorage* storage)
{
  m_storages.erase(std::remove(m_storages.begin(), m_storages.end(), storage));
}

void GlTextureName::params(const SH::ShTextureTraits& params)
{
  Binding binding(this);

  if (params.interpolation() == 0) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  } else if (params.interpolation() == 1) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  }
  if (params.wrapping() == SH::ShTextureTraits::SH_WRAP_CLAMP) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP));
  } else if (params.wrapping() == SH::ShTextureTraits::SH_WRAP_CLAMP_TO_EDGE) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
  } else if (params.wrapping() == SH::ShTextureTraits::SH_WRAP_REPEAT) {
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT));
    SH_GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT));
  }
  // TODO: SH_FILTER (Mipmapping)
  
  m_params = params;
}

GlTextureName::NameList GlTextureName::m_names = GlTextureName::NameList();

GlTextureName::Binding::Binding(const ShPointer<const GlTextureName>& name)
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
#if defined( __APPLE__ )
  case GL_TEXTURE_RECTANGLE_EXT:
    SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_EXT, &last));
#else
  case GL_TEXTURE_RECTANGLE_NV:
    SH_GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_NV, &last));
#endif // __APPLE__
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
