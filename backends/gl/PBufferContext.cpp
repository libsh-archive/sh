#include "PBufferContext.hpp"
#include "GlBackend.hpp"
#include "GlTextures.hpp"
#include "GlTextureStorage.hpp"

namespace shgl {

using namespace SH;

PBufferHandle::PBufferHandle()
{
}

PBufferHandle::~PBufferHandle()
{
}

PBufferStorage::PBufferStorage(const ShPointer<PBufferContext>& context,
                               ShMemory* memory)
  : ShStorage(memory),
    m_context(context)
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
    std::cerr << "Transferring pbuffer to texture" << std::endl;
    
    const PBufferStorage* pbuffer = dynamic_cast<const PBufferStorage*>(from);
    PBufferContextPtr context = pbuffer->context();
    
    PBufferHandlePtr handle = context->activate();
    GlTextureStorage* texture = dynamic_cast<GlTextureStorage*>(to);

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

  int cost()
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


}
