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
#include "GlTextureStorage.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"

namespace shgl {

using namespace SH;

GlTextureStorage::GlTextureStorage(ShMemory* memory, GLenum target,
                                   GLenum format, GLint internalFormat,
                                   ShValueType valueType, 
                                   int width, int height, int depth, int tuplesize,
                                   int count, GlTextureNamePtr name, GLint mipmap_level)
  : ShStorage(memory, valueType),
    m_name(name), m_target(target), m_format(format), 
    m_internalFormat(internalFormat),
    m_width(width), m_height(height), m_depth(depth), m_tuplesize(tuplesize), m_count(count),
    m_mipmap_level(mipmap_level)
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

    ShValueType host_type = host->value_type();
    ShValueType texture_type = texture->value_type();
    int count = texture->count();
    int tuplesize = texture->tuplesize();

    GLenum type;
    ShValueType converted_type;
    type = shGlType(texture_type, converted_type);

    ShVariantPtr data_variant;
    // @todo a little hackish...but we promise host->data() will not change... 
    ShVariantPtr host_variant = shVariantFactory(host_type, SH_MEM)->
      generate(count * tuplesize, const_cast<void*>(host->data()), false);
    SH_DEBUG_ASSERT(host_variant);

    if(converted_type != SH_VALUETYPE_END) {
      SH_DEBUG_WARN("GL backend does not handle " << shValueTypeName(texture_type) 
		    << " natively.  Converting to " << shValueTypeName(converted_type));
      texture->value_type(converted_type);
      data_variant = shVariantFactory(converted_type, SH_MEM)->generate(count * tuplesize);
      data_variant->set(host_variant);
    } else if (texture_type != host_type) {
      data_variant = shVariantFactory(texture_type, SH_MEM)->generate(count * tuplesize);
      data_variant->set(host_variant);
    } else {
      data_variant = host_variant;
    }

    int width = texture->width();
    int height = texture->height();
    int depth = texture->depth();

    // If the texture is not full, must not copy more than count
    bool full_copy = (count == (width * height * depth));

    switch(texture->target()) {
    case GL_TEXTURE_1D:
      if (full_copy) {
	SH_GL_CHECK_ERROR(glTexImage1D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, 0, texture->format(), type,
				       data_variant->array()));
      } else {
	SH_GL_CHECK_ERROR(glTexImage1D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, 0, texture->format(), type,
				       NULL));
	SH_GL_CHECK_ERROR(glTexSubImage1D(texture->target(), texture->mipmap_level(), 0,
					  count, texture->format(), type,
					  data_variant->array()));
      }
      break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
#if defined( __APPLE__ )
    case GL_TEXTURE_RECTANGLE_EXT:
#else
    case GL_TEXTURE_RECTANGLE_NV:
#endif
      if (full_copy) {
	SH_GL_CHECK_ERROR(glTexImage2D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, height, 0, texture->format(),
				       type, data_variant->array()));
      } else {
	SH_GL_CHECK_ERROR(glTexImage2D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, height, 0, texture->format(),
				       type, NULL));
	int last_row_count = count % width;
	int full_rows_count = count - last_row_count;
	int full_rows_height = full_rows_count / width;
	SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), texture->mipmap_level(), 0, 0,
					  width, full_rows_height, texture->format(),
					  type, data_variant->array()));
	int array_offset = full_rows_height * width * data_variant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), texture->mipmap_level(), 0, full_rows_height,
					  last_row_count, 1, texture->format(),
					  type, static_cast<char*>(data_variant->array()) + array_offset));
      }
      break;
    case GL_TEXTURE_3D:
      if (full_copy) {
	SH_GL_CHECK_ERROR(glTexImage3D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, height,
				       depth, 0, texture->format(),
				       type, data_variant->array()));
      } else {
	SH_GL_CHECK_ERROR(glTexImage3D(texture->target(), texture->mipmap_level(),
				       texture->internalFormat(),
				       width, height,
				       depth, 0, texture->format(),
				       type, NULL));
	int last_surface_count = count % (width * height);
	int last_row_count = last_surface_count % width;
	int full_rows_count = last_surface_count - last_row_count;
	int full_rows_height = full_rows_count / width;
	int full_surfaces_count = count - last_surface_count;
	int full_surfaces_depth = full_surfaces_count / (width * height);
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), texture->mipmap_level(), 0, 0,
					  0, width, height,
					  full_surfaces_depth, texture->format(),
					  type, data_variant->array()));
	int array_offset = full_surfaces_depth * width * height * data_variant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), texture->mipmap_level(), 0, 0,
					  full_surfaces_depth, width, full_rows_height,
					  1, texture->format(),
					  type, static_cast<char*>(data_variant->array()) + array_offset));
	array_offset += full_rows_height * width * data_variant->datasize() * tuplesize;
	SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), texture->mipmap_level(), 0, full_rows_height,
					  full_surfaces_depth, last_row_count, 1,
					  1, texture->format(),
					  type, static_cast<char*>(data_variant->array()) + array_offset));
      }
      break;
    default:
      SH_DEBUG_WARN("Texture target " << texture->target() << " not handled by GL backend");
      break;
    }
    
    
    return true;
  }
  
  int cost(const ShStorage* from, const ShStorage* to)
  {
    // Texture downloads are expensive!
    return 100;
  }

  static HostGlTextureTransfer* instance;
};

HostGlTextureTransfer* HostGlTextureTransfer::instance = new HostGlTextureTransfer();

}
