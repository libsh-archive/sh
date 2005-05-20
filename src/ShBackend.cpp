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
#include <dirent.h>

#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShDebug.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"
#include "ShSyntax.hpp"

namespace SH {

ShBackend::ShBackendList* ShBackend::m_instantiated_backends = 0;
ShBackend::ShBackendList* ShBackend::m_selected_backends = 0;
std::set<std::string>* ShBackend::m_selected_backend_names = 0;
bool ShBackend::m_doneInit = false;
bool ShBackend::m_all_backends_loaded = false;

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
}

ShBackend::~ShBackend()
{
  m_instantiated_backends->remove(ShBackendPtr(this));
  m_selected_backends->remove(ShBackendPtr(this));
  m_selected_backend_names->erase(name());
}

ShBackendSetPtr ShBackend::generate_set(const ShProgramSet& s)
{
  return new ShTrivialBackendSet(s, this);
}

ShBackend::ShBackendList::iterator ShBackend::begin()
{
  init();
  return m_instantiated_backends->begin();
}

ShBackend::ShBackendList::iterator ShBackend::end()
{
  init();
  return m_instantiated_backends->end();
}

ShPointer<ShBackend> ShBackend::lookup(const std::string& name)
{
  init();
  
  for (ShBackendList::iterator I = m_instantiated_backends->begin(); 
       I != m_instantiated_backends->end(); ++I) {
    if ((*I)->name() == name) return *I;
  }

  std::string libname;
#ifndef WIN32
  libname = SH_INSTALL_PREFIX;
  libname += "/lib/sh/libsh" + name + ".so";
#else
  libname = "LIBSH";
  libname += name;
# ifdef SH_DEBUG
    libname += "D";
# endif
  libname += ".DLL";
#endif /* WIN32 */

  load_library(libname);

  for (ShBackendList::iterator I = m_instantiated_backends->begin();
       I != m_instantiated_backends->end(); ++I) {
    if ((*I)->name() == name) return *I;
  }
  
  SH_DEBUG_ERROR("Could not find the " << name << " backend");
  
  return 0;
}

void ShBackend::load_library(const std::string& filename)
{
  init();
  ShBackend* backend = 0;

#ifndef WIN32
  unsigned extension_pos = filename.rfind(".so");
  unsigned filename_pos = filename.rfind("/") + 1;
  std::string backend_name = filename.substr(filename_pos, extension_pos - filename_pos);

  lt_dlhandle module = lt_dlopenext(filename.c_str());
  if (module) {
    // Instantiate the backend
    std::string init_function_name = "shBackend_" + backend_name + "_instantiate";
    typedef ShBackend* EntryPoint();
    EntryPoint* instantiate = (EntryPoint*)lt_dlsym(module, init_function_name.c_str()); 
   
    if (instantiate) {
      backend = (*instantiate)();
    } else {
      SH_DEBUG_ERROR("Could not find " << init_function_name);
    }
  } else {
    SH_DEBUG_WARN("Could not open " << filename << ": " << lt_dlerror());
  }
#else
  HMODULE module = NULL;
  module = LoadLibrary(filename.c_str());

  if (module) {
    // TODO: Instantiate the backend
  } else {
    SH_DEBUG_ERROR("Could not open " << filename);
  }
#endif /* WIN32 */

  if (backend) {
    m_instantiated_backends->push_back(backend);
  }
}

bool ShBackend::use_backend(const std::string& name)
{
  init();
  if (m_selected_backend_names->find(name) != m_selected_backend_names->end()) {
    return true; // already selected
  }
  ShBackendPtr backend = SH::ShBackend::lookup(name);
  if (!backend) return false;
  m_selected_backends->push_back(backend);
  m_selected_backend_names->insert(name);
  return true;
}

void ShBackend::clear_backends()
{
  init();
  m_selected_backend_names->clear();
  m_selected_backends->clear();
}

ShPointer<ShBackend> ShBackend::get_backend(const std::string& target)
{
  using namespace std;

  init();
  if (m_selected_backends->size() > 0) {
    // First look in all selected backends
    for (ShBackendList::iterator i = m_selected_backends->begin(); 
	 i != m_selected_backends->end(); i++) {
      if ((*i)->can_handle(target)) return *i;
    }
  }

  if (!m_all_backends_loaded) {
    // Instantiate all installed backends
#ifndef WIN32
    string search_path(SH_INSTALL_PREFIX);
    search_path += "/lib/sh/";

    DIR* dirp = opendir(search_path.c_str());
    if (dirp) {
      // Go through all files in lib/sh/
      for (struct dirent* entry = readdir(dirp); entry != 0; entry = readdir(dirp)) {
	string filename(entry->d_name);
	unsigned extension_pos = filename.rfind(".so");
	if ((filename.find("libsh") == 0) && ((filename.size() - 3) == extension_pos)) {
	  load_library(search_path + filename);
	}
      }
      closedir(dirp);
    }
#else
    // TODO: Find all DLLs
#endif /* WIN32 */
    m_all_backends_loaded = true;
  }

  // Look in all instantiated backends
  for (ShBackendList::iterator i = m_instantiated_backends->begin(); 
       i != m_instantiated_backends->end(); i++) {
    if ((*i)->can_handle(target)) return *i;
  }

  cerr << "Could not find a backend that supports the '" << target << "' target." << endl;
  return 0;
}

void ShBackend::init()
{
  if (m_doneInit) return;
  
  m_instantiated_backends = new ShBackendList();
  m_selected_backends = new ShBackendList();
  m_selected_backend_names = new std::set<std::string>();

#ifndef WIN32
  if (lt_dlinit()) {
    SH_DEBUG_ERROR("Error initializing ltdl: " << lt_dlerror());
  }

  std::string searchpath(SH_INSTALL_PREFIX);
  searchpath += "/lib/sh";

  // TODO: add ~/.shbackends/ to the search path

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
