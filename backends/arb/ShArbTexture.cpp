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
#include "ShArb.hpp"
#include <iostream>
#include <sstream>

#ifdef WIN32

#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

#else

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#undef GL_GLEXT_PROTOTYPES

#endif /* WIN32 */

#include "ShVariable.hpp"
#include "ShDebug.hpp"
#include "ShLinearAllocator.hpp"
#include "ShEnvironment.hpp"
#include "ShTextureNode.hpp"
#include "ShSyntax.hpp"

#define shGlActiveTextureARB glActiveTextureARB

namespace ShArb {

using namespace SH;

const unsigned int shGlCubeMapTargets[] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

const unsigned int shGlTextureType[] = {
  GL_TEXTURE_1D,
  GL_TEXTURE_2D,
  GL_TEXTURE_3D,
  GL_TEXTURE_CUBE_MAP,
  GL_TEXTURE_RECTANGLE_NV,
};

void ArbCode::loadTexture(ShTextureNodePtr texture) 
{
  if (!texture) return;
  
  RegMap::const_iterator texRegIterator = m_registers.find(texture);
  
  SH_DEBUG_ASSERT(texRegIterator != m_registers.end());
  
  const ArbReg& texReg = texRegIterator->second;
  
  shGlActiveTextureARB(GL_TEXTURE0 + texReg.index);
  
  glBindTexture(shGlTextureType[texture->dims()], texReg.bindingIndex);
  
  if (shGlTextureType[texture->dims()] != GL_TEXTURE_RECTANGLE_NV) {
    if (1) {
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    if (1) {
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(shGlTextureType[texture->dims()], GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
  }
    
  ShDataTextureNodePtr datatex = texture;
  if (datatex) {
    loadDataTexture(datatex);
    return;
  }

  ShCubeTextureNodePtr cubetex = texture;
  if (cubetex) {
    loadCubeTexture(cubetex);
    return;
  }
    
  SH_DEBUG_WARN(texture->name() << " is not a valid texture!");
}

void ArbCode::loadDataTexture(ShDataTextureNodePtr texture, unsigned int type)
{
  if (!type) {
    switch (texture->dims()) {
    case SH_TEXTURE_1D:
      type = GL_TEXTURE_1D;
      break;
    case SH_TEXTURE_2D:
      type = GL_TEXTURE_2D;
      break;
    case SH_TEXTURE_3D:
      type = GL_TEXTURE_3D;
      break;
    case SH_TEXTURE_RECT:
      type = GL_TEXTURE_RECTANGLE_NV;
      break;
    default:
      SH_DEBUG_ERROR("No type specified and no known type for this texture.");
      break;
    }
  }

  // TODO: Other types of textures.
  // TODO: Element Format
  // TODO: sampling/filtering
  // TODO: wrap/clamp
  unsigned int format;

  ShDataMemoryObjectPtr dataMem = texture->mem();
  if( dataMem ) { // 
    switch (texture->elements()) {
    case 1:
      format = GL_LUMINANCE;
      break;
    case 2:
      format = GL_LUMINANCE_ALPHA;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      format = 0;
      break;
    }
    int internal_format = texture->elements();
    if (type == GL_TEXTURE_RECTANGLE_NV) {
      if (texture->elements() >= 1 && texture->elements() <= 4) {
        // TODO: 16 bit formats?
        int nv_formats[4] = {GL_FLOAT_R32_NV, GL_FLOAT_RG32_NV,
                             GL_FLOAT_RGB32_NV, GL_FLOAT_RGBA32_NV};
        internal_format = nv_formats[texture->elements() - 1];
      } else {
        internal_format = 0;
      }
    };

    float* data = dataMem->data();
    switch(type) {
    case GL_TEXTURE_1D:
      glTexImage1D(type, 0, internal_format, texture->width(), 0, format, GL_FLOAT, data);
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_RECTANGLE_NV:
      glTexImage2D(type, 0, internal_format, texture->width(), texture->height(), 0, format,
                   GL_FLOAT, data);
      break;
    case GL_TEXTURE_3D:
      glTexImage3D(type, 0, internal_format, texture->width(), texture->height(), texture->depth(),
                   0, format, GL_FLOAT, data);
      break;
    default:
      SH_DEBUG_WARN("Texture type not handled by ARB backend");
      break;
    }
    delete [] data; // <-- ShMemoryObjects are just _ever_ so useful!
    
  } else {
    ShExternalMemoryObjectPtr extMem = texture->mem();
    if( extMem ) {
      extMem->attach();
    } else {
      SH_DEBUG_ERROR("No memory object/image loaded for texture"); 
    }
  } 

  int error = glGetError();
  if (error != GL_NO_ERROR) {
    SH_DEBUG_ERROR("Error loading texture: " << error);
    switch(error) {
    case GL_INVALID_ENUM:
      SH_DEBUG_ERROR("INVALID_ENUM");
      break;
    case GL_INVALID_VALUE:
      SH_DEBUG_ERROR("INVALID_VALUE");
      break;
    case GL_INVALID_OPERATION:
      SH_DEBUG_ERROR("INVALID_OPERATION");
      break;
    }
  }
}

void ArbCode::loadCubeTexture(ShCubeTextureNodePtr cube)
{
  for (int i = 0; i < 6; i++) {
    loadDataTexture(cube->face(static_cast<ShCubeDirection>(i)), shGlCubeMapTargets[i]);
  }
}

void ArbCode::allocTextures()
{
  
  for (ShProgramNode::VarList::const_iterator I = m_shader->textures.begin();
       I != m_shader->textures.end(); ++I) {
    ShTextureNodePtr node = *I;
    int binding, index;
    index = m_numTextures;
    glGenTextures(1, (GLuint*)&binding);
    m_registers[node] = ArbReg(SH_ARB_REG_TEXTURE, index);
    m_registers[node].bindingIndex = binding;
    m_numTextures++;
  }
}

}
