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
#include "GlTextureStorage.hpp"
#include "TypeInfo.hpp"
#include "VariantFactory.hpp"
#include "FBOCache.hpp"
#include <sh.hpp>

namespace shgl {

using namespace SH;

GlTextureStorage::GlTextureStorage(Memory* memory, GLenum target,
                                   GLenum format, GLint internalFormat,
                                   ValueType valueType, 
                                   int width, int height, int depth, int tuplesize,
                                   GlTextureNamePtr name, 
                                   GLint mipmap_level, bool internalRGB)
  : Storage(memory, valueType),
    m_name(name), m_target(target), m_format(format),
    m_internalFormat(internalFormat),
    m_width(width), m_height(height), m_depth(depth), m_tuplesize(tuplesize),
    m_write(false),
    m_internalFormatRGB(internalRGB),
    m_mipmap_level(mipmap_level)
{
  m_name->addStorage(this);
}

GlTextureStorage::~GlTextureStorage()
{
  m_name->removeStorage(this);
}

void GlTextureStorage::initTexture()
{
  HostGlTextureTransfer::instance->transfer(0, this);
}

std::ostream& operator<<(std::ostream& out, const GlTextureStorage& storage)
{
  out << "ID " << storage.name() << " tgt 0x" << std::hex
      << storage.target() << " fmt 0x" << storage.format() 
      << " infmt 0x" << storage.internalFormat()
      << (storage.internalFormatRGB() ? " (RGBx) " : " ")
      << std::dec << storage.width() << "x" << storage.height()
      << "x" << storage.depth() << "x" << storage.tuplesize()
      << " time " << storage.timestamp();
  if (storage.write())
    out << " write";
  return out;
}

HostGlTextureTransfer::HostGlTextureTransfer()
  : Transfer("host", "opengl:texture")
{
}
  
bool HostGlTextureTransfer::transfer(const Storage* from, Storage* to)
{
  const HostStorage* host = dynamic_cast<const HostStorage*>(from);
  GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

  // Bind texture name for this scope.
  GlTextureName::Binding binding(texture->texName());

  GLenum type;
  ValueType texture_type = texture->value_type();
  ValueType converted_type;
  type = glType(texture_type, converted_type);

  int width = texture->width();
  int height = texture->height();
  int depth = texture->depth();
  int tuplesize = texture->tuplesize();

  VariantPtr data_variant;
  int count = 0;
  bool full_copy = false;
  if (host) {
    count = host->length() / host->value_size() / tuplesize;
    full_copy = (count >= (width * height * depth));

    ValueType host_type = host->value_type();
    // @todo a little hackish...but we promise host->data() will not change... 
    VariantPtr host_variant = variantFactory(host_type, MEM)->
      generate(count * tuplesize, const_cast<void*>(host->data()), false);
    SH_DEBUG_ASSERT(host_variant);

    if(converted_type != VALUETYPE_END) {
      SH_DEBUG_WARN("GL backend does not handle " << valueTypeName(texture_type) 
              	    << " natively.  Converting to " << valueTypeName(converted_type));
      texture->value_type(converted_type);
      data_variant = variantFactory(converted_type, MEM)->generate(count * tuplesize);
      data_variant->set(host_variant);
    }
    else if (texture_type != host_type) {
      data_variant = variantFactory(texture_type, MEM)->generate(count * tuplesize);
      data_variant->set(host_variant);
    } else {
      data_variant = host_variant;
    }
  }

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
      if (host) {
        SH_GL_CHECK_ERROR(glTexSubImage1D(texture->target(), texture->mipmap_level(), 0,
                                          count, texture->format(), type,
                                          data_variant->array()));
      }
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
      if (host) {
        int last_row_count = count % width;
        int full_rows_count = count - last_row_count;
        int full_rows_height = full_rows_count / width;
        SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), texture->mipmap_level(), 0, 0,
                                          width, full_rows_height, texture->format(),
                                          type, data_variant->array()));
        int array_offset = full_rows_height * width * data_variant->datasize() * tuplesize;
        SH_GL_CHECK_ERROR(glTexSubImage2D(texture->target(), texture->mipmap_level(), 0, full_rows_height,
                                          last_row_count, 1, texture->format(),
                                          type, static_cast<const char*>(data_variant->array()) + array_offset));
      }
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
      if (host) {
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
                                          type, static_cast<const char*>(data_variant->array()) + array_offset));
        array_offset += full_rows_height * width * data_variant->datasize() * tuplesize;
        SH_GL_CHECK_ERROR(glTexSubImage3D(texture->target(), texture->mipmap_level(), 0, full_rows_height,
                                          full_surfaces_depth, last_row_count, 1,
                                          1, texture->format(),
                                          type, static_cast<const char*>(data_variant->array()) + array_offset));
      }
    }
    break;
  default:
    SH_DEBUG_WARN("Texture target " << texture->target() << " not handled by GL backend");
    break;
  }
  
  return true;
}
  
