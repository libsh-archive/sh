// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "ShContext.hpp"
#include "GlTextures.hpp"
#include <sstream>
#include "GlTextureName.hpp"
#include "GlTextureStorage.hpp"

namespace shgl {

using namespace SH;

const unsigned int shGlTargets[] = {
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

const unsigned int shGlCubeMapTargets[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

ShCubeDirection glToShCubeDir(GLuint target)
{
  switch (target) {
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    return SH_CUBE_POS_X;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    return SH_CUBE_NEG_X;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    return SH_CUBE_POS_Y;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    return SH_CUBE_NEG_Y;
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    return SH_CUBE_POS_Z;
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    return SH_CUBE_NEG_Z;
  }
  return SH_CUBE_POS_X;
}

GLenum shGlInternalFormat(const ShTextureNodePtr& node)
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
  
  std::string exts(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

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
    SH_DEBUG_ERROR("Could not find appropriate texture format \n"
		   "Using default instead!");
    return node->size();
    break;
  }
  
  return formats[node->size() - 1];
}

GLenum shGlFormat(const ShTextureNodePtr& node)
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
  SH_DEBUG_ERROR("Invalid GL format for the texture.");
  return 0;
}

/* Returns glReadPixels/glTexImage type for a given value type 
 * and returns a value type for the temporary buffer
 * (or SH_VALUETYPE_END if we can read pixels directly into
 * the original buffer)*/
GLenum shGlType(ShValueType valueType, ShValueType &convertedType) {
  convertedType = SH_VALUETYPE_END;
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
      SH_DEBUG_ERROR("Unsupported value type to glReadPixel type conversion"); 
      break;
  }

  std::string exts(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
  if(valueType == SH_HALF) {
#ifndef __APPLE__
    if (exts.find("NV_half_float") != std::string::npos) {
      convertedType = SH_VALUETYPE_END; 
      result = GL_HALF_FLOAT_NV; 
    }
#else
    if (exts.find("APPLE_float_pixels") != std::string::npos) {
      convertedType = SH_VALUETYPE_END; 
      result = GL_HALF_APPLE; 
    }
#endif
  }
  SH_DEBUG_ASSERT(result != GL_NONE);
  return result;
}

struct StorageFinder {
  StorageFinder(const ShTextureNodePtr& node, bool ignoreTarget = false)
    : node(node), ignoreTarget(ignoreTarget)
  {
  }
  
  bool operator()(const ShStoragePtr& storage) const
  {
    GlTextureStoragePtr t = shref_dynamic_cast<GlTextureStorage>(storage);
    if (!t) {
      return false;
    }
    if (!ignoreTarget) {
      if (t->texName()->params() != node->traits()) return false;
      if (t->target() != shGlTargets[node->dims()]) return false;
    }
    if (t->width() != node->width()) return false;
    if (t->height() != node->height()) return false;
    if (t->depth() != node->depth()) return false;
    return true;
  }
  
  const ShTextureNodePtr& node;
  bool ignoreTarget;
};

GlTextures::GlTextures(void)
{
}

TextureStrategy* GlTextures::create(void)
{
  return new GlTextures;
}


void GlTextures::bindTexture(const ShTextureNodePtr& node, GLenum target)
{
  if (!node) return;

  if (!node->meta("opengl:texid").empty()) {
    SH_GL_CHECK_ERROR(glActiveTextureARB(target));
    GLuint name;
    std::istringstream is(node->meta("opengl:texid"));
    is >> name; // TODO: Check for errors
    SH_GL_CHECK_ERROR(glBindTexture(shGlTargets[node->dims()], name));
    return;
  } 

  int mipmap_levels = node->mipmap_levels();

  if (node->dims() == SH_TEXTURE_CUBE) {
    
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
        ShCubeDirection dir = glToShCubeDir(s->target());
        if (!node->memory(dir, 0)) {
	  shError(ShException("No memory associated with the cube map texture."));
	  return;
	}
        if (s->memory() != node->memory(dir, 0).object() || !StorageFinder(node, true)(s))
          break;
      }
      // If we got through the whole list, we've found a matching list.
      if (S == name->endStorages()) break;
    }
    
