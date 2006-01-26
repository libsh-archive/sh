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
