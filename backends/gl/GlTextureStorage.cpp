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
#include "GlTextures.hpp"

namespace shgl {

using namespace SH;

GlTextureStorage::GlTextureStorage(ShMemory* memory, GLenum target,
                                   GLenum format, GLint internalFormat,
                                   ShValueType valueType, 
                                   int width, int height, int depth, int tuplesize,
                                   int count, GlTextureNamePtr name)
  : ShStorage(memory),
    m_name(name), m_target(target), m_format(format), 
    m_internalFormat(internalFormat),
    m_valueType(valueType), 
    m_width(width), m_height(height), m_depth(depth), m_tuplesize(tuplesize), m_count(count)
{
  m_name->addStorage(this);

  init();
}


GlTextureStorage::~GlTextureStorage()
{
  m_name->removeStorage(this);
}

void GlTextureStorage::init()
{
  
  // Bind texture name for this scope.
  GlTextureName::Binding binding(this->texName());
  
  ShValueType valueType = this->valueType(); 
  
  GLenum type; 
  ShValueType convertedType;
  type = shGlType(valueType, convertedType);

  switch(this->target()) {
  case GL_TEXTURE_1D:
    SH_GL_CHECK_ERROR(glTexImage1D(this->target(), 0,
                                   this->internalFormat(),
                                   this->width(), 0, this->format(), type,
                                   NULL));
    break;
  case GL_TEXTURE_2D:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
  case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
  case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
  case GL_TEXTURE_RECTANGLE_NV:
    SH_GL_CHECK_ERROR(glTexImage2D(this->target(), 0,
                                   this->internalFormat(),
                                   this->width(), this->height(), 0, this->format(),
                                   type, NULL));
    break;
  case GL_TEXTURE_3D:
    SH_GL_CHECK_ERROR(glTexImage3D(this->target(), 0,
                                   this->internalFormat(),
                                   this->width(), this->height(), this->depth(),
                                   0, this->format(),
                                   type, NULL));
    break;
  default:
    SH_DEBUG_WARN("Texture target " << this->target() << " not handled by GL backend");
    break;
  }
}

GlTextureMemory::GlTextureMemory(const SH::ShTextureNodePtr& node)
  : m_storage(0)
{
  SH_DEBUG_ASSERT(node->dims() != SH_TEXTURE_CUBE);
  
  GlTextureNamePtr name = new GlTextureName(shGlTargets[node->dims()]);
  m_storage = new GlTextureStorage(this,
                                   shGlTargets[node->dims()],
                                   shGlFormat(node),
                                   shGlInternalFormat(node),
                                   node->valueType(),
                                   node->width(), node->height(), 
                                   node->depth(), node->size(),
                                   node->count(), name);
  m_storage->setTimestamp(0);
  name->params(node->traits());
}

class HostGlTextureTransfer : public ShTransfer {
  HostGlTextureTransfer()
    : ShTransfer("host", "opengl:texture")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    std::cerr << "host->opengl:texture" << std::endl; 
    const ShHostStorage* host = dynamic_cast<const ShHostStorage*>(from);
    GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texture->texName());

    ShValueType valueType = texture->valueType(); 
    int count = texture->count();
    int tuplesize = texture->tuplesize();

    GLenum type; 
    ShValueType convertedType;
    type = shGlType(valueType, convertedType);

    ShVariantPtr dataVariant; 
    // @todo a little hackish...but we promise host->data() will not change... 
    ShVariantPtr hostVariant = shVariantFactory(valueType, SH_MEM)->generate(
                                                                             const_cast<void *>(host->data()), count * tuplesize, false);

    if(convertedType != SH_VALUETYPE_END) {
      SH_DEBUG_WARN("ARB backend does not handle " << shValueTypeName(valueType) << " natively.  Converting to " << shValueTypeName(convertedType));
      dataVariant = shVariantFactory(convertedType, SH_MEM)->generate(count * tuplesize);
      dataVariant->set(hostVariant);
    } else {
      dataVariant = hostVariant; 
    }

    int width = texture->width();
    int height = texture->height();
    int depth = texture->depth();

