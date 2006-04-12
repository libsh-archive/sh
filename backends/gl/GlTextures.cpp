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
#include "Context.hpp"
#include "GlTextures.hpp"
#include <sstream>
#include "GlTextureName.hpp"
#include "GlTextureStorage.hpp"
#include "FBOCache.hpp"

namespace shgl {

using namespace SH;

const unsigned int glTargets[] = {
  GL_TEXTURE_1D,
  GL_TEXTURE_2D,
#if defined ( __APPLE__ )
  GL_TEXTURE_RECTANGLE_EXT,
#else
  GL_TEXTURE_RECTANGLE_NV,
#endif
  GL_TEXTURE_3D,
  GL_TEXTURE_CUBE_MAP,
};

const unsigned int glCubeMapTargets[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

CubeDirection glToShCubeDir(GLuint target)
{
  switch (target) {
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    return CUBE_POS_X;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    return CUBE_NEG_X;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    return CUBE_POS_Y;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    return CUBE_NEG_Y;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    return CUBE_POS_Z;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    return CUBE_NEG_Z;
  }
  return CUBE_POS_X;
}

GLenum glInternalFormat(const TextureNodePtr& node, bool forceRGB)
{
  GLenum byteformats[4] = {GL_LUMINANCE8, GL_LUMINANCE8_ALPHA8, GL_RGB8, GL_RGBA8}; 
  GLenum shortformats[4] = {GL_LUMINANCE16, GL_LUMINANCE16_ALPHA16, GL_RGB16, GL_RGBA16}; 

#ifndef __APPLE__
  GLenum halfformats_nv[4] = {GL_FLOAT_R16_NV, GL_FLOAT_RGBA16_NV, GL_FLOAT_RGB16_NV, GL_FLOAT_RGBA16_NV};
  GLenum fpformats_nv[4] = {GL_FLOAT_R32_NV, GL_FLOAT_RGBA32_NV, GL_FLOAT_RGB32_NV, GL_FLOAT_RGBA32_NV};
  GLenum halfformats_ati[4] = {GL_LUMINANCE_FLOAT16_ATI,
                             GL_LUMINANCE_ALPHA_FLOAT16_ATI,
                             GL_RGB_FLOAT16_ATI,
                             GL_RGBA_FLOAT16_ATI};

  GLenum fpformats_ati[4] = {GL_LUMINANCE_FLOAT32_ATI,
                             GL_LUMINANCE_ALPHA_FLOAT32_ATI,
                             GL_RGB_FLOAT32_ATI,
                             GL_RGBA_FLOAT32_ATI};
#else
  GLenum halfformats_apple[4] = {GL_LUMINANCE_FLOAT16_APPLE,
                                 GL_LUMINANCE_ALPHA_FLOAT16_APPLE,
                                 GL_RGB_FLOAT16_APPLE,
                                 GL_RGBA_FLOAT16_APPLE};
  GLenum fpformats_apple[4] = {GL_LUMINANCE_FLOAT32_APPLE,
                               GL_LUMINANCE_ALPHA_FLOAT32_APPLE,
                               GL_RGB_FLOAT32_APPLE,
                               GL_RGBA_FLOAT32_APPLE};
#endif
  GLenum* formats = 0;
  
  const char* exts_ptr = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  std::string exts;
  if (exts_ptr) {
    exts = std::string(exts_ptr);
  } else {
    DEBUG_WARN("Could not read GL_EXTENSIONS.  Something wrong with your driver installation?");
  }

  GLenum* float_formats = 0;
  GLenum* half_formats = 0;
  
#ifndef __APPLE__
  // note that right now 1/2/3D float textures on NV are only supported through
  // the ATI extension, so we only use nv for RECT. 
  bool float_nv = (exts.find("NV_float_buffer") != std::string::npos) && 
                  (node->dims() == SH_TEXTURE_RECT);

  bool float_ati = (exts.find("ATI_texture_float") != std::string::npos);

  if (float_ati) {
    float_formats = fpformats_ati;
    half_formats = halfformats_ati;
  } else if (float_nv) {
    float_formats = fpformats_nv;
    half_formats = halfformats_nv;
  } else {
    DEBUG_WARN("Float textures are not available for this texture size.  Using byte textures instead. ");
    float_formats = byteformats;
    half_formats = byteformats;
  }
#else
  bool float_apple = (exts.find("APPLE_float_pixels") != std::string::npos);

  if (float_apple) {
    float_formats = fpformats_apple;
    half_formats = halfformats_apple;
  }
#endif
  
  if (node->size() < 0 || node->size() > 4) return 0;
  // @todo type
  // handle fractional types
  // right now all available formats - double, float, signed ints, unsigned ints should
  // be stored internally as float if possible

  switch(node->valueType()) {
  case SH_DOUBLE:
  case SH_FLOAT:        
  case SH_INT: 
  case SH_UINT:
    formats = float_formats;
    break;
  case SH_HALF:
  case SH_SHORT: 
  case SH_BYTE:
  case SH_USHORT:
  case SH_UBYTE:
    formats = half_formats;
    break;
    
  case SH_FINT:
  case SH_FSHORT:
  case SH_FUINT:
  case SH_FUSHORT:
    formats = shortformats;
    break;
    
  case SH_FBYTE:
  case SH_FUBYTE:
    formats = byteformats;
    break;
  default:
    DEBUG_ERROR("Could not find appropriate texture format \n"
		   "Using default instead!");
    return node->size();
    break;
  }
  if (forceRGB) {
#ifndef __APPLE__
    // The NV_float_buffer extension is only used on older NVIDIA hardware
    // (the one that doesn't support ATI_texture_float extension) very old
    // such hardware (FX5200 in particular) supports writing only to RGBA
    // so we always force RGBA here as the gcd.
    if (formats == fpformats_nv || formats == halfformats_nv) {
      return formats[3];
    }
#endif
    if (node->size() <= 2) {
      return formats[node->size() - 1 + 2];
    }
  }

  return formats[node->size() - 1];
}

GLenum glFormat(const TextureNodePtr& node)
{
  switch (node->size()) {
  case 1:
    return GL_LUMINANCE;
  case 2:
    return GL_LUMINANCE_ALPHA;
  case 3:
    return GL_RGB;
  case 4:
    return GL_RGBA;
  default:
    break;
  }
  DEBUG_ERROR("Invalid GL format for the texture.");
  return 0;
}

/* Returns glReadPixels/glTexImage type for a given value type 
 * and returns a value type for the temporary buffer
 * (or VALUETYPE_END if we can read pixels directly into
 * the original buffer)*/
GLenum glType(ValueType valueType, ValueType &convertedType) {
  convertedType = VALUETYPE_END;
  GLenum result = GL_NONE;
  switch(valueType) {
    case SH_DOUBLE:
    case SH_HALF:
    case SH_INT: 
    case SH_SHORT: 
    case SH_BYTE:
    case SH_UINT:
    case SH_USHORT:
    case SH_UBYTE:
      convertedType = SH_FLOAT;
    case SH_FLOAT:        result = GL_FLOAT; break;


    case SH_FINT:     result = GL_INT; break;
    case SH_FSHORT:   result = GL_SHORT; break;
    case SH_FBYTE:    result = GL_BYTE; break;
    case SH_FUINT:    result = GL_UNSIGNED_INT; break;
    case SH_FUSHORT:  result = GL_UNSIGNED_SHORT; break;
    case SH_FUBYTE:   result = GL_UNSIGNED_BYTE; break;

    default:
      DEBUG_ERROR("Unsupported value type to glReadPixel type conversion"); 
      break;
  }

  const char* exts_ptr = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  std::string exts;
  if (exts_ptr) {
    exts = std::string(exts_ptr);
  } else {
    DEBUG_WARN("Could not read GL_EXTENSIONS.  Something wrong with your driver installation?");
  }

  if(valueType == SH_HALF) {
#ifndef __APPLE__
    if (exts.find("NV_half_float") != std::string::npos) {
      convertedType = VALUETYPE_END; 
      result = GL_HALF_FLOAT_NV; 
    }
#else
    if (exts.find("APPLE_float_pixels") != std::string::npos) {
      convertedType = VALUETYPE_END; 
      result = GL_HALF_APPLE; 
    }
#endif
  }
  DEBUG_ASSERT(result != GL_NONE);
  return result;
}


GlTextures::ActiveTexture::ActiveTexture(GLenum texture_unit)
  : texture_unit(texture_unit)
{
  // Save old texture unit
  GLint temp;
  GL_CHECK_ERROR(glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &temp));
  last_unit = temp;

  // Set new one
  GL_CHECK_ERROR(glActiveTextureARB(texture_unit));
}

GlTextures::ActiveTexture::~ActiveTexture()
{
  // Restore old texture unit
  GL_CHECK_ERROR(glActiveTextureARB(last_unit));
}


struct StorageFinder {

