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
#ifndef GLTEXTURES_HPP
#define GLTEXTURES_HPP

#include "ShContext.hpp"
#include "GlBackend.hpp"
#include "GlTextureName.hpp"
#include "GlTextureStorage.hpp"
#include <sstream>
#include <cstring>

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

ShCubeDirection glToShCubeDir(GLuint target);

GLenum shGlInternalFormat(const ShTextureNodePtr& node);
GLenum shGlFormat(const ShTextureNodePtr& node);
GLenum shGlType(ShValueType valueType, ShValueType &convertedType);

struct StorageFinder {
  StorageFinder(const ShTextureNodePtr& node, bool writing, bool strict, bool ignoreTarget = false)
    : node(node), ignoreTarget(ignoreTarget), writing(writing), strict(strict)
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

    if(t->isWrite() && !writing) return false;

    if(strict && ((writing && (t->timestamp() == node->memory(0)->timestamp())) || (!writing && (t->timestamp() < node->memory(0)->timestamp()))) ) return false;
	    
    return true;
  }
  
  const ShTextureNodePtr& node;
  bool ignoreTarget;
  bool writing;
  bool strict;
};

class GlTextures : public TextureStrategy {
public:
  GlTextures(void);

  TextureStrategy* create(void);
  
  void bindTexture(const SH::ShTextureNodePtr& texture,
                   GLenum target);
};

}

#endif
