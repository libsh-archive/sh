// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#include <algorithm>
#include "ShBackend.hpp"
#include "ShProgram.hpp"

namespace SH {

ShBackend::ShBackendList* ShBackend::m_backends = 0;

ShBackendCode::~ShBackendCode()
{
}

ShBackend::ShBackend()
{
  if (!m_backends) m_backends = new ShBackendList();
  m_backends->push_back(this);
}

ShBackend::~ShBackend()
{
  m_backends->erase(std::remove(begin(), end(), ShBackendPtr(this)), end());
}

ShBackend::ShBackendList::iterator ShBackend::begin()
{
  if (!m_backends) m_backends = new ShBackendList();
  return m_backends->begin();
}

ShBackend::ShBackendList::iterator ShBackend::end()
{
  if (!m_backends) m_backends = new ShBackendList();
  return m_backends->end();
}

ShRefCount<ShBackend> ShBackend::lookup(const std::string& name)
{
  if (!m_backends) m_backends = new ShBackendList();
  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name) return *I;
  }
  return 0;
}


}
