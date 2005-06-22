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
