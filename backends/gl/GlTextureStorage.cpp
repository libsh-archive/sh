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
#include "GlTextureStorage.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"

namespace {

// @todo type get rid of this when there's a proper framework
// for manipulating data in memory 

// converts an array of type from to type to using the default cast
// (should be good enough for now. may want to hook in Sh casting
// functions if necessary...)
//
// @return newly allocated array of type To with size elements 
template<typename D, typename S>
D* convertData(const S* originalData, int size)
{
  D* result = new D[size];

  for(int i = 0; i < size; ++i) {
    result[i] = static_cast<D>(originalData[i]);
  }
  return result;
}

}

namespace shgl {

using namespace SH;

GlTextureStorage::GlTextureStorage(int context,
                                   ShMemory* memory, GLenum target,
                                   GLenum format, GLint internalFormat,
                                   int typeIndex, 
                                   int width, int height, int depth, int tuplesize,
                                   GlTextureNamePtr name)
  : ShStorage(memory), m_context(context),
    m_name(name), m_target(target), m_format(format), 
    m_internalFormat(internalFormat),
    m_typeIndex(typeIndex), 
    m_width(width), m_height(height), m_depth(depth), m_tuplesize(tuplesize)
{
  m_name->addStorage(this);
}

GlTextureStorage::~GlTextureStorage()
{
  m_name->removeStorage(this);
}


class HostGlTextureTransfer : public ShTransfer {
  HostGlTextureTransfer()
    : ShTransfer("host", "opengl:texture")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    const ShHostStorage* host = dynamic_cast<const ShHostStorage*>(from);
    GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texture->texName());

    GLenum type = GL_FLOAT;

    int typeIndex = texture->typeIndex(); 
    int count = texture->count();
    // @todo type half-float/frac types 
    ShPointer<ShDataVariant<float> > dataVariant; 

    if(typeIndex != shTypeIndex<float>()) {
      const ShVariantFactory* variantFactory = shTypeInfo(typeIndex)->variantFactory();

      // @todo type shouldn't really use this trickery...
      // But knowing ShDataVariant internals, it will not change host->data() 
      ShVariantCPtr hostVariant = 
        variantFactory->generate(const_cast<void *>(host->data()), count, false);

      dataVariant = new ShDataVariant<float>(count); 
      dataVariant->set(hostVariant);
    } else {
      dataVariant = new ShDataVariant<float>(const_cast<void *>(host->data()),
          count, false);
    }

    // @todo type 
    // change type if we have a fractional/half 

    switch(texture->target()) {
    case GL_TEXTURE_1D:
      SH_GL_CHECK_ERROR(glTexImage1D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), 0, texture->format(), type,
                                     dataVariant->begin()));
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_RECTANGLE_NV:
      SH_GL_CHECK_ERROR(glTexImage2D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), texture->height(), 0, texture->format(),
                                     type, dataVariant->begin()));
      break;
    case GL_TEXTURE_3D:
      SH_GL_CHECK_ERROR(glTexImage3D(texture->target(), 0,
                                     texture->internalFormat(),
                                     texture->width(), texture->height(),
                                     texture->depth(), 0, texture->format(),
                                     type, dataVariant->begin()));
      break;
    default:
      SH_DEBUG_WARN("Texture target " << texture->target() << " not handled by GL backend");
      break;
    }


    return true;
  }
  
  int cost()
  {
    // Texture downloads are expensive!
    return 100;
  }

  static HostGlTextureTransfer* instance;
};

HostGlTextureTransfer* HostGlTextureTransfer::instance = new HostGlTextureTransfer();

}