    std::cerr << "   Target has " << count << "/" << width << "x" << height << "x" << depth << std::endl;
    
    // If the texture is not full, must not copy more than count
    bool full_copy = (count == (width * height * depth));

    switch(texture->target()) {
    case GL_TEXTURE_1D:
      SH_GL_CHECK_ERROR(glTexSubImage1D(texture->target(), 0, 0,
                                        (full_copy ? width : count), texture->format(), type,
                                        dataVariant->array()));
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
    case GL_TEXTURE_RECTANGLE_NV:
      if (full_copy) {
	SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), 0, 0, 0,
					  width, height, texture->format(),
					  type, dataVariant->array()));
      } else {
	int last_row_count = count % width;
	int full_rows_count = count - last_row_count;
	int full_rows_height = full_rows_count / width;
	SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), 0, 0, 0,
					  width, full_rows_height, texture->format(),
					  type, dataVariant->array()));
	int array_offset = full_rows_height * width * dataVariant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), 0, 0, full_rows_height,
					  last_row_count, 1, texture->format(),
					  type, static_cast<char*>(dataVariant->array()) + array_offset));
      }
      break;
    case GL_TEXTURE_3D:
      if (full_copy) {
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), 0, 0, 0,
					  0, width, height,
					  depth, texture->format(),
					  type, dataVariant->array()));
      } else {
	int last_surface_count = count % (width * height);
	int last_row_count = last_surface_count % width;
	int full_rows_count = last_surface_count - last_row_count;
	int full_rows_height = full_rows_count / width;
	int full_surfaces_count = count - last_surface_count;
	int full_surfaces_depth = full_surfaces_count / (width * height);
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), 0, 0, 0,
					  0, width, height,
					  full_surfaces_depth, texture->format(),
					  type, dataVariant->array()));
	int array_offset = full_surfaces_depth * width * height * dataVariant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), 0, 0, 0,
					  full_surfaces_depth, width, full_rows_height,
					  1, texture->format(),
					  type, static_cast<char*>(dataVariant->array()) + array_offset));
	array_offset += full_rows_height * width * dataVariant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), 0, 0, full_rows_height,
					  full_surfaces_depth, last_row_count, 1,
					  1, texture->format(),
					  type, static_cast<char*>(dataVariant->array()) + array_offset));
      }
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

class GlTextureHostTransfer : public ShTransfer {
  GlTextureHostTransfer()
    : ShTransfer("opengl:texture", "host")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    std::cerr << "Transferring opengl:texture to host" << std::endl;
    const GlTextureStorage* texture = dynamic_cast<const GlTextureStorage*>(from);
    ShHostStorage* host = dynamic_cast<ShHostStorage*>(to);

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texture->texName());

    ShValueType valueType = texture->valueType(); 
    int count = texture->count();
    int tuplesize = texture->tuplesize();

    GLenum type; 
    ShValueType convertedType;
    type = shGlType(valueType, convertedType);

    int width = texture->width();
    int height = texture->height();
    int depth = texture->depth();

    // If the texture is not full, must not copy more than count
    bool full_copy = (count == (width * height * depth));

    // TODO: Type conversion stuff.
    ShVariantPtr hostVariant = shVariantFactory(valueType,
                                                SH_MEM)->generate(host->data(), count * tuplesize, false);
    ShVariantPtr dataVariant; 
    if (!full_copy || convertedType != SH_VALUETYPE_END) {
      dataVariant = shVariantFactory((convertedType == SH_VALUETYPE_END ? valueType : convertedType),
                                     SH_MEM)->generate(count * tuplesize);
    } else {
      dataVariant = hostVariant; 
    }

    SH_GL_CHECK_ERROR(glGetTexImage(texture->target(), 0, texture->format(), type, dataVariant->array()));

    if (dataVariant != hostVariant) {
      hostVariant->set(dataVariant);
    }

    return true;
  }
  
  int cost()
  {
    // Texture uploads are REALLY expensive!
    return 1000;
  }

  static GlTextureHostTransfer* instance;
};

GlTextureHostTransfer* GlTextureHostTransfer::instance = new GlTextureHostTransfer();


}
