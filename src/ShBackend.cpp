#include <algorithm>
#include "ShBackend.hpp"

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
