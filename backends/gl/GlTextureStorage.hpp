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
#ifndef SH_GLTEXTURESTORAGE_HPP
#define SH_GLTEXTURESTORAGE_HPP

#include "ShVariableType.hpp"
#include "ShMemory.hpp"
#include "GlBackend.hpp"
#include "GlTextureName.hpp"
#include <sh.hpp>

namespace shgl {

class GlTextureStorage : public SH::ShStorage {
public:
  GlTextureStorage(SH::ShMemory* memory, GLenum target,
                   GLenum format, GLint internalFormat,
                   SH::ShValueType valueType, 
                   int width, int height, int depth, int tuplesize,
                   GlTextureNamePtr name, 
                   GLint mipmap_level, bool internalRGB);
  
  ~GlTextureStorage();
  
  void initTexture();
  
  std::string id() const { return "opengl:texture"; }
  
  GLuint name() const { return m_name->value(); }
  const GlTextureNamePtr& texName() const { return m_name; }
  GLenum target() const { return m_target; }
  GLenum format() const { return m_format; }
  GLint internalFormat() const { return m_internalFormat; }
  bool internalFormatRGB() const { return m_internalFormatRGB; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int depth() const { return m_depth; }
  int tuplesize() const { return m_tuplesize; }
  GLint mipmap_level() const { return m_mipmap_level; }
  
  bool write() const { return m_write; }
  void write(bool w) { m_write = w; }
private:
  GlTextureNamePtr m_name;

  // OpenGL texture params.  type will be determined by the Transfer function
  GLenum m_target;
  GLenum m_format;
  GLint m_internalFormat;

  int m_width, m_height, m_depth, m_tuplesize;
  
  unsigned int m_params;
  
  // True if the storage is currently being written to
  bool m_write;
  // True if the internal storage format is a form of RGB or RGBA
  bool m_internalFormatRGB;

  GlTextureStorage(const GlTextureStorage&);
  GlTextureStorage& operator=(const GlTextureStorage&);
  
  GLint m_mipmap_level;
};

typedef SH::ShPointer<GlTextureStorage> GlTextureStoragePtr;
typedef SH::ShPointer<const GlTextureStorage> GlTextureStorageCPtr;

class HostGlTextureTransfer : public SH::ShTransfer {
public:
  HostGlTextureTransfer();
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);
  int cost(const SH::ShStorage* from, const SH::ShStorage* to);
  static HostGlTextureTransfer* instance;
};

class GlTextureHostTransfer : public SH::ShTransfer {
public:
  GlTextureHostTransfer();
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);
  int cost(const SH::ShStorage* from, const SH::ShStorage* to);
  static GlTextureHostTransfer* instance;
};

class GlTextureGlTextureTransfer : public SH::ShTransfer {
public:
  GlTextureGlTextureTransfer();
  bool transfer(const SH::ShStorage* from, SH::ShStorage* to);
  int cost(const SH::ShStorage* from, const SH::ShStorage* to);
  static GlTextureGlTextureTransfer* instance;

private:
  SH::ShArray2D<SH::ShAttrib4f> source_texture;
  SH::ShProgramSetPtr render_to_tex_prog;
};

}

#endif
