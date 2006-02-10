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
#ifndef SHPOOL_HPP
#define SHPOOL_HPP

#define SH_USE_MEMORY_POOL

#ifdef SH_USE_MEMORY_POOL

#include <cstddef>
#include "ShDllExport.hpp"

namespace SH {

class 
SH_DLLEXPORT
ShPool {
public:
  ShPool(std::size_t element_size, std::size_t block_size);

  void* alloc();
  void free(void*);

private:
  std::size_t m_element_size;
  std::size_t m_block_size;

  void* m_next;
};

}

#endif // SH_USE_MEMORY_POOL

#endif
