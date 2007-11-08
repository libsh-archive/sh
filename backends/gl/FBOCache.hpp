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
#ifndef SHFBOCACHE_HPP
#define SHFBOCACHE_HPP

#include "GlTextureStorage.hpp"

namespace shgl {

class FBOCache {
public:

  static FBOCache *instance();

  /* Bind new framebuffer and add to top of stack 
   * (Actually, it looks like this just indicates we need a new FBO the 
   * next time someone starts to bind textures) */  
  void bindFramebuffer();

  /* Unbind framebuffer, and return to last FBO on stack */ 
  void unbindFramebuffer();
  void check();

  void bindTexture(GlTextureStoragePtr storage, 
                   GLenum attachment, GLint zoffset);
  GLenum bindTexture(GlTextureStoragePtr storage, GLint zoffset);

private:
  FBOCache();
  void updateLRU();
  /* Binds the given texture to a color attachment on the current FBO */
  void fbTexture(GlTextureStoragePtr storage, GLenum attachment, GLint zoffset);

  /* Unbinds the given texture from an attachment on the current FBO */  
  void fbUnbindTexture(GlTextureStoragePtr storage, GLenum attachment);
  
  static const int NUM_COLOR_ATTACHMENTS=16;
  struct FBO {
    GLuint id;
    GlTextureStoragePtr m_attachment[NUM_COLOR_ATTACHMENTS];
    GLint m_zoffset[NUM_COLOR_ATTACHMENTS];

    // Getting rid of "unused" attachments, since conflicts in dimensions there 
    // seem to be screwing up framebuffer completeness
    //bool m_used[NUM_COLOR_ATTACHMENTS];
    FBO *m_lru_next, *m_lru_prev;
  };
  
  /* hmm - seems reusing FBOs is not always working - some state is still mucked up */
  static const int NUM_FBOS = 8;
  FBO m_fbo[NUM_FBOS];
  
  FBO *m_lru;
  GLint m_original_fbo;
  
  std::stack<FBO *> m_fbo_stack;
  
  static FBOCache *m_instance;

  /* Something weird's going on when FBOs from the cache are bein reused.
   * Wrapping some of the main functions to trace during debugging */
  void GenFb(GLsizei n, GLuint *framebuffers);
  void BindFb(GLuint framebuffer);
  GLint GetFbBinding();

  /* Prints stats about bindings in current framebuffer */
  void PrintFbInfo();

  /* Clears out any texture attachments in the LRU FBO and prepares it again for use, binding it */ 
  FBO* RecycleLRU();

  /* Binds and clears an FBO of all texture attachments */
  void ClearFBO(FBO* fbo);

};

}

#endif
