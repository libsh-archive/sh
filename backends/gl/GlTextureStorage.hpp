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
                   int count, GlTextureNamePtr name);
  
  ~GlTextureStorage();

  // Initialize TexImages without sending any data
  void init();
  
  std::string id() const { return "opengl:texture"; }
  
  GLuint name() const { return m_name->value(); }
  const GlTextureNamePtr& texName() const { return m_name; }
  GLenum target() const { return m_target; }
  GLenum format() const { return m_format; }
  GLint internalFormat() const { return m_internalFormat; }
  SH::ShValueType valueType() const { return m_valueType; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  int depth() const { return m_depth; }
  int tuplesize() const { return m_tuplesize; }
  int count() const { return (m_count != -1) ? m_count : m_width * m_height * m_depth; }
  
private:
  GlTextureNamePtr m_name;

  // OpenGL texture params.  type will be determined by the Transfer function
  GLenum m_target;
  GLenum m_format;
  GLint m_internalFormat;

  SH::ShValueType m_valueType; // type index expected of data on host
  int m_width, m_height, m_depth, m_tuplesize, m_count;
  
  unsigned int m_params;

  GlTextureStorage(const GlTextureStorage&);
  GlTextureStorage& operator=(const GlTextureStorage&);
  
  // TODO: Mipmapping?
};

typedef SH::ShPointer<GlTextureStorage> GlTextureStoragePtr;

}

#endif
