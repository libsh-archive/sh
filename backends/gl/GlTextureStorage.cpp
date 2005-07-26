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
#include "GlTextureStorage.hpp"
#include "ShTypeInfo.hpp"
#include "ShVariantFactory.hpp"
#include <stdio.h>

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
    m_mipmap_level(mipmap_level), m_writeflag(false)
{
  m_name->addStorage(this);
}

GlTextureStorage::~GlTextureStorage()
{
  m_name->removeStorage(this);
}


bool GlTextureStorage::allocate(){

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texName());

    GLenum type;
    ShValueType converted_type;
    type = shGlType(value_type(), converted_type);

    switch(target()) {
    case GL_TEXTURE_1D:
     	SH_GL_CHECK_ERROR(glTexImage1D(target(), mipmap_level(),
				       internalFormat(),
				       width(), 0, format(), type,
				       0));
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
       SH_GL_CHECK_ERROR(glTexImage2D(target(), mipmap_level(),
				       internalFormat(),
				       width(), height(), 0, format(),
				       type, 0));
     break;
    case GL_TEXTURE_3D:
      	SH_GL_CHECK_ERROR(glTexImage3D(target(), mipmap_level(),
				       internalFormat(),
				       width(), height(),
				       depth(), 0, format(),
				       type, 0));
      break;
    default:
      SH_DEBUG_WARN("Texture target " << target() << " not handled by GL backend");
      return false;
    }
    
      SH_GL_CHECK_ERROR(glBindTexture(target(), 0));
	
    return true;
}


class HostGlTextureTransfer : public ShTransfer {
  HostGlTextureTransfer()
    : ShTransfer("host", "opengl:texture")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {

printf("GlTextureStorage.cpp:transfer(): Wants to transfer from host to tex\n");	  
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

//}



class GlTextureHostTransfer : public ShTransfer {
  GlTextureHostTransfer()
    : ShTransfer("opengl:texture", "host")
  {
  }
  
  bool transfer(const ShStorage* from, ShStorage* to)
  {
    //printf("TRANSFER: Wants to transfer from tex to host\n");
	  
    const GlTextureStorage* texture = dynamic_cast<const GlTextureStorage*>(from);
    ShHostStorage* host = dynamic_cast<ShHostStorage*>(to);

    // Bind texture name for this scope.
    GlTextureName::Binding binding(texture->texName());

    ShValueType host_type = host->value_type();
    ShValueType texture_type = texture->value_type();
    int count = texture->count();
    int tuplesize = texture->tuplesize();
    ShValueType valueType = host_type;
	    
//
    // @todo half-float
    //GLenum readpixelType;
    ShValueType convertedType; 
    GLenum type = shGlType(valueType, convertedType);
    if (convertedType != SH_VALUETYPE_END) {
      SH_DEBUG_WARN("GL backend does not handle stream output type " 
		    << shValueTypeName(valueType) << " natively."
                    << "  Using " << shValueTypeName(convertedType) 
		    << " temporary buffer.");
    }
    
    ShVariantPtr  resultBuffer;
    int resultDatasize = tuplesize * count;
    bool using_temporary_buffer = false;
   // if ((tuplesize() != 2) && (SH_VALUETYPE_END == convertedType)) {
      // Use the output buffer directly
      resultBuffer = shVariantFactory(valueType, SH_MEM)->
	generate(resultDatasize, host->data(), false);
   /* } else {
      if (2 == output->size()) {
	// Hack to support 2-component outputs (add an extra component)
	resultDatasize = 3 * count;
	if (SH_VALUETYPE_END == convertedType) convertedType = valueType;
      }
      // Use a temporary buffer
      resultBuffer = shVariantFactory(convertedType, SH_MEM)->
	generate(resultDatasize);
      using_temporary_buffer = true;
    }*/
//

    
   /* ShVariantPtr data_variant;
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
*/
    int width = texture->width();
    int height = texture->height();
    int depth = texture->depth();

    // If the texture is not full, must not copy more than count
    bool full_copy = (count == (width * height * depth));

    void* augmented_data;
    //if(!full_copy){
    	augmented_data = malloc(width * height * depth * tuplesize * 4); // to be able to fit the whole texture
   // }
	    
	    

 /*   switch(texture->target()) {
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
#endif*/
      /*if (full_copy) {
	SH_GL_CHECK_ERROR(glGetTexImage(texture->target(), texture->mipmap_level(),
				       texture->format(), type, host->data()));
      } else {*/
	
	SH_GL_CHECK_ERROR(glGetTexImage(GL_TEXTURE_2D/*texture->target()*/, 0/*texture->mipmap_level()*/,
				       texture->format(), type, host->data()/*resultBuffer->array()*//*augmented_data*/));
	
	if(count == 0)
		printf("Count is ZERO!\n");
	
//	memcpy(host->data(), augmented_data, count*tuplesize*4);
	free(augmented_data);

	
	/*SH_GL_CHECK_ERROR(glTexImage2D(texture->target(), texture->mipmap_level(),

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
					  type, static_cast<char*>(data_variant->array()) + array_offset));*/
    //  }//else
 /*     break;
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
 */   
    
    return true;
  }
  
  int cost(const ShStorage* from, const ShStorage* to)
  {
    // Texture uploads are also expensive!
    return 100;
  }

  static GlTextureHostTransfer* instance;
};

GlTextureHostTransfer* GlTextureHostTransfer::instance = new GlTextureHostTransfer();

}


