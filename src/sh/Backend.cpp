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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(_WIN32)
# include <windows.h>
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
# include <CoreFoundation/CoreFoundation.h>
# include <dirent.h>
#else
# include <ltdl.h>
# include <dirent.h>
# define LOCAL_BACKEND_DIRNAME ".shbackends"
#endif

#include "Backend.hpp"
#include "Program.hpp"
#include "Debug.hpp"
#include "Internals.hpp"
#include "Transformer.hpp"
#include "Syntax.hpp"

#include <algorithm>
#include <cctype>
#include "binreloc.h"

namespace {

template<typename EntryPoint>
EntryPoint* load_function(SH::Backend::LibraryHandle module, const char* name)
{
#if defined(_WIN32)
  return (EntryPoint*)GetProcAddress((HMODULE)module, name);  
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
  CFStringRef n = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
  EntryPoint* p = (EntryPoint*)CFBundleGetFunctionPointerForName(module, n);
  CFRelease(n);
  return p;
#else
  return (EntryPoint*)lt_dlsym(module, name); 
#endif
}

void get_params(const SH::BaseTexture& stream,
                int& offset, int& stride, int& repeat, int& count)
{
  stream.get_offset(&offset, 1);
  stream.get_stride(&stride, 1);
  stream.get_repeat(&repeat, 1);
  stream.get_count(&count, 1);
}                

class Gather {
public:
  Gather(const SH::BaseTexture& source)
  {
    m_size = source.node()->size() * 
             SH::typeInfo(source.node()->valueType(), SH::MEM)->datasize();
  }
  void operator()(char *src, int src_elt, char *dest, int dest_elt) const
  {
    memcpy(dest + dest_elt*m_size, src + src_elt*m_size, m_size);
  }
private:
  int m_size;
};

class Scatter {
public:
  Scatter(const SH::BaseTexture& source)
  {
    m_size = source.node()->size() * 
             SH::typeInfo(source.node()->valueType(), SH::MEM)->datasize();
  }
  void operator()(char *dest, int dest_elt, char *src, int src_elt) const
  {
    memcpy(dest + dest_elt*m_size, src + src_elt*m_size, m_size);
  }
private:
  int m_size;
};

template <typename Functor>
void do_gather_scatter_1d(const SH::BaseTexture& stream1,
                          const SH::BaseTexture& stream2,
                          const SH::BaseTexture& index,
                          const Functor& functor)
{
  SH::HostStoragePtr stream1_storage = 
    SH::shref_dynamic_cast<SH::HostStorage>(stream1.node()->memory(0)->findStorage("host"));
  SH::HostStoragePtr stream2_storage = 
    SH::shref_dynamic_cast<SH::HostStorage>(stream2.node()->memory(0)->findStorage("host"));
  SH::HostStoragePtr index_storage = 
    SH::shref_dynamic_cast<SH::HostStorage>(index.node()->memory(0)->findStorage("host"));
  if (!stream1_storage || !stream2_storage || !index_storage) {
    SH::error(SH::Exception("No storage assocated with stream"));
  }
  
  int index_offset, index_stride, index_repeat, index_count;
  get_params(index, index_offset, index_stride, index_repeat, index_count);
  int s1_offset, s1_stride, s1_repeat, s1_count;
  get_params(stream1, s1_offset, s1_stride, s1_repeat, s1_count);
  int s2_offset, s2_stride, s2_repeat, s2_count;
  get_params(stream2, s2_offset, s2_stride, s2_repeat, s2_count);

  SH::VariantPtr index_variant = SH::variantFactory(index_storage->value_type(), SH::MEM)->
    generate(index_storage->length() / index_storage->value_size(),
             const_cast<void*>(index_storage->data()), false);
  if (index_storage->value_type() != SH::SH_INT &&
      index_storage->value_type() != SH::SH_UINT) {
    // TODO: we shouldn't convert [u]{s,b}
    SH::VariantPtr converted_variant = SH::variantFactory(SH::SH_INT, SH::MEM)->
      generate(index_storage->length() / index_storage->value_size());
    converted_variant->set(index_variant);
    index_variant = converted_variant;
  }
  
  int* index_ptr = reinterpret_cast<int*>(index_variant->array());
  char* stream1_ptr = reinterpret_cast<char*>(stream1_storage->data());
  char* stream2_ptr = reinterpret_cast<char*>(stream2_storage->data());

  for (int i = 0; i < index_count; ++i) {
    int index_elt = ((i * index_stride / index_repeat) % index_count) + index_offset;
    int stream1_elt = (index_ptr[index_elt] * s1_stride / s1_repeat) % s1_count + s1_offset;
    int stream2_elt = ((i * s2_stride / s2_repeat) % s2_count) + s2_offset;
    functor(stream1_ptr, stream1_elt, stream2_ptr, stream2_elt);
  }
}

}