  enum LookFor {
    READ_CLEAN,
    READ_ANY,
    WRITE
  };

  // Can optionally provide custom dimensions, otherwise node dimentions are used
  StorageFinder(const TextureNodePtr& node, LookFor lookFor, bool ignoreTarget = false,
                int width = -1, int height = -1, int depth = -1)
    : node(node), m_width(width), m_height(height), m_depth(depth),
      m_lookFor(lookFor), ignoreTarget(ignoreTarget), m_nr_clean(0)
  {
    if (m_width  < 0) m_width  = node->width();
    if (m_height < 0) m_height = node->height();
    if (m_depth  < 0) m_depth  = node->depth();
  }

  // assignment operator could not be generated
  StorageFinder& operator=(StorageFinder const&);
  
  bool operator()(const StoragePtr& storage)
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) {
      return false;
    }
    if (!ignoreTarget) {
      if (t->target() != glTargets[node->dims()]) return false;
      // We copy traits if they differ, so they need not match
    }
    if (t->width()  != m_width ) return false;
    if (t->height() != m_height) return false;
    if (t->depth()  != m_depth ) return false;
    
    if (m_lookFor == READ_CLEAN && !t->write() &&
        t->memory()->timestamp() == t->timestamp()) {
      return true;
    }
    if (m_lookFor == READ_ANY && !t->write()) {
      return true;
    }
    // either a dirty page or the second clean one
    if (m_lookFor == WRITE) {
      // only RGB textures can be rendered to
      if (!t->internalFormatRGB()) {
        return false;
      }
      if (t->memory()->timestamp() != t->timestamp()) {
        return true;
      }
      else {
        ++m_nr_clean;
        if (m_nr_clean == 2) return true;
      }
    }
    return false;
  }
  
  const TextureNodePtr& node;
  int m_width;
  int m_height;
  int m_depth;
  LookFor m_lookFor;
  bool ignoreTarget;
  int m_nr_clean;
};