int HostGlTextureTransfer::cost(const Storage* from, const Storage* to)
{
  // Texture downloads are expensive!
  return 100;
}

GlTextureHostTransfer::GlTextureHostTransfer()
  : Transfer("opengl:texture", "host")
{
}
  
bool GlTextureHostTransfer::transfer(const Storage* from, Storage* to)
{
  // Get rid of const so that we can do conversions
  GlTextureStorage* texture = 
    dynamic_cast<GlTextureStorage*>(const_cast<Storage*>(from));
  HostStorage* host = dynamic_cast<HostStorage*>(to);
        
  GLenum type;
  ValueType texture_type = texture->value_type();
  ValueType converted_type;
  type = glType(texture_type, converted_type);

  // The conversion should have occured during download 
  if (converted_type != VALUETYPE_END) {
    // convert the storage. Should we warn (again) ?
    texture->value_type(converted_type);
    texture_type = texture->value_type();
  }

  int width = texture->width();
  int height = texture->height();
  int depth = texture->depth();
  int tuplesize = texture->tuplesize();
  int count = host->length() / host->value_size() / tuplesize;
  if (count > width * height * depth)
    count = width * height * depth;

  VariantPtr dest_variant;
  ValueType host_type = host->value_type();
  // @todo a little hackish...but we promise host->data() will not change... 
  VariantPtr host_variant = variantFactory(host_type, MEM)->
    generate(count * tuplesize, const_cast<void*>(host->data()), false);
  SH_DEBUG_ASSERT(host_variant);
    
  SH_DEBUG_ASSERT(tuplesize > 0 && tuplesize < 5)
  int glTuplesize[5] = { 0, 1, 4, 3, 4 };
  GLenum format[5] = { 0, GL_RED, GL_RGBA, GL_RGB, GL_RGBA };

  if (converted_type != VALUETYPE_END || 
      texture_type != host_type || tuplesize == 2) {
    dest_variant = variantFactory(texture_type, MEM)->
      generate(count * glTuplesize[tuplesize]);
  } 
  else {
    dest_variant = host_variant;
  }

  FBOCache::instance()->bindFramebuffer();
  GLenum buffer = FBOCache::instance()->bindTexture(texture, 0);
  FBOCache::instance()->check();
    
  GLint prevRead;
  SH_GL_CHECK_ERROR(glGetIntegerv(GL_READ_BUFFER, &prevRead));
  SH_GL_CHECK_ERROR(glReadBuffer(buffer));

  SH_GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT));
  SH_GL_CHECK_ERROR(glViewport(0, 0, width, height));

  SH_GL_CHECK_ERROR(glReadPixels(0, 0, count > width ? width : count,
                                 count/width, format[tuplesize], type,
                                 dest_variant->array()));
  if (count % width) {
    SH_GL_CHECK_ERROR(glReadPixels(0, count/width, count % width, 1, 
                                   format[tuplesize], type,
                                   static_cast<char *>(dest_variant->array())
                                   + (count - (count % width)) * 
                                     glTuplesize[tuplesize] * 
                                     dest_variant->datasize()));
  }

  if (dest_variant != host_variant) {
    if (tuplesize == 2) {
      dest_variant = dest_variant->get(false, Swizzle(4, 0, 3), count);
    }
    host_variant->set(dest_variant);
  }

  SH_GL_CHECK_ERROR(glPopAttrib());
  SH_GL_CHECK_ERROR(glReadBuffer(prevRead));
  FBOCache::instance()->unbindFramebuffer();

  return true;
}
  
