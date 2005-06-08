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
# include <ltdl.h>
# include <dirent.h>
# define LOCAL_BACKEND_DIRNAME ".shbackends"
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
  string backend_name = name();
  m_instantiated_backends->erase(backend_name);

#ifndef WIN32
  if (!lt_dlclose((*m_loaded_libraries)[backend_name])) {
    SH_DEBUG_ERROR("Could not unload the " << backend_name << " library: " << lt_dlerror());
  }
#else
  if (!FreeLibrary((HMODULE)(*m_loaded_libraries)[backend_name])) {
    SH_DEBUG_ERROR("Could not unload the " << backend_name << " library.");
  }
#endif

  m_loaded_libraries->erase(backend_name);
  m_selected_backends->erase(backend_name);
}

ShBackendSetPtr ShBackend::generate_set(const ShProgramSet& s)
{
  return new ShTrivialBackendSet(s, this);
}

string ShBackend::lookup_filename(const string& backend_name)
{
  init();
  string libname = "libsh" + backend_name;

#ifndef WIN32
  libname += ".so";
#else
# ifdef SH_DEBUG
    libname += "_DEBUG";
# endif
  libname += ".DLL";
#endif

  return libname;
}

bool ShBackend::is_valid_name(const string& backend_name)
{
  unsigned length = backend_name.size();
  if (0 == length) return false;

  if ("gl" == backend_name) return false; // not a real backend

  for (unsigned i=0; i < length; i++) {
    char c = backend_name[i];
    if (isalnum(c) || ('_' == c)) {
      continue;
    } else {
      return false; // found an invalid character
    }
  }
  return true;
}

