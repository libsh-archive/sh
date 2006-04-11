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

FBOCache *FBOCache::m_instance;

FBOCache::FBOCache()
{
  for (int i = 0; i < NUM_FBOS; ++i) {
    m_fbo[i].m_lru_next = &m_fbo[(i+1) % NUM_FBOS];
    m_fbo[i].m_lru_prev = &m_fbo[(i+NUM_FBOS-1) % NUM_FBOS];
    GL_CHECK_ERROR(glGenFramebuffersEXT(1, &m_fbo[i].id));
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
    GL_CHECK_ERROR(glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &m_original_fbo));
  }
  
  m_fbo_stack.push(0);
}

void FBOCache::unbindFramebuffer()
{
  m_fbo_stack.pop();
  if (m_fbo_stack.empty()) {
    GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_original_fbo))
  }
  else {
    GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 
                                           m_fbo_stack.top()->id));
  }
}

void FBOCache::bindTexture(GlTextureStoragePtr storage, 
                           GLenum attachment, GLint zoffset)
{
  DEBUG_ASSERT(!m_fbo_stack.empty());
  // If we aren't already working with an FBO, either find one with the
  // storage in the right place, or make a new one
  if (!m_fbo_stack.top()) {
    // First look for FBO with the storage in the right place
    for (int i = 0; i < NUM_FBOS; ++i) {
      if (m_fbo[i].m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] == storage) {
        m_fbo_stack.top() = &m_fbo[i];
        break;
      }
    }
    // no such FBO exists, trash the LRU one
    if (!m_fbo_stack.top()) {
      m_fbo_stack.top() = m_lru;
      for (int i = 0; i < 16; ++i)
        m_fbo_stack.top()->m_attachment[i] = 0;
    }
    
    // At this point, if we needed to restart with a different FBO, we
    // wouldn't have to copy any other attachments
    for (int i = 0; i < 16; ++i)
      m_fbo_stack.top()->m_used[i] = false;
    
    updateLRU();    
      
    GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 
                                           m_fbo_stack.top()->id));
  }
  // If we are already working with an FBO, make sure it has the storage in
  // the right place, or make a new FBO otherwise
  else {
    if (m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] &&
        m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] != storage) {
      FBO *previous = m_fbo_stack.top();
      m_fbo_stack.top() = m_lru;
      GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 
                                             m_fbo_stack.top()->id));

      for (int i = 0; i < NUM_FBOS; ++i) {
        if (previous->m_used[i]) {
          fbTexture(previous->m_attachment[i], 
                    GL_COLOR_ATTACHMENT0_EXT + i, 
                    previous->m_zoffset[i]);
          m_fbo_stack.top()->m_used[i] = true;
        }
        else {
          m_fbo_stack.top()->m_attachment[i] = 0;
          m_fbo_stack.top()->m_used[i] = false;
        }
      }
    }
  }
  
  fbTexture(storage, attachment, zoffset);
  m_fbo_stack.top()->m_used[attachment - GL_COLOR_ATTACHMENT0_EXT] = true;
}

GLenum FBOCache::bindTexture(GlTextureStoragePtr storage, GLint zoffset)
{
  DEBUG_ASSERT(!m_fbo_stack.top());
  
  int attachment = 0;
  for (int i = 0; i < NUM_FBOS && !m_fbo_stack.top(); ++i) {
    for (attachment = 0; attachment < 16; ++attachment) {
      if (m_fbo[i].m_attachment[attachment] == storage) {
        m_fbo_stack.top() = &m_fbo[i];
        break;
      }
    }
  }
  if (!m_fbo_stack.top()) {
    m_fbo_stack.top() = m_lru;
    for (int i = 0; i < 16; ++i)
      m_fbo_stack.top()->m_attachment[i] = 0;
    attachment = 0;
  }
  updateLRU();
  GL_CHECK_ERROR(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 
                                         m_fbo_stack.top()->id));
    
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
  DEBUG_ASSERT(m_fbo_stack.top());
  if (m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] != storage) {
  
    m_fbo_stack.top()->m_attachment[attachment - GL_COLOR_ATTACHMENT0_EXT] = storage;
    m_fbo_stack.top()->m_zoffset[attachment - GL_COLOR_ATTACHMENT0_EXT] = zoffset;

    switch (storage->target()) {
    case GL_TEXTURE_1D:
      GL_CHECK_ERROR(glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, 
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
      GL_CHECK_ERROR(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
                                                  attachment,
                                                  storage->target(), 
                                                  storage->name(),
                                                  storage->mipmap_level()));
      break;

    case GL_TEXTURE_3D:
      GL_CHECK_ERROR(glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, 
                                                  attachment,
                                                  storage->target(), 
                                                  storage->texName()->value(),
                                                  storage->mipmap_level(),
                                                  zoffset));
      break;
      
    default:
      DEBUG_WARN("Cannot attach texture target " << storage->target() << " to an FBO.");
      break;
    }
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
  default:
    error = "Unknown error";
    break;
  }
  DEBUG_PRINT("GL Framebuffer error " << error);
}

}
