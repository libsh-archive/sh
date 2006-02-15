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
#ifndef FBOCACHE_HPP
#define FBOCACHE_HPP

#include "GlTextureStorage.hpp"

namespace shgl {

class FBOCache {
public:

  static FBOCache *instance();

  void bindFramebuffer();
  void unbindFramebuffer();
  void check();

  void bindTexture(GlTextureStoragePtr storage, 
                   GLenum attachment, GLint zoffset = 0);
  GLenum bindTexture(GlTextureStoragePtr storage, GLint zoffset = 0);

private:
  FBOCache();
  void updateLRU();
  void fbTexture(GlTextureStoragePtr storage, GLenum attachment, GLint zoffset);
  
  struct FBO {
    GLuint id;
    GlTextureStoragePtr m_attachment[16];
    GLint m_zoffset[16];
    bool m_used[16];
    FBO *m_lru_next, *m_lru_prev;
  };
  
  static const int NUM_FBOS = 8;
  FBO m_fbo[NUM_FBOS];
  
  FBO *m_lru, *m_current;
  GLint m_previous;
  
  static FBOCache *m_instance;
};

}

#endif
