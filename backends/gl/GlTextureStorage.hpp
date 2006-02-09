// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SH_GLTEXTURESTORAGE_HPP
#define SH_GLTEXTURESTORAGE_HPP

#include "ShVariableType.hpp"
#include "ShMemory.hpp"
#include "GlBackend.hpp"
#include "GlTextureName.hpp"

namespace shgl {

class GlTextureStorage : public SH::ShStorage {
public:
  GlTextureStorage(SH::ShMemory* memory, GLenum target,
                   GLenum format, GLint internalFormat,
                   SH::ShValueType valueType, 
                   int width, int height, int depth, int tuplesize,
                   int count, GlTextureNamePtr name, GLint mipmap_level);
  
  ~GlTextureStorage();
  
  void initTexture();
  
  std::string id() const { return "opengl:texture"; }
  
  GLuint name() const { return m_name->value(); }
  const GlTextureNamePtr& texName() const { return m_name; }
  GLenum target() const { return m_target; }
  GLenum format() const { return m_format; }
  GLint internalFormat() const { return m_internalFormat; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int depth() const { return m_depth; }
  int tuplesize() const { return m_tuplesize; }
  int count() const { return (m_count != -1) ? m_count : m_width * m_height * m_depth; }
  GLint mipmap_level() const { return m_mipmap_level; }
  
  bool write() const { return m_write; }
  void write(bool w) { m_write = w; }
private:
  GlTextureNamePtr m_name;

  // OpenGL texture params.  type will be determined by the Transfer function
  GLenum m_target;
  GLenum m_format;
  GLint m_internalFormat;

  int m_width, m_height, m_depth, m_tuplesize, m_count;
  
  unsigned int m_params;
  
  bool m_write;

  GlTextureStorage(const GlTextureStorage&);
  GlTextureStorage& operator=(const GlTextureStorage&);
  
  GLint m_mipmap_level;
};

typedef SH::ShPointer<GlTextureStorage> GlTextureStoragePtr;

}

#endif