GlTextures::GlTextures(void)
{
}

TextureStrategy* GlTextures::create(void)
{
  return new GlTextures;
}


void GlTextures::bindTexture(const TextureNodePtr& node, GLenum target, bool write)
{
  if (!node) return;

  if (!node->meta("opengl:texid").empty()) {
    ActiveTexture active_texture(target);
    GLuint name;
    std::istringstream is(node->meta("opengl:texid"));
    is >> name; // TODO: Check for errors
    GL_CHECK_ERROR(glBindTexture(glTargets[node->dims()], name));
    return;
  } 

  int mipmap_levels = node->mipmap_levels();
  if (mipmap_levels > 1 && write) {
    error(Exception("Cannot render to a mipmapped texture."));
    return;
  }

  if (node->dims() == SH_TEXTURE_CUBE) {
    
    if (write) {
      // Actually, maybe it could be done
	    error(Exception("Cannot render to cube map texture."));
	    return;
    }

    // Look for a cubemap that happens to have just the right storages    
    GlTextureName::NameList::const_iterator I;
    for (I = GlTextureName::beginNames(); I != GlTextureName::endNames(); ++I) {
      const GlTextureName* name = *I;
      if (name->target() != GL_TEXTURE_CUBE_MAP) continue;
      if (name->params() != node->traits()) continue;
      
      GlTextureName::StorageList::const_iterator S;
      for (S = name->beginStorages(); S != name->endStorages(); ++S) {
        GlTextureStorage* s = dynamic_cast<GlTextureStorage*>(*S);
        if (!s) continue;
        CubeDirection dir = glToShCubeDir(s->target());
        if (!node->memory(dir, 0)) {
          error(Exception("No memory associated with the cube map texture."));
          return;
        }
        if (s->mipmap_level() == 0) {
          if (s->memory() != node->memory(dir, 0).object() ||
              !StorageFinder(node, StorageFinder::READ_ANY, true)(s)) {
            break;
          }
        }
      }
      // If we got through the whole list, we've found a matching list.
      if (S == name->endStorages()) break;
    }
    
    if (I == GlTextureName::endNames()) {
      // Need to allocate new storages
      GlTextureNamePtr texname = new GlTextureName(GL_TEXTURE_CUBE_MAP);

      std::ostringstream os;
      os << texname->value();
      node->meta("opengl:alloc_texid", os.str());

      texname->params(node->traits());

      for (int i = 0; i < 6; i++) {
        CubeDirection dir = static_cast<CubeDirection>(i);
        if (!node->memory(dir, 0)) {
          std::stringstream s;
          s << "No memory for the cube map texture (direction = " << dir << ").";
          error(Exception(s.str()));
          return;
        }
        GlTextureStoragePtr storage = new GlTextureStorage(node->memory(dir, 0).object(),
                                                           glCubeMapTargets[i],
                                                           glFormat(node),
                                                           glInternalFormat(node, write),
                                                           node->valueType(),
                                                           node->width(), node->height(),
                                                           node->depth(), node->size(),
                                                           texname, 0,
                                                           write || node->size() >= 3);
        storage->sync();

        if (mipmap_levels > 1) {
          if (node->build_mipmaps(dir)) {
            std::stringstream s;
            s << "Automatically generated the " << mipmap_levels << " mipmap levels (dir = " << dir << ").";
            DEBUG_WARN(s.str());
          }

          int width = node->width();
          int height = node->height();
          for (int j=1; j < mipmap_levels; j++) {
            if (!node->memory(dir, j)) {
              std::stringstream s;
              s << "No memory for the cube map texture at mipmap level " << j 
                << " (dir = " << dir << ", nb levels = " << mipmap_levels << ").";
              error(Exception(s.str()));
            }
        
            width /= 2;
            height /= 2;
            GlTextureStoragePtr mip_storage = new GlTextureStorage(node->memory(dir, j).object(),
                                                                   glCubeMapTargets[i],
                                                                   glFormat(node),
                                                                   glInternalFormat(node, write),
                                                                   node->valueType(),
                                                                   width, height,
                                                                   node->depth(), node->size(),
                                                                   texname, j,
                                                                   write || node->size() >= 3);
            // TODO: this should go away, needs to be done every time
            mip_storage->sync();
          }
        }
      }

      ActiveTexture active_texture(target);
      GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texname->value()));      
    } else {
      // Just synchronize the storages
      GlTextureName::StorageList::const_iterator S;
      for (S = (*I)->beginStorages(); S != (*I)->endStorages(); ++S) {
        GlTextureStorage* s = dynamic_cast<GlTextureStorage*>(*S);
        if (!s) continue;
        s->sync();
      }
      ActiveTexture active_texture(target);
      GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, (*I)->value()));
      std::ostringstream os;
      os << (*I)->value();
      node->meta("opengl:alloc_texid", os.str());
    }
  } else {
    if (!node->memory(0)) {
      error(Exception("No memory associated with the texture."));
      return;
    }

    if (node->build_mipmaps()) {
      std::stringstream s;
      s << "Automatically generated the " << mipmap_levels << " mipmap levels.";
      DEBUG_WARN(s.str());
    }
  
    int width = node->width();
    int height = node->height();
    GlTextureNamePtr name = 0;
    for (int i = 0; i < mipmap_levels; ++i, width /= 2, height /= 2) {    
      if (!node->memory(i)) {
        std::stringstream s;
        s << "No memory for the texture at mipmap level " << i << " (nb levels = " << mipmap_levels << ").";
        error(Exception(s.str()));
      }
      
      StorageFinder finder(node, (write ? StorageFinder::WRITE : StorageFinder::READ_CLEAN),
                           false, width, height);
      GlTextureStoragePtr storage =
        shref_dynamic_cast<GlTextureStorage>(node->memory(i)->findStorage("opengl:texture", finder));
      if (!storage && !write) {
        // Couldn't find a clean storage, find a dirty one and sync it
        StorageFinder finder(node, StorageFinder::READ_ANY, false, width, height);
        storage =
          shref_dynamic_cast<GlTextureStorage>(node->memory(i)->findStorage("opengl:texture", finder));
      }
      
      // First time through the loop (while dealing with the base texture)
      // get the texture name, or create one if there is no storage for 
      // the base texture yet.
      if (!name) {
        if (storage) {
          name = storage->texName();
        } else {
          name = new GlTextureName(glTargets[node->dims()]);

          std::ostringstream os;
          os << name->value();
          node->meta("opengl:alloc_texid", os.str());
        }

        // Copy traits (interpolation, etc) if they have changed
        if (name->params() != node->traits()) {
          name->params(node->traits());
        }
      }
    
      if (!storage) {
        storage = new GlTextureStorage(node->memory(i).object(),
                                       glTargets[node->dims()],
                                       glFormat(node),
                                       glInternalFormat(node, write),
                                       node->valueType(),
                                       width, height, node->depth(), node->size(),
                                       name, i, write || node->size() >= 3);
        if (write) {
          storage->initTexture();
        }
      }
      if (!write) {
        storage->sync();
      }
    }
    DEBUG_ASSERT(name);
    
    if (write) {
      GlTextureStorage* storage = 0;
      GlTextureName::StorageList::const_iterator I;
      for (I = name->beginStorages(); I != name->endStorages(); ++I) {
        storage = dynamic_cast<GlTextureStorage*>(*I);
        // TODO: write to different mipmap level?
        if (storage && storage->mipmap_level() == 0)
          break;
      }
      DEBUG_ASSERT(I != name->endStorages());

      FBOCache::instance()->bindTexture(storage, target, 0);
      // TODO use dirtyall when the full texture is written to
      storage->dirty();
      storage->write(true);
    }
    else {
      ActiveTexture active_texture(target);
      GL_CHECK_ERROR(glBindTexture(glTargets[node->dims()], name->value()));
    }    
  }
}

}