int GlTextureHostTransfer::cost(const Storage* from, const Storage* to)
{
  // Reading data from GPU is waaay expensive
  return 1000;
}

GlTextureGlTextureTransfer::GlTextureGlTextureTransfer(const std::string& target)
  : Transfer("opengl:texture", "opengl:texture"), m_target(target)
{
  Program vsh = SH_BEGIN_PROGRAM(target+":vertex") {
    InOutPosition4f pos;
    InOutTexCoord2f tc;
  } SH_END;
  Program fsh = SH_BEGIN_PROGRAM(target+":fragment") {
    InputTexCoord2f tc;
    OutputColor4f o = source_texture(tc);
  } SH_END;
  render_to_tex_prog = new ProgramSet(vsh, fsh);
}
  
bool GlTextureGlTextureTransfer::transfer(const Storage* from, Storage* to)
{
  GlTextureStorage* src_tex = 
    dynamic_cast<GlTextureStorage*>(const_cast<Storage*>(from));
  GlTextureStorage* dst_tex = dynamic_cast<GlTextureStorage*>(to);
        
  SH_DEBUG_ASSERT(src_tex->width() == dst_tex->width());
  SH_DEBUG_ASSERT(src_tex->height() == dst_tex->height());

  GlTextureName::Binding binding(dst_tex->texName());

  if (dst_tex->internalFormatRGB()) {
    FBOCache::instance()->bindFramebuffer();
    GLenum buffer = FBOCache::instance()->bindTexture(dst_tex, 0);
    FBOCache::instance()->check();
  
    GLint prevDraw;
    SH_GL_CHECK_ERROR(glGetIntegerv(GL_DRAW_BUFFER, &prevDraw));
    SH_GL_CHECK_ERROR(glDrawBuffer(buffer));

    SH_GL_CHECK_ERROR(glPushAttrib(GL_VIEWPORT_BIT |  GL_ENABLE_BIT));
    SH_GL_CHECK_ERROR(glViewport(0, 0, dst_tex->width(), dst_tex->height()));
    
    GLint prev_vert, prev_frag;
    GLhandleARB prev_prog;
    if (m_target == "arb") {
      SH_GL_CHECK_ERROR(glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &prev_vert));
      SH_GL_CHECK_ERROR(glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &prev_frag));
    }
    else {
      SH_GL_CHECK_ERROR(prev_prog = glGetHandleARB(GL_PROGRAM_OBJECT_ARB));
    }

    std::ostringstream os;
    os << src_tex->name();
    source_texture.meta("opengl:texid", os.str());

    bind(*render_to_tex_prog);
    glBegin(GL_QUADS); {
      glTexCoord2f(0,0);
      glVertex2f(-1, -1);
      glTexCoord2f(1,0);
      glVertex2f(1, -1);
      glTexCoord2f(1,1);
      glVertex2f(1, 1);
      glTexCoord2f(0,1);
      glVertex2f(-1, 1);
    } glEnd();
    unbind(*render_to_tex_prog);

    if (m_target == "arb") {
      SH_GL_CHECK_ERROR(glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prev_frag));
      SH_GL_CHECK_ERROR(glBindProgramARB(GL_VERTEX_PROGRAM_ARB, prev_vert));
    }
    else {
      SH_GL_CHECK_ERROR(glUseProgramObjectARB(prev_prog));
    }
    SH_GL_CHECK_ERROR(glPopAttrib());
    SH_GL_CHECK_ERROR(glDrawBuffer(prevDraw));
    FBOCache::instance()->unbindFramebuffer();
  }
  else {
    HostStoragePtr host = 
      shref_dynamic_cast<HostStorage>(src_tex->memory()->findStorage("host"));
    host->sync();
    to->sync();
  }

  return true;
}
  
int GlTextureGlTextureTransfer::cost(const Storage* from, const Storage* to)
{
  const GlTextureStorage* dst_tex = dynamic_cast<const GlTextureStorage*>(to);
  
  if (dst_tex->internalFormatRGB())
    return 20;
  else
    return 2000;
}

HostGlTextureTransfer* HostGlTextureTransfer::instance;
GlTextureHostTransfer* GlTextureHostTransfer::instance;
GlTextureGlTextureTransfer* GlTextureGlTextureTransfer::instance;

}
