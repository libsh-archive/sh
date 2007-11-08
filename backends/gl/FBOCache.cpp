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
#include "FBOCache.hpp"

namespace shgl {

#define FBO_DEBUG false 

#define FBO_PRINT(x) { if(FBO_DEBUG) { SH_DEBUG_PRINT(x); } }


FBOCache *FBOCache::m_instance;

FBOCache::FBOCache()
{
  for (int i = 0; i < NUM_FBOS; ++i) {
    m_fbo[i].m_lru_next = &m_fbo[(i+1) % NUM_FBOS];
    m_fbo[i].m_lru_prev = &m_fbo[(i+NUM_FBOS-1) % NUM_FBOS];
    GenFb(1, &m_fbo[i].id);
  }
  m_lru = &m_fbo[0];
}

FBOCache *FBOCache::instance()
{
  if (!m_instance) {
    m_instance = new FBOCache();
  }
  return m_instance;
}

void FBOCache::bindFramebuffer()
{
  if (m_fbo_stack.empty()) {
    m_original_fbo = GetFbBinding();
  }
  
  m_fbo_stack.push(0);
}

void FBOCache::unbindFramebuffer()
{
  m_fbo_stack.pop();
  if (m_fbo_stack.empty()) {
    BindFb(m_original_fbo);
  }
  else {
    BindFb(m_fbo_stack.top()->id);
  }
}

void FBOCache::bindTexture(GlTextureStoragePtr storage, 
                           GLenum attachment, GLint zoffset)
{
  SH_DEBUG_ASSERT(!m_fbo_stack.empty());
  // If we aren't already working with an FBO, either find one with the
  // storage in the right place, or make a new one
  if (!m_fbo_stack.top()) {
    // First look for FBO with the storage in the right place
    for (int i = 0; i < NUM_FBOS; ++i) {
      if (m_fbo[i].m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] == storage) {
        m_fbo_stack.top() = &m_fbo[i];
        BindFb(m_fbo_stack.top()->id);
        break;
      }
    }
    // no such FBO exists, trash the LRU one
    if (!m_fbo_stack.top()) {
      m_fbo_stack.top() = RecycleLRU(); 
    }
  }
  // If we are already working with an FBO, make sure it has the storage in
  // the right place, or make a new FBO otherwise
  else {
    if (m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] &&
        m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] != storage) {
      FBO *previous = m_fbo_stack.top();
      m_fbo_stack.top() = RecycleLRU(); 
      SH_DEBUG_ASSERT(m_fbo_stack.top() != previous);

      for (int i = 0; i < NUM_COLOR_ATTACHMENTS; ++i) {
        if (previous->m_attachment[i]) {
          fbTexture(previous->m_attachment[i], 
                    GL_COLOR_ATTACHMENT0_EXT + i, 
                    previous->m_zoffset[i]);
        } 
      }
    }
  }
  updateLRU();    
  
  fbTexture(storage, attachment, zoffset);
}

GLenum FBOCache::bindTexture(GlTextureStoragePtr storage, GLint zoffset)
{
  SH_DEBUG_ASSERT(!m_fbo_stack.top());
  
  int attachment = 0;
  for (int i = 0; i < NUM_FBOS && !m_fbo_stack.top(); ++i) {
    // Ideally, we'd look through all the attachments of each framebuffer
    // but since the program for copying textures is hardcoded to render
    // into the first color target (there's no way to programatically 
    // change that) we only look at the first attachment
    //for (attachment = 0; attachment < NUM_COLOR_ATTACHMENTS; ++attachment) {}
    for (attachment = 0; attachment < 1; ++attachment) {
      if (m_fbo[i].m_attachment[attachment] == storage) {
        m_fbo_stack.top() = &m_fbo[i];
        break;
      }
    }
  }
  if (!m_fbo_stack.top()) {
    m_fbo_stack.top() = RecycleLRU(); 
    attachment = 0;
  }
  updateLRU();
  BindFb(m_fbo_stack.top()->id);
    
  fbTexture(storage, GL_COLOR_ATTACHMENT0_EXT + attachment, zoffset);
  return GL_COLOR_ATTACHMENT0_EXT + attachment;
}


void FBOCache::updateLRU()
{
  // update LRU chain
  if (m_fbo_stack.top() == m_lru)
    m_lru = m_lru->m_lru_next;
  else {
    FBO *current = m_fbo_stack.top();
    current->m_lru_prev->m_lru_next = current->m_lru_next;
    current->m_lru_next->m_lru_prev = current->m_lru_prev;
    current->m_lru_prev = m_lru->m_lru_prev;
    current->m_lru_next = m_lru;
    current->m_lru_prev->m_lru_next = current;
    current->m_lru_next->m_lru_prev = current;
  }
}

