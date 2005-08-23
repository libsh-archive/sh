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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(WIN32)
# include <windows.h>
#elif defined(__APPLE__)
# include <CoreFoundation/CoreFoundation.h>
# include <dirent.h>
#else
# include <ltdl.h>
# include <dirent.h>
# define LOCAL_BACKEND_DIRNAME ".shbackends"
#endif


#include <algorithm>
#include <cctype>

#include "ShBackend.hpp"
#include "ShProgram.hpp"
#include "ShDebug.hpp"
#include "ShInternals.hpp"
#include "ShTransformer.hpp"
#include "ShSyntax.hpp"

namespace {

template<typename EntryPoint>
EntryPoint* load_function(SH::ShBackend::LibraryHandle module, const char* name)
{
#if defined(WIN32)
  return (EntryPoint*)GetProcAddress((HMODULE)module, name);  
#elif defined(__APPLE__)
  CFStringRef n = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
  EntryPoint* p = (EntryPoint*)CFBundleGetFunctionPointerForName(module, n);
  CFRelease(n);
  return p;
#else
  return (EntryPoint*)lt_dlsym(module, name); 
#endif
}

}

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

#if defined(WIN32)
  if (!FreeLibrary((HMODULE)(*m_loaded_libraries)[backend_name])) {
    SH_DEBUG_ERROR("Could not unload the " << backend_name << " library.");
  }
#elif defined(__APPLE__)
  CFRelease((*m_loaded_libraries)[backend_name]);
#else
  if (!lt_dlclose((*m_loaded_libraries)[backend_name])) {
    SH_DEBUG_ERROR("Could not unload the " << backend_name << " library: " << lt_dlerror());
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

#if defined(WIN32)
# ifdef SH_DEBUG
    libname += "_DEBUG";
# endif
  libname += ".DLL";
#elif defined(__APPLE__)
  libname += ".bundle";
#else
  libname += ".so";
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

#if defined(WIN32)
  string uc_filename(filename);
  std::transform(filename.begin(), filename.end(), uc_filename.begin(), toupper);
  string::size_type extension_pos = uc_filename.rfind("_DEBUG.DLL");
  if (uc_filename.npos == extension_pos) extension_pos = uc_filename.rfind(".DLL");
  string::size_type filename_pos = uc_filename.rfind("\\") + 1;
#elif defined(__APPLE__)
  string::size_type extension_pos = filename.rfind(".bundle");
  string::size_type filename_pos = filename.rfind("/") + 1;
#else
  string::size_type extension_pos = filename.rfind(".so");
  string::size_type filename_pos = filename.rfind("/") + 1;
#endif
  filename_pos += 5; // remove the "libsh" portion of the filename

  string backend_name = filename.substr(filename_pos, extension_pos - filename_pos);
  if (!is_valid_name(backend_name)) return false; // invalid backend -- don't load
 
  if (m_loaded_libraries->find(backend_name) != m_loaded_libraries->end()) {
    return true; // already loaded
  }

#if defined(WIN32)
  LibraryHandle module = LoadLibrary(filename.c_str());
#elif defined(__APPLE__)
  CFURLRef bundleURL;
  CFStringRef n = CFStringCreateWithCString(kCFAllocatorDefault, filename.c_str(), kCFStringEncodingASCII);

  bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, 
											n,
											kCFURLPOSIXPathStyle,
											true );
											
  CFRelease(n);											

  LibraryHandle module = CFBundleCreate( kCFAllocatorDefault, bundleURL );
  
  CFRelease( bundleURL );
#else
  LibraryHandle module = lt_dlopenext(filename.c_str());
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
# ifdef __APPLE__
      string::size_type extension_pos = filename.rfind(".bundle");
# else
      string::size_type extension_pos = filename.rfind(".so");

      // Skip files like "libsharb.so.0.0.0"
      const string so_extension(".so");
      if ((filename.size() - so_extension.size()) != extension_pos) continue; 
# endif
      if ((filename.find("libsh") == 0) && (std::string::npos != extension_pos)) {
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

  EntryPoint* instantiate = load_function<EntryPoint>(module, init_function_name.c_str());

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
  
  EntryPoint* func = load_function<EntryPoint>(module, init_function_name.c_str());

  if (func) {
     cost = (*func)(target);
  } else {
    SH_DEBUG_ERROR("Could not find " << init_function_name);
  }

  return cost;
}

void ShBackend::load_all_backends()
{
  if (m_all_backends_loaded) return;

#if defined(WIN32)
  load_libraries(string("."));
  
  // Search the system path
  string path(getenv("PATH"));
  unsigned length = path.size();
  unsigned start = 0;
  unsigned end = length - 1;
  while (start < length) {
    while ((start != end) && (' ' == path[start])) start++; // trim leading whitespace
    string::size_type separator = path.find(";", start);
    if (separator != path.npos) end = separator - 1;
    while ((start != end ) && (' ' == path[end])) end--; // trim trailing whitespace
    
    string entry = path.substr(start, end - start + 1);
    load_libraries(entry);
    
    if (path.npos == separator) break; // reached the end of the path
    start = separator + 1;
    end = length - 1;
  }
#elif defined(__APPLE__)
  load_libraries(string(getenv("HOME")) + "/Library/Sh/Backends");
  load_libraries("/Local/Library/Sh/Backends");
  load_libraries("/Library/Sh/Backends");
  load_libraries("/System/Library/Sh/Backends");
#else
  load_libraries(string(getenv("HOME")) + "/" + LOCAL_BACKEND_DIRNAME);
  load_libraries(string(SH_INSTALL_PREFIX) + "/lib/sh");
#endif

  m_all_backends_loaded = true;
}

string ShBackend::target_handler(const string& target, bool restrict_to_selected)
{
  init();

  bool selected_only = (m_selected_backends->size() > 0) && restrict_to_selected;
  if (!selected_only) {
    load_all_backends();
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

#if !defined(WIN32) && !defined(__APPLE__)
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

void ShBackend::check_target(const string& target, const string& generic_target, 
                             list<string>& derived_targets)
{
  load_all_backends();

  for (LibraryMap::iterator i = m_loaded_libraries->begin(); 
       i != m_loaded_libraries->end(); i++) {
    string backend_name = i->first;
    if (target_cost(backend_name, target) > 0) {
      derived_targets.push_back(backend_name + ":" + generic_target);
    }
  }
}

list<string> ShBackend::derived_targets(const string& target)
{
  list<string> ret;

  // Vertex shaders
  if (("vertex" == target) || ("*:vertex" == target)) {
    ret.push_back("gpu:vertex");
    check_target(target, "vertex", ret);
  } else if ("gpu:vertex" == target) {
    check_target(target, "vertex", ret);
  } 
  // Fragment shaders
  else if (("fragment" == target) || ("*:fragment" == target)) {
    ret.push_back("gpu:fragment");
    check_target(target, "fragment", ret);
  } else if ("gpu:fragment" == target) {
    check_target(target, "fragment", ret);
  } 
  // Stream programs
  else if (("stream" == target) || ("*:stream" == target)) {
    ret.push_back("cpu:stream");
    ret.push_back("gpu:stream");
    check_target(target, "stream", ret);
  } else if ("gpu:stream" == target) {
    check_target(target, "stream", ret);
  } else if ("cpu:stream" == target) {
    check_target(target, "stream", ret);
  }

  return ret;
}

}
