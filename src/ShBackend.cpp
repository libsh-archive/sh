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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef WIN32
#include <ltdl.h>
#else
#include <windows.h>
#endif

#include <algorithm>

#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShDebug.hpp"
#include "ShEnvironment.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"
#include "ShSyntax.hpp"

namespace SH {

ShBackend::ShBackendList* ShBackend::m_backends = 0;
bool ShBackend::m_doneInit = false;

ShBackendCode::~ShBackendCode()
{
}

ShBackendSet::~ShBackendSet()
{
}

// A default implementation of ShBackendSet, usable for any backends
// that don't involve special linking.
class ShTrivialBackendSet : public ShBackendSet {
public:
  ShTrivialBackendSet(const ShProgramSet& s,
                      const ShBackendPtr& backend);
  
  virtual ~ShTrivialBackendSet();

  virtual void link();

  virtual void bind();
  virtual void unbind();

protected:
  std::list<ShBackendCodePtr> m_code;
};

typedef ShPointer<ShTrivialBackendSet> ShTrivialBackendSetPtr;
typedef ShPointer<const ShTrivialBackendSet> ShTrivialBackendSetCPtr;

ShTrivialBackendSet::ShTrivialBackendSet(const ShProgramSet& s,
                                         const ShBackendPtr& backend)
{
  for (ShProgramSet::const_iterator I = s.begin(); I != s.end(); ++I) {
    m_code.push_back((*I)->code(backend));
  }
}

ShTrivialBackendSet::~ShTrivialBackendSet()
{
}

void ShTrivialBackendSet::link()
{
}

void ShTrivialBackendSet::bind()
{
  for (std::list<ShBackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->bind();
  }
}

void ShTrivialBackendSet::unbind()
{
  // TODO: This may not quite have the correct semantics
  for (std::list<ShBackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->unbind();
  }
}
 
ShBackend::ShBackend()
{
  init();
  m_backends->push_back(this);
}

ShBackend::~ShBackend()
{
  m_backends->erase(std::remove(begin(), end(), ShBackendPtr(this)), end());
}

ShBackendSetPtr ShBackend::generate_set(const ShProgramSet& s)
{
  return new ShTrivialBackendSet(s, this);
}

ShBackend::ShBackendList::iterator ShBackend::begin()
{
  init();
  return m_backends->begin();
}

ShBackend::ShBackendList::iterator ShBackend::end()
{
  init();
  if (!m_backends) m_backends = new ShBackendList();
  return m_backends->end();
}

ShPointer<ShBackend> ShBackend::lookup(const std::string& name)
{
  init();
  
  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name) return *I;
  }
#ifndef WIN32
  std::string libname(SH_INSTALL_PREFIX);
  libname += "/lib/sh/libsh" + name + ".so";
  
  lt_dlhandle handle = lt_dlopenext(libname.c_str());

  if (!handle) {
    SH_DEBUG_ERROR("Could not open " << libname << ": " << lt_dlerror());
    return 0;
  }

  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name) {
      return *I;
    }
  }

  SH_DEBUG_ERROR("Could not find " << name << "backend");

  return 0;
#else
  
  HMODULE mod = NULL;
  std::string libname("LIBSH");
  libname += name;
#ifdef SH_DEBUG
  libname += "D";
#endif
  libname += ".DLL";
  mod = LoadLibrary(libname.c_str());

  if (!mod) {
    SH_DEBUG_ERROR("Could not open " << libname);
    return 0;
  }

  for (ShBackendList::iterator I = begin(); I != end(); ++I) {
    if ((*I)->name() == name) {
      return *I;
    }
  }
  
  SH_DEBUG_ERROR("Could not find " << name << "backend");
  
  return 0;
#endif
}

void ShBackend::init()
{
  if (m_doneInit) return;
  // SH_DEBUG_PRINT("Initializing backend system");
  
  m_backends = new ShBackendList();

#ifndef WIN32
  if (lt_dlinit()) {
    SH_DEBUG_ERROR("Error initializing ltdl: " << lt_dlerror());
  }

  std::string searchpath(SH_INSTALL_PREFIX);
  searchpath += "/lib/sh";

  if (lt_dladdsearchdir(searchpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + searchpath + " to search dir: " << lt_dlerror());
  }
#endif

  m_doneInit = true;
}

void ShBackend::unbind_all()
{
  // This won't really work with multiple backends, but is good enough
  // for now -- sdt
  
  while (ShContext::current()->begin_bound() != ShContext::current()->end_bound()) {
    shUnbind(ShContext::current()->begin_bound()->second);
  }
}

}