namespace SH {

using namespace std;

Backend::BackendMap* Backend::m_instantiated_backends = 0;
Backend::BackendSet* Backend::m_selected_backends = 0;
Backend::LibraryMap* Backend::m_loaded_libraries = 0;
bool Backend::m_done_init = false;
bool Backend::m_all_backends_loaded = false;

BackendCode::~BackendCode()
{
}

BackendSet::~BackendSet()
{
}

// A default implementation of BackendSet, usable for any backends
// that don't involve special linking.
class TrivialBackendSet : public BackendSet {
public:
  TrivialBackendSet(const ProgramSet& s,
                      const BackendPtr& backend);
  
  virtual ~TrivialBackendSet();

  virtual void link();

  virtual void bind();
  virtual void unbind();

protected:
  list<BackendCodePtr> m_code;
};

typedef Pointer<TrivialBackendSet> TrivialBackendSetPtr;
typedef Pointer<const TrivialBackendSet> TrivialBackendSetCPtr;

TrivialBackendSet::TrivialBackendSet(const ProgramSet& s,
                                         const BackendPtr& backend)
{
  for (ProgramSet::const_iterator I = s.begin(); I != s.end(); ++I) {
    m_code.push_back((*I)->code(backend));
  }
}

TrivialBackendSet::~TrivialBackendSet()
{
}

void TrivialBackendSet::link()
{
}

void TrivialBackendSet::bind()
{
  for (list<BackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->bind();
  }
}

void TrivialBackendSet::unbind()
{
  // TODO: This may not quite have the correct semantics
  for (list<BackendCodePtr>::iterator I = m_code.begin(); I != m_code.end(); ++I) {
    (*I)->unbind();
  }
}
 
Backend::Backend(const string& name, const string& version)
  : m_name(name), m_version(version)
{
}

Backend::~Backend()
{
  SH_DEBUG_ASSERT(!m_name.empty());
  SH_DEBUG_ASSERT(m_loaded_libraries->find(m_name) != m_loaded_libraries->end());
  LibraryHandle handle = (*m_loaded_libraries)[m_name]->handle();
  
  // handle may be null if library was loaded by app (i.e. statically linked)
  if (handle) {
#if defined(_WIN32)
    if (!FreeLibrary((HMODULE)handle)) {
      SH_DEBUG_ERROR("Could not unload the " << m_name << " library.");
    }
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
    CFRelease(handle);
#else
    if (!lt_dlclose(handle)) {
      SH_DEBUG_ERROR("Could not unload the " << m_name << " library: " << lt_dlerror());
    }
#endif
  }

  m_instantiated_backends->erase(m_name);
  m_loaded_libraries->erase(m_name);
  m_selected_backends->erase(m_name);
}

BackendSetPtr Backend::generate_set(const ProgramSet& s)
{
  return new TrivialBackendSet(s, this);
}

string Backend::lookup_filename(const string& backend_name)
{
  init();
  string libname = "libsh" + backend_name;

#if defined(_WIN32)
# ifdef SH_DEBUG
    libname += "_DEBUG";
# endif
  libname += ".DLL";
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
  libname += ".bundle";
#elif defined(__APPLE__) && defined(AUTOTOOLS)
  libname += ".la";
#else
  libname += ".so";
#endif

  return libname;
}

bool Backend::is_valid_name(const string& backend_name)
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

void Backend::register_backend(const std::string& backend_name, InstantiateEntryPoint *instantiate, TargetCostEntryPoint *target_cost)
{
  init();
  
  if (m_loaded_libraries->find(backend_name) != m_loaded_libraries->end()) {
    return; // already loaded
  }

  (*m_loaded_libraries)[backend_name] = new LibraryInformation(NULL, instantiate, target_cost);
}

bool Backend::load_library(const string& filename)
{
  init();

  if (filename.empty()) return false;

#if defined(_WIN32)
  string uc_filename(filename);
  std::transform(filename.begin(), filename.end(), uc_filename.begin(), toupper);
  string::size_type extension_pos = uc_filename.rfind("_DEBUG.DLL");
  if (uc_filename.npos == extension_pos) {
#ifdef SH_DEBUG
    // refuse to load non-debugging libraries
    return false;
#else
    extension_pos = uc_filename.rfind(".DLL");
#endif
  }
  string::size_type filename_pos = uc_filename.rfind("\\") + 1;
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
  string::size_type extension_pos = filename.rfind(".bundle");
  string::size_type filename_pos = filename.rfind("/") + 1;
#elif defined(__APPLE__) && defined(AUTOTOOLS)
  string::size_type extension_pos = filename.rfind(".la");
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

  const char* errormsg = "";
#if defined(_WIN32)
  LibraryHandle module = LoadLibrary(filename.c_str());
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
  CFURLRef bundleURL;

  CFStringRef n = CFStringCreateWithCString(kCFAllocatorDefault, filename.c_str(), kCFStringEncodingASCII);
  bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, n, kCFURLPOSIXPathStyle, true );
  CFRelease(n);

