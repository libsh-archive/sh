#include "ShPool.hpp"

#ifdef SH_USE_MEMORY_POOL

#include "ShDebug.hpp"

namespace SH {

ShPool::ShPool(std::size_t element_size, std::size_t block_size)
  : m_element_size(element_size),
    m_block_size(block_size),
    m_next(0)
{
}

void* ShPool::alloc()
{
  if (!m_next) {
    SH_DEBUG_PRINT("new pool");
    char* block = new char[m_block_size * m_element_size];
    void* next = 0;
    for (std::size_t i = 0; i < m_block_size; i++) {
      *((void**)block) = next;
      next = (void*)block;
      block += m_element_size;
    }
    m_next = next;
  }
  void* r = m_next;
  m_next = *((void**)m_next);
  return r;
}

void ShPool::free(void* ptr)
{
  *((void**)ptr) = m_next;
  m_next = ptr;
}

}

#endif // SH_USE_MEMORY_POOL
