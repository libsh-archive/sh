#include "PBufferContext.hpp"

namespace shgl {

PBufferHandle::PBufferHandle()
{
}

PBufferHandle::~PBufferHandle()
{
}

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