    if (I == GlTextureName::endNames()) {
      // Need to allocate new storages
      GlTextureNamePtr texname = new GlTextureName(GL_TEXTURE_CUBE_MAP);
      texname->params(node->traits());
      for (int i = 0; i < 6; i++) {
        ShCubeDirection dir = static_cast<ShCubeDirection>(i);
	if (!node->memory(dir, 0)) {
	  std::stringstream s;
	  s << "No memory for the cube map texture (direction = " << dir << ").";
	  shError(ShException(s.str()));
	  return;
	}
        GlTextureStoragePtr storage = new GlTextureStorage(node->memory(dir, 0).object(),
                                                           shGlCubeMapTargets[i],
                                                           shGlFormat(node),
                                                           shGlInternalFormat(node),
                                                           node->valueType(),
                                                           node->width(), node->height(),
                                                           node->depth(), node->size(),
                                                           node->count(), texname, 0);
        storage->sync();

	if (mipmap_levels > 1) {
	  if (!node->memory(dir, 1)) {
	    std::stringstream s;
	    s << "Automatically generated the " << mipmap_levels << " mipmap levels (dir = " << dir << ").";
	    SH_DEBUG_WARN(s.str());
	    node->build_mipmaps(dir);
	  }

	  int width = node->width();
	  int height = node->height();
	  for (int j=1; j < mipmap_levels; j++) {
	    if (!node->memory(dir, j)) {
	      std::stringstream s;
	      s << "No memory for the cube map texture at mipmap level " << j 
		<< " (dir = " << dir << ", nb levels = " << mipmap_levels << ").";
	      shError(ShException(s.str()));
	    }
	    
	    width /= 2;
	    height /= 2;
	    int count = width * height * node->depth();
	    GlTextureStoragePtr mip_storage = new GlTextureStorage(node->memory(dir, j).object(),
								   shGlCubeMapTargets[i],
								   shGlFormat(node),
								   shGlInternalFormat(node),
								   node->valueType(),
								   width, height,
								   node->depth(), node->size(),
								   count, texname, j);
	    mip_storage->sync();
	  }
	}
      }

      SH_GL_CHECK_ERROR(glActiveTextureARB(target));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texname->value()));
      std::ostringstream os;
      os << texname->value();
      node->meta("opengl:alloc_texid", os.str());
    } else {
      // Just synchronize the storages
      GlTextureName::StorageList::const_iterator S;
      for (S = (*I)->beginStorages(); S != (*I)->endStorages(); ++S) {
        GlTextureStorage* s = dynamic_cast<GlTextureStorage*>(*S);
        if (!s) continue;
        s->sync();
      }
      SH_GL_CHECK_ERROR(glActiveTextureARB(target));
      SH_GL_CHECK_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, (*I)->value()));
      std::ostringstream os;
      os << (*I)->value();
      node->meta("opengl:alloc_texid", os.str());
    }
  } else {
    if (!node->memory(0)) {
      shError(ShException("No memory associated with the texture."));
      return;
    }

    if ((mipmap_levels > 1) && (!node->memory(1))) {
      std::stringstream s;
      s << "Automatically generated the " << mipmap_levels << " mipmap levels.";
      SH_DEBUG_WARN(s.str());
      node->build_mipmaps();
    }

    StorageFinder finder(node);
    GlTextureStoragePtr storage =
      shref_dynamic_cast<GlTextureStorage>(node->memory(0)->findStorage("opengl:texture", finder));
    if (!storage) {
      GlTextureNamePtr name = new GlTextureName(shGlTargets[node->dims()]);
      storage = new GlTextureStorage(node->memory(0).object(),
                                     shGlTargets[node->dims()],
                                     shGlFormat(node),
                                     shGlInternalFormat(node),
                                     node->valueType(),
                                     node->width(), node->height(), 
                                     node->depth(), node->size(),
                                     node->count(), name, 0);
      if (mipmap_levels > 1) {
	storage->sync();
	
	int width = node->width();
	int height = node->height();
	for (int i=1; i < mipmap_levels; i++) {
	  if (!node->memory(i)) {
	    std::stringstream s;
	    s << "No memory for the texture at mipmap level " << i << " (nb levels = " << mipmap_levels << ").";
	    shError(ShException(s.str()));
	  }

	  width /= 2;
	  height /= 2;
	  int count = width * height * node->depth();
	  GlTextureStoragePtr mip_storage = new GlTextureStorage(node->memory(i).object(),
								 shGlTargets[node->dims()],
								 shGlFormat(node),
								 shGlInternalFormat(node),
								 node->valueType(),
								 width, height,
								 node->depth(), node->size(),
								 count, name, i);
	  mip_storage->sync();
	}
      }

      name->params(node->traits());
    }

    SH_GL_CHECK_ERROR(glActiveTextureARB(target));
    storage->sync();
    SH_GL_CHECK_ERROR(glBindTexture(shGlTargets[node->dims()], storage->name()));

    std::ostringstream os;
    os << storage->name();
    node->meta("opengl:alloc_texid", os.str());
  }
}

}