void FBOCache::fbTexture(GlTextureStoragePtr storage,
                         GLenum attachment, GLint zoffset)
{
  SH_DEBUG_ASSERT(m_fbo_stack.top());
  if (m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] != storage) {
  
    m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] = storage;
    m_fbo_stack.top()->m_zoffset[attachment - GL_COLOR_ATTACHMENT0_EXT] = zoffset;

    FBO_PRINT("Attaching texture w=" << storage->width() 
               << " h=" << storage->height() << " d=" << storage->depth() 
               << " tuplesize=" << storage->tuplesize()
               << " internal format=" << std::hex <<  storage->internalFormat() << std::dec);
    switch (storage->target()) {
    case GL_TEXTURE_1D:
      FBO_PRINT("FramebufferTexture1D attachment=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
                << " target=" << std::hex << storage->target() << std::dec
                << " texid=" << storage->texName()->value()
                << " mipmap_level=" << storage->mipmap_level());
      SH_GL_CHECK_ERROR(glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, 
                                                  attachment,
                                                  storage->target(), 
                                                  storage->texName()->value(),
                                                  storage->mipmap_level()));
      break;
    
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_RECTANGLE_ARB:
      FBO_PRINT("FramebufferTexture2D attachment=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
                << " target=" << std::hex << storage->target() << std::dec
                << " texid=" << storage->texName()->value()
                << " mipmap_level=" << storage->mipmap_level());
      SH_GL_CHECK_ERROR(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                                  attachment,
                                                  storage->target(), 
                                                  storage->name(),
                                                  storage->mipmap_level()));
      break;

    case GL_TEXTURE_3D:
      FBO_PRINT("FramebufferTexture3D attachment=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
                << " target=" << std::hex << storage->target() << std::dec
                << " texid=" << storage->texName()->value()
                << " mipmap_level=" << storage->mipmap_level()
                << " zoffset=" << zoffset);
      SH_GL_CHECK_ERROR(glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, 
                                                  attachment,
                                                  storage->target(), 
                                                  storage->texName()->value(),
                                                  storage->mipmap_level(),
                                                  zoffset));
      break;
      
    default:
      SH_DEBUG_WARN("Cannot attach texture target " << storage->target() << " to an FBO.");
      break;
    }
  }
}

void FBOCache::fbUnbindTexture(GlTextureStoragePtr storage, GLenum attachment) 
{
  switch (storage->target()) {
  case GL_TEXTURE_1D:
    FBO_PRINT("FramebufferTexture1D detach=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
              << " target=" << std::hex << storage->target() << std::dec
              << " texid=" << storage->texName()->value()
              << " mipmap_level=" << storage->mipmap_level());
    SH_GL_CHECK_ERROR(glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, 
                                                attachment,
                                                storage->target(),  0, 0));
    break;
  
  case GL_TEXTURE_2D:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
  case GL_TEXTURE_RECTANGLE_ARB:
    FBO_PRINT("FramebufferTexture2D detach=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
              << " target=" << std::hex << storage->target() << std::dec
              << " texid=" << storage->texName()->value()
              << " mipmap_level=" << storage->mipmap_level());
    SH_GL_CHECK_ERROR(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                                attachment,
                                                storage->target(), 0, 0)); 
    break;

  case GL_TEXTURE_3D:
    FBO_PRINT("FramebufferTexture3D detach=" << (attachment - GL_COLOR_ATTACHMENT0_EXT)
              << " target=" << std::hex << storage->target() << std::dec
              << " texid=" << storage->texName()->value()
              << " mipmap_level=" << storage->mipmap_level());
    SH_GL_CHECK_ERROR(glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, 
                                                attachment,
                                                storage->target(), 0, 0, 0)); 
    break;
    
  default:
    SH_DEBUG_WARN("Cannot detach texture target " << storage->target() << " from an FBO.");
    break;
  }
}

void FBOCache::check()
{
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  char *error = 0;
  switch (status) {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    return;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_FORMATS";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    error = "FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    error = "FRAMEBUFFER_UNSUPPORTED";
    break;
  default:
    error = "Unknown error";
    break;
  }
  SH_DEBUG_PRINT("GL Framebuffer error " << error);
}

void FBOCache::GenFb(GLsizei n, GLuint *framebuffers) {
  SH_GL_CHECK_ERROR(glGenFramebuffersEXT(n, framebuffers)); 
  FBO_PRINT("Generated " << n << " framebuffers");
  for(int i = 0; i < n; ++i) {
    FBO_PRINT("  " << framebuffers[i]);
  }
}

void FBOCache::BindFb(GLuint framebuffer) {
  FBO_PRINT("Bind framebuffer " << framebuffer);
  SH_GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer)); 
}

GLint FBOCache::GetFbBinding() {
  GLint result;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &result));
  FBO_PRINT("Get framebuffer binding = " << result);
  return result;
}

void FBOCache::PrintFbInfo() {

}

FBOCache::FBO* FBOCache::RecycleLRU() {
  ClearFBO(m_lru);
  return m_lru;
}

void FBOCache::ClearFBO(FBO* fbo) {
  FBO_PRINT("Clearing FBO " << fbo->id);
  BindFb(fbo->id);
  for (int i = 0; i < NUM_COLOR_ATTACHMENTS; ++i) {
    if(fbo->m_attachment[i]) {
      fbUnbindTexture(fbo->m_attachment[i], i + GL_COLOR_ATTACHMENT0_EXT); 
    }
    fbo->m_attachment[i] = 0; 
    fbo->m_zoffset[i] = 0; 
  }
}


}
