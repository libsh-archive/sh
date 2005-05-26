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
////////////////////ppppp//////////////////////////////////////////////////////////
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef WIN32
# include <ltdl.h>
# include <dirent.h>
#else
# include <windows.h>
#endif

#include <algorithm>
#include <cctype>

#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShDebug.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"
#include "ShSyntax.hpp"

namespace SH {

using namespace std;

ShBackend::BackendMap* ShBackend::m_instantiated_backends = 0;
ShBackend::BackendSet* ShBackend::m_selected_backends = 0;
ShBackend::LibraryMap* ShBackend::m_loaded_libraries = 0;
bool ShBackend::m_done_init = false;
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
  list<ShBackendCodePtr> m_code;
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
  for (list<ShBackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->bind();
  }
}

void ShTrivialBackendSet::unbind()
{
  // TODO: This may not quite have the correct semantics
  for (list<ShBackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->unbind();
  }
}
 
ShBackend::ShBackend()
{
}

ShBackend::~ShBackend()
{
  m_instantiated_backends->erase(name());
  // TODO: unload the library
  m_loaded_libraries->erase(name());
  m_selected_backends->erase(name());
}

ShBackendSetPtr ShBackend::generate_set(const ShProgramSet& s)
{
  return new ShTrivialBackendSet(s, this);
}

string ShBackend::lookup_filename(const string& name)
{
  init();
  string libname;

#ifndef WIN32
  libname = SH_INSTALL_PREFIX;
  libname += "/lib/sh/" + name + ".so";
#else
  libname = name;
# ifdef SH_DEBUG
    libname += "D";
# endif
  libname += ".DLL";
#endif /* WIN32 */

  // TODO: check if the file exists and is readable

  return libname;
}

bool ShBackend::load_library(const string& filename)
{
  init();

  if (filename.empty()) return false;

#ifndef WIN32
  unsigned extension_pos = filename.rfind(".so");
  unsigned filename_pos = filename.rfind("/") + 1;
#else
  string uc_filename;
  std::transform(filename.begin(), filename.end(), uc_filename.begin(), toupper);
  unsigned extension_pos = filename.rfind(".DLL");
  unsigned filename_pos = filename.rfind("\\") + 1;
#endif

  string backend_name = filename.substr(filename_pos, extension_pos - filename_pos);
  if (m_loaded_libraries->find(backend_name) != m_loaded_libraries->end()) {
    return true; // already loaded
  }

#ifndef WIN32
  LibraryHandle module = lt_dlopenext(filename.c_str());
#else
  LibraryHandle module = LoadLibrary(filename.c_str());
#endif
  
  if (module) {
    (*m_loaded_libraries)[backend_name] = module;
    return true;
  } else {
    SH_DEBUG_WARN("Could not open " << filename);
    return false;
  }
}

ShBackendPtr ShBackend::instantiate_backend(const string& backend_name)
{
  init();
  ShBackend* backend = 0;

  if (m_instantiated_backends->find(backend_name) != m_instantiated_backends->end()) {
    return (*m_instantiated_backends)[backend_name];
  }

  LibraryHandle module = (*m_loaded_libraries)[backend_name];
  string init_function_name = "shBackend_" + backend_name + "_instantiate";

  typedef ShBackend* EntryPoint();
#ifndef WIN32
  EntryPoint* instantiate = (EntryPoint*)lt_dlsym(module, init_function_name.c_str()); 
#else
  EntryPoint* instantiate = (EntryPoint*)GetProcAddress(module, init_function_name.c_str());  
#endif /* WIN32 */

  if (instantiate) {
    backend = (*instantiate)();
  } else {
    SH_DEBUG_ERROR("Could not find " << init_function_name);
  }

  if (backend) {
    (*m_instantiated_backends)[backend_name] = backend;
  }

  return backend;
}

bool ShBackend::use_backend(const string& name)
{
  init();
  const string backend_name = "libsh" + name;
  m_selected_backends->insert(backend_name);
  return load_library(lookup_filename(backend_name));
}

void ShBackend::clear_backends()
{
  init();
  m_selected_backends->clear();
}

int ShBackend::target_cost(const string& backend_name, const string& target)
{
  init();
  int cost = 0;

  string init_function_name = "shBackend_" + backend_name + "_target_cost";
  LibraryHandle module = (*m_loaded_libraries)[backend_name];
  SH_DEBUG_ASSERT(module); // library not loaded

  typedef int EntryPoint(const string&);
#ifndef WIN32
  EntryPoint* func = (EntryPoint*)lt_dlsym(module, init_function_name.c_str());
#else
  EntryPoint* func = (EntryPoint*)GetProcAddress(module, init_function_name.c_str());
#endif /* WIN32 */

  if (func) {
     cost = (*func)(target);
  } else {
    SH_DEBUG_ERROR("Could not find " << init_function_name);
  }

  return cost;
}

ShPointer<ShBackend> ShBackend::get_backend(const string& target)
{
  init();

  if ((0 == m_selected_backends->size()) && (!m_all_backends_loaded)) {
    // Load all installed backend libraries
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

  // Look in all loaded libraries for the backend with the lowest cost
  string best_backend = "";
  int lowest_cost = -1;
  for (LibraryMap::iterator i = m_loaded_libraries->begin(); 
       i != m_loaded_libraries->end(); i++) {
    string backend_name = i->first;
    if ((m_selected_backends->size() > 0) && 
	m_selected_backends->find(backend_name) == m_selected_backends->end()) {
      continue; // this backend is not currently in use
    }

    int cost = target_cost(backend_name, target);
    if (0 == cost) continue; // not supported by this backend

    if ((lowest_cost < 0) || (cost < lowest_cost)) {
      lowest_cost = cost;
      best_backend = backend_name;
      if (1 == cost) break; // we won't find a better one
    }
  }
  
  if (lowest_cost > 0) {
    return instantiate_backend(best_backend);
  } else {
    cerr << "Could not find a backend that supports the '" << target << "' target." << endl;
    if (m_selected_backends->size() > 0) {
      cerr << "Try removing all shUseBackend() and shSetBackend() calls to search all installed backends." << endl;
    }
    return 0;
  }
}

void ShBackend::init()
{
  if (m_done_init) return;
  
  m_instantiated_backends = new BackendMap();
  m_selected_backends = new BackendSet();
  m_loaded_libraries = new LibraryMap();

#ifndef WIN32
  if (lt_dlinit()) {
    SH_DEBUG_ERROR("Error initializing ltdl: " << lt_dlerror());
  }

  string searchpath(SH_INSTALL_PREFIX);
  searchpath += "/lib/sh";

  // TODO: add ~/.shbackends/ to the search path

  if (lt_dladdsearchdir(searchpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + searchpath + " to search dir: " << lt_dlerror());
  }
#endif

  m_done_init = true;
}

void ShBackend::unbind_all_programs()
{
  while (ShContext::current()->begin_bound() != ShContext::current()->end_bound()) {
    shUnbind(ShContext::current()->begin_bound()->second);
  }
}

void ShBackend::unbind_all_backends()
{
  init();
  for (BackendMap::iterator i = m_instantiated_backends->begin();
       i != m_instantiated_backends->end(); i++) {
    i->second->unbind_all_programs();
  }
}

}
