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
    //SH_DEBUG_PRINT("new pool");
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