bool ShBackend::load_library(const string& filename)
{
  init();

  if (filename.empty()) return false;

#ifndef WIN32
  unsigned extension_pos = filename.rfind(".so");
  unsigned filename_pos = filename.rfind("/") + 1;
#else
  string uc_filename(filename);
  std::transform(filename.begin(), filename.end(), uc_filename.begin(), toupper);
  unsigned extension_pos = uc_filename.rfind("_DEBUG.DLL");
  if (uc_filename.npos == extension_pos) extension_pos = uc_filename.rfind(".DLL");
  unsigned filename_pos = uc_filename.rfind("\\") + 1;
#endif
  filename_pos += 5; // remove the "libsh" portion of the filename

  string backend_name = filename.substr(filename_pos, extension_pos - filename_pos);
  if (!is_valid_name(backend_name)) return false; // invalid backend -- don't load
 
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

void ShBackend::load_libraries(const string& directory)
{
#ifndef WIN32
  DIR* dirp = opendir(directory.c_str());
  if (dirp) {
    // Go through all files in lib/sh/
    for (struct dirent* entry = readdir(dirp); entry != 0; entry = readdir(dirp)) {
      string filename(entry->d_name);
      unsigned extension_pos = filename.rfind(".so");
      if ((filename.find("libsh") == 0) && ((filename.size() - 3) == extension_pos)) {
        load_library(directory + "/" + filename);
      }
    }
    closedir(dirp);
  }
#else
  string filenames = directory + "\\libsh*.dll";
  WIN32_FIND_DATA find_data;

  HANDLE file_handle = FindFirstFile(filenames.c_str(), &find_data);
  while (file_handle != INVALID_HANDLE_VALUE) {
    load_library(string(find_data.cFileName));
    if (!FindNextFile(file_handle, &find_data)) break;
  }
  FindClose(file_handle);
#endif /* WIN32 */
}

ShBackendPtr ShBackend::instantiate_backend(const string& backend_name)
{
  init();
  ShBackend* backend = 0;

  if (m_instantiated_backends->find(backend_name) != m_instantiated_backends->end()) {
    return (*m_instantiated_backends)[backend_name];
  }

  LibraryHandle module = (*m_loaded_libraries)[backend_name];
  string init_function_name = "shBackend_libsh" + backend_name + "_instantiate";

  typedef ShBackend* EntryPoint();
#ifndef WIN32
  EntryPoint* instantiate = (EntryPoint*)lt_dlsym(module, init_function_name.c_str()); 
#else
  EntryPoint* instantiate = (EntryPoint*)GetProcAddress((HMODULE)module, init_function_name.c_str());  
#endif

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

bool ShBackend::use_backend(const string& backend_name)
{
  init();
  m_selected_backends->insert(backend_name);
  return load_library(lookup_filename(backend_name));
}

bool ShBackend::have_backend(const string& backend_name)
{
  init();
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

  string init_function_name = "shBackend_libsh" + backend_name + "_target_cost";
  LibraryHandle module = (*m_loaded_libraries)[backend_name];
  SH_DEBUG_ASSERT(module); // library not loaded

  typedef int EntryPoint(const string&);
#ifndef WIN32
  EntryPoint* func = (EntryPoint*)lt_dlsym(module, init_function_name.c_str());
#else
  EntryPoint* func = (EntryPoint*)GetProcAddress((HMODULE)module, init_function_name.c_str());
#endif

  if (func) {
     cost = (*func)(target);
  } else {
    SH_DEBUG_ERROR("Could not find " << init_function_name);
  }

  return cost;
}

string ShBackend::target_handler(const string& target, bool restrict_to_selected)
{
  init();

  bool selected_only = (m_selected_backends->size() > 0) && restrict_to_selected;

  if (!selected_only && !m_all_backends_loaded) {
    // Load all installed backend libraries
#ifndef WIN32
    load_libraries(string(getenv("HOME")) + "/" + LOCAL_BACKEND_DIRNAME);
    load_libraries(string(SH_INSTALL_PREFIX) + "/lib/sh");
#else
    load_libraries(string("."));

    // Search the system path
    string path(getenv("PATH"));
    unsigned length = path.size();
    unsigned start = 0;
    unsigned end = length - 1;
    while (start < length) {
      while ((start != end) && (' ' == path[start])) start++; // trim leading whitespace
      unsigned separator = path.find(";", start);
      if (separator != path.npos) end = separator - 1;
      while ((start != end ) && (' ' == path[end])) end--; // trim trailing whitespace

      string entry = path.substr(start, end - start + 1);
      load_libraries(entry);

      if (path.npos == separator) break; // reached the end of the path
      start = separator + 1;
      end = length - 1;
    }
#endif
    m_all_backends_loaded = true;
  }

  // Look in all loaded libraries for the backend with the lowest cost
  string best_backend = "";
  int lowest_cost = -1;
  for (LibraryMap::iterator i = m_loaded_libraries->begin(); 
       i != m_loaded_libraries->end(); i++) {
    string backend_name = i->first;
    if (selected_only && (m_selected_backends->find(backend_name) == m_selected_backends->end())) {
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

  return best_backend;
}

ShPointer<ShBackend> ShBackend::get_backend(const string& target)
{
  init();

  string best_backend = target_handler(target, true);
  
  if (!best_backend.empty()) {
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

  string userpath(getenv("HOME"));
  userpath += "/";
  userpath += LOCAL_BACKEND_DIRNAME;
  if (lt_dladdsearchdir(userpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + userpath + " to search dir: " << lt_dlerror());
  }

  string searchpath(SH_INSTALL_PREFIX);
  searchpath += "/lib/sh";
  if (lt_dladdsearchdir(searchpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + searchpath + " to search dir: " << lt_dlerror());
  }
#endif /* WIN32 */

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

list<string> ShBackend::derived_targets(const string& target)
{
  list<string> ret;

  // Vertex shaders
  if (("vertex" == target) || ("*:vertex" == target)) {
    ret.push_back("arb:vertex");
    ret.push_back("glsl:vertex");
    ret.push_back("gpu:vertex");
  } else if ("gpu:vertex" == target) {
    ret.push_back("arb:vertex");
    ret.push_back("glsl:vertex");
  } 
  // Fragment shaders
  else if (("fragment" == target) || ("*:fragment" == target)) {
    ret.push_back("arb:fragment");
    ret.push_back("glsl:fragment");
    ret.push_back("gpu:fragment");
  } else if ("gpu:fragment" == target) {
    ret.push_back("arb:fragment");
    ret.push_back("glsl:fragment");
  } 
  // Stream programs
  else if (("stream" == target) || ("*:stream" == target)) {
    ret.push_back("arb:stream");
    ret.push_back("glsl:stream");
    ret.push_back("cc:stream");
    ret.push_back("cpu:stream");
    ret.push_back("gpu:stream");
  } else if ("gpu:stream" == target) {
    ret.push_back("arb:stream");
    ret.push_back("glsl:stream");
  } else if ("cpu:stream" == target) {
    ret.push_back("cc:stream");
  }

  return ret;
}

}
