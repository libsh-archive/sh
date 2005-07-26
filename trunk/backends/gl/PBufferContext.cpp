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
#include "PBufferContext.hpp"
#include "GlBackend.hpp"
#include "GlTextures.hpp"
#include "GlTextureStorage.hpp"
#ifdef WIN32
#include "WGLPBufferContext.hpp"
#else
#include "GLXPBufferContext.hpp"
#endif

namespace shgl {

using namespace SH;

PBufferHandle::PBufferHandle()
{
}

PBufferHandle::~PBufferHandle()
{
}

PBufferStorage::~PBufferStorage()
{
}

ShPointer<PBufferContext> PBufferStorage::context() const
{
  return m_context;
}

class PBufferGlTextureTransfer : public ShTransfer {
  PBufferGlTextureTransfer()
    : ShTransfer("opengl:pbuffer", "opengl:texture")
  {
  }

  bool transfer(const ShStorage* from, ShStorage* to)
  {
    const PBufferStorage* pbuffer = dynamic_cast<const PBufferStorage*>(from);
    PBufferContextPtr context = pbuffer->context();
    
    PBufferHandlePtr handle = context->activate();
    GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

    SH_DEBUG_ASSERT(from->value_type() == to->value_type());

    SH_DEBUG_ASSERT(texture->target() == GL_TEXTURE_2D ||
                    texture->target() == GL_TEXTURE_RECTANGLE_NV);
    SH_DEBUG_ASSERT(context->width() == texture->width());
    SH_DEBUG_ASSERT(context->height() == texture->height());

    GlTextureName::Binding binding(texture->texName());
    SH_GL_CHECK_ERROR(glCopyTexSubImage2D(texture->target(), 0,
                                          0, 0, 0, 0, context->width(), context->height()));
    
    if (handle) { handle->restore(); }

    return true;
  }

  int cost(const ShStorage* from, const ShStorage* to)
  {
    return 20;
  }

  static PBufferGlTextureTransfer* instance;
};

PBufferGlTextureTransfer* PBufferGlTextureTransfer::instance = new PBufferGlTextureTransfer();

PBufferContext::PBufferContext(int width, int height, void* id)
  : m_width(width), m_height(height), m_id(id)
{
}

PBufferContext::~PBufferContext()
{
}

PBufferFactory::PBufferFactory()
{
}

PBufferFactory::~PBufferFactory()
{
}

PBufferFactory* PBufferFactory::instance()
{
#ifdef WIN32
  return WGLPBufferFactory::instance();
#else
  return GLXPBufferFactory::instance();
#endif  
}

}
