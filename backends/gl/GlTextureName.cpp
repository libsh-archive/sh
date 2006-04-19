// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "GlTextureName.hpp"

namespace shgl {

using namespace SH;

GlTextureName::GlTextureName(GLenum target)
  : m_target(target),
    m_params(0, SH::TextureTraits::FILTER_NONE,
             SH::TextureTraits::WRAP_CLAMP),
    m_managed(true)
{
  glGenTextures(1, &m_name);
  m_names->push_back(this);
}

GlTextureName::GlTextureName(GLenum target, GLuint name)
  : m_target(target),
    m_name(name),
    m_params(0, SH::TextureTraits::FILTER_NONE,
             SH::TextureTraits::WRAP_CLAMP),
    m_managed(false)
{
  m_names->push_back(this);
}

GlTextureName::~GlTextureName()
{
  m_names->erase(std::remove(m_names->begin(), m_names->end(), this), m_names->end());
  if (m_managed) {
    glDeleteTextures(1, &m_name);
  }
}

void GlTextureName::addStorage(SH::Storage* storage)
{
  m_storages.push_back(storage);
}

void GlTextureName::removeStorage(SH::Storage* storage)
{
  m_storages.erase(std::remove(m_storages.begin(), m_storages.end(), storage));
}

void GlTextureName::params(const SH::TextureTraits& params)
{
  Binding binding(this);

  if (params.interpolation() == 0) {
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  } else if (params.interpolation() == 1) {
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  }
  if (params.wrapping() == SH::TextureTraits::WRAP_CLAMP) {
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP));
  } else if (params.wrapping() == SH::TextureTraits::WRAP_CLAMP_TO_EDGE) {
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
  } else if (params.wrapping() == SH::TextureTraits::WRAP_REPEAT) {
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT));
  }
  if (params.filtering() == SH::TextureTraits::FILTER_MIPMAP_LINEAR) {
    if (0 == params.interpolation()) {
      GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
    } else {
      GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    }
  } else if (params.filtering() == SH::TextureTraits::FILTER_MIPMAP_NEAREST) {
    if (0 == params.interpolation()) {
      GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
    } else {
      GL_CHECK_ERROR(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
    }
  }
  
  m_params = params;
}

GlTextureName::NameList* GlTextureName::m_names = new GlTextureName::NameList();

GlTextureName::Binding::Binding(const Pointer<const GlTextureName>& name)
{
  target = name->target();
  
  switch(target) {
  case GL_TEXTURE_1D:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_1D, &last));
    break;
  case GL_TEXTURE_2D:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last));
    break;
  case GL_TEXTURE_CUBE_MAP:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &last));
    break;
#if defined( __APPLE__ )
  case GL_TEXTURE_RECTANGLE_EXT:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_EXT, &last));
#else
  case GL_TEXTURE_RECTANGLE_NV:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_NV, &last));
#endif // __APPLE__
    break;
  case GL_TEXTURE_3D:
    GL_CHECK_ERROR(glGetIntegerv(GL_TEXTURE_BINDING_3D, &last));
    break;
  default:
    SH_DEBUG_WARN("Texture target not handled by GL backend");
    break;
  }
    
  GL_CHECK_ERROR(glBindTexture(target, name->value()));
}

GlTextureName::Binding::~Binding()
{
  GL_CHECK_ERROR(glBindTexture(target, last));
}


}