  LibraryHandle module = CFBundleCreate( kCFAllocatorDefault, bundleURL );
  CFRelease( bundleURL );
#else
  LibraryHandle module = lt_dlopenext(filename.c_str());
  errormsg = static_cast<const char*>(lt_dlerror());
#endif

  if (!module) {
    SH_DEBUG_WARN("Could not open " << filename);
    if (errormsg != "") {
      SH_DEBUG_WARN("lt_dlerror: " << errormsg);
    }
    return false;
  }

  string init_function_name = "backend_libsh" + backend_name + "_instantiate";
  string target_cost_function_name = "backend_libsh" + backend_name + "_target_cost";
  
  InstantiateEntryPoint* instantiate = load_function<InstantiateEntryPoint>(module, init_function_name.c_str());
  TargetCostEntryPoint* target_cost = load_function<TargetCostEntryPoint>(module, target_cost_function_name.c_str());

  if (!instantiate) {
    SH_DEBUG_ERROR("Could not find " << init_function_name);
  }

  if (!target_cost) {
    SH_DEBUG_ERROR("Could not find " << target_cost_function_name);
  }

  (*m_loaded_libraries)[backend_name] = new LibraryInformation(module, instantiate, target_cost);
  return true;
}

void Backend::load_libraries(const string& directory)
{
#ifndef _WIN32
  DIR* dirp = opendir(directory.c_str());
  if (dirp) {
    // Go through all files in lib/sh/
    for (struct dirent* entry = readdir(dirp); entry != 0; entry = readdir(dirp)) {
      string filename(entry->d_name);
# if defined(__APPLE__) && !defined(AUTOTOOLS)
      string::size_type extension_pos = filename.rfind(".bundle");
# elif defined(__APPLE__) && defined(AUTOTOOLS)
      string::size_type extension_pos = filename.rfind(".la");
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
#endif /* _WIN32 */
}


BackendPtr Backend::instantiate_backend(const string& backend_name)
{
  init();
  Backend* backend = 0;

  if (m_instantiated_backends->find(backend_name) != m_instantiated_backends->end()) {
    return (*m_instantiated_backends)[backend_name];
  }

  SH_DEBUG_ASSERT(m_loaded_libraries->find(backend_name) != m_loaded_libraries->end());
  InstantiateEntryPoint* instantiate = (*m_loaded_libraries)[backend_name]->instantiate_function();

  if (instantiate) {
    backend = (*instantiate)();
  }

  if (backend) {
    (*m_instantiated_backends)[backend_name] = backend;
  }

  return backend;
}

bool Backend::use_backend(const string& backend_name)
{
  init();
  m_selected_backends->insert(backend_name);
  return load_library(lookup_filename(backend_name));
}

bool Backend::have_backend(const string& backend_name)
{
  init();
  return load_library(lookup_filename(backend_name));
}

void Backend::clear_backends()
{
  init();
  m_selected_backends->clear();
}

int Backend::target_cost(const string& backend_name, const string& target)
{
  init();
  int cost = 0;

  SH_DEBUG_ASSERT(m_loaded_libraries->find(backend_name) != m_loaded_libraries->end());
  TargetCostEntryPoint* func = (*m_loaded_libraries)[backend_name]->target_cost_function();

  if (func) {
    cost = (*func)(target);
  }

  return cost;
}

void Backend::load_all_backends()
{
  if (m_all_backends_loaded) return;

#if defined(_WIN32)
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
#else
  string backend_dir = "";
  {
    const char* env_var = getenv("BACKEND_DIR");
    if (env_var) {
      backend_dir = string(env_var);
    }
  }

  if (backend_dir.empty()) {
# if defined(__APPLE__) && !defined(AUTOTOOLS)
    load_libraries(string(getenv("HOME")) + "/Library/Sh/Backends");
    load_libraries("/Local/Library/Sh/Backends");
    load_libraries("/Library/Sh/Backends");
    load_libraries("/System/Library/Sh/Backends");
# else
    load_libraries(string(getenv("HOME")) + "/" + LOCAL_BACKEND_DIRNAME);
    char* install_prefix = br_find_prefix(SH_INSTALL_PREFIX);
    load_libraries(string(install_prefix) + "/lib/sh");
    free(install_prefix);
# endif
  } else {
    load_libraries(backend_dir);
  }
#endif

  m_all_backends_loaded = true;
}

string Backend::target_handler(const string& target, bool restrict_to_selected)
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

Pointer<Backend> Backend::get_backend(const string& target)
{
  init();

  string best_backend = target_handler(target, true);
  
  if (!best_backend.empty()) {
    return instantiate_backend(best_backend);
  } else {
    cerr << "Could not find a backend that supports the '" << target << "' target." << endl;
    if (m_selected_backends->size() > 0) {
      cerr << "Try removing all useBackend() and setBackend() calls to search all installed backends." << endl;
    }
    return 0;
  }
}

void Backend::init()
{
  if (m_done_init) return;
  
  m_instantiated_backends = new BackendMap();
  m_selected_backends = new BackendSet();
  m_loaded_libraries = new LibraryMap();

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(AUTOTOOLS))
  if (lt_dlinit()) {
    SH_DEBUG_ERROR("Error initializing ltdl: " << lt_dlerror());
  }

  string userpath(getenv("HOME"));
  userpath += "/";
  userpath += LOCAL_BACKEND_DIRNAME;
  if (lt_dladdsearchdir(userpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + userpath + " to search dir: " << lt_dlerror());
  }

  char* install_prefix = br_find_prefix(SH_INSTALL_PREFIX);
  string searchpath(install_prefix);
  searchpath += "/lib/sh";
  free(install_prefix);
  if (lt_dladdsearchdir(searchpath.c_str())) {
    SH_DEBUG_ERROR("Could not add " + searchpath + " to search dir: " << lt_dlerror());
  }
#endif /* _WIN32 */

  m_done_init = true;
}

void Backend::unbind_all_programs()
{
  while (Context::current()->begin_bound() != Context::current()->end_bound()) {
    unbind(Context::current()->begin_bound()->second);
  }
}

void Backend::unbind_all_backends()
{
  init();
  for (BackendMap::iterator i = m_instantiated_backends->begin();
       i != m_instantiated_backends->end(); i++) {
    i->second->unbind_all_programs();
  }
}

void Backend::check_target(const string& target, const string& generic_target, 
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

list<string> Backend::derived_targets(const string& target)
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

void Backend::gather(const SH::BaseTexture& dest,
                     const SH::BaseTexture& src,
                     const SH::BaseTexture& index)
{
  if (src.node()->dims() != SH::SH_TEXTURE_1D) {
    SH::error(SH::Exception("Only 1D gather is currently supported"));
  }
  do_gather_scatter_1d(src, dest, index, Gather(src));
}

void Backend::scatter(const SH::BaseTexture& dest,
                      const SH::BaseTexture& index,
                      const SH::BaseTexture& src)
{
  if (src.node()->dims() != SH::SH_TEXTURE_1D) {
    SH::error(SH::Exception("Only 1D scatter is currently supported"));
  }
  do_gather_scatter_1d(dest, src, index, Scatter(src));
}

}
