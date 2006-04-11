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
#ifndef SHBACKEND_HPP
#define SHBACKEND_HPP

#include <vector>
#include <iosfwd>
#include <string>
#include "DllExport.hpp"
#include "RefCount.hpp"
#include "Program.hpp"
#include "ProgramSet.hpp"
#include "VariableNode.hpp"

#if defined(_WIN32)
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
#include <CoreFoundation/CoreFoundation.h>
#else
  struct lt_dlhandle_struct;
#endif

namespace SH  {

class Stream;

class
DLLEXPORT BackendCode : public RefCountable {
public:
  virtual ~BackendCode();

  /// Used by a register allocater to signal that a register should be
  /// allocated to var. Return true iff the allocation succeeded.
  virtual bool allocateRegister(const VariableNodePtr& var) = 0;

  /// Used by the register allocator to signal that the register used
  /// by var can be used by other registers in future
  /// allocateRegister() calls.
  virtual void freeRegister(const VariableNodePtr& var) = 0;
  
  /// Upload this shader code to the GPU.
  virtual void upload() = 0;

  /// Bind this shader code after it has been uploaded.
  virtual void bind() = 0;

  /// Unbind this shader.
  virtual void unbind() = 0;

  /// Upload any textures and uniform parameters which are out-of-date but required
  virtual void update() = 0;

  /// Update the value of a uniform parameter after it has changed.
  virtual void updateUniform(const VariableNodePtr& uniform) = 0;

  virtual std::ostream& print(std::ostream& out) = 0;

  /// Prints input and output specification in target-specific format
  // (Useful for how to format long tuple input on targets 
  // that only support limited tuple lengths) 
  virtual std::ostream& describe_interface(std::ostream& out) = 0;

  /// Prints binding description in target-specific format
  virtual std::ostream& describe_bindings(std::ostream& out) = 0;
};

typedef Pointer<BackendCode> BackendCodePtr;
typedef Pointer<const BackendCode> BackendCodeCPtr;

// A backend-specific set of programs, to be linked together/bound at once
class
DLLEXPORT BackendSet : public RefCountable {
public:
  virtual ~BackendSet();

  virtual void link() = 0;

  virtual void bind() = 0;
  virtual void unbind() = 0;
};

typedef Pointer<BackendSet> BackendSetPtr;
typedef Pointer<const BackendSet> BackendSetCPtr;

class Transformer;
class
DLLEXPORT Backend : public RefCountable {
public:
  virtual ~Backend();

  /** Short name of the backend (e.g. "arb", "cc", "glsl")*/
  std::string name() const { return m_name;}

  /** Backend-specific version number */
  std::string version() const { return m_version; }

  /** Generate the backend code for a particular shader. */
  virtual BackendCodePtr generate_code(const std::string& target,
                                         const ProgramNodeCPtr& shader) = 0;

  virtual BackendSetPtr generate_set(const ProgramSet& s);
  
  /** Execute a stream program, if supported */
  virtual void execute(const ProgramNodeCPtr& program, Stream& dest) = 0;
  
  /** Unbind all programs bound under the backend */
  virtual void unbind_all_programs();

  /** Unbind all programs bound under all backends */
  static void unbind_all_backends();

  /** Add a backend to the list of selected backends */
  static bool use_backend(const std::string& backend_name);

  /** Checks whether the backend is available or not */
  static bool have_backend(const std::string& backend_name);

  /** Clear the list of selected backends */
  static void clear_backends();

  /** Find the name of the best backend that handles the given target */
  static std::string target_handler(const std::string& target, bool restrict_to_selected);

  /** Returns a backend that can run the specified target */
  static Pointer<Backend> get_backend(const std::string& target);

  /** Returns a list of targets derived from a generic target
      name. e.g. "arb:vertex, glsl:vertex" is returned when the
      generic target is "gpu:vertex". */
  static std::list<std::string> derived_targets(const std::string& target);

  typedef Backend* InstantiateEntryPoint();
  typedef int TargetCostEntryPoint(const std::string &);

  /** Add a backend to the list of loaded backends. */
  static void register_backend(const std::string& backend_name, InstantiateEntryPoint *instantiate, TargetCostEntryPoint *target_cost);
  
#if defined(_WIN32)
  typedef void* LibraryHandle;
#elif defined(__APPLE__) && !defined(AUTOTOOLS)
  typedef CFBundleRef LibraryHandle;
#else
  typedef lt_dlhandle_struct* LibraryHandle;
#endif
  
  class LibraryInformation : public RefCountable {
  public:
    LibraryInformation(LibraryHandle handle, InstantiateEntryPoint* instantiate_function, TargetCostEntryPoint* target_cost_function) :
      m_handle(handle), m_instantiate_function(instantiate_function), m_target_cost_function(target_cost_function)
    {}
    
    LibraryHandle handle() { return m_handle; }
    InstantiateEntryPoint* instantiate_function() { return m_instantiate_function; }
    TargetCostEntryPoint* target_cost_function() { return m_target_cost_function; }
  private: 
    LibraryHandle m_handle;
    InstantiateEntryPoint* m_instantiate_function;
    TargetCostEntryPoint* m_target_cost_function;
  };

  typedef std::map<std::string, Pointer<LibraryInformation> > LibraryMap;
  typedef std::map<std::string, Pointer<Backend> > BackendMap;
  typedef std::set<std::string> BackendSet;
  
protected:
  Backend(const std::string& name, const std::string& version);
  
private:
  static BackendMap* m_instantiated_backends;
  static BackendSet* m_selected_backends;
  static LibraryMap* m_loaded_libraries;
  static bool m_done_init;
  static bool m_all_backends_loaded;

  std::string m_name;
  std::string m_version;

  /** Initialize the data structures */
  static void init();

  /** Convert the backend name (e.g. "arb") to a filename
   * Returns an empty string if a library cannot be found. */
  static std::string lookup_filename(const std::string& backend_name);

  /** Check that the backend name doesn't contain invalid characters. */
  static bool is_valid_name(const std::string& backend_name);

  /** Load all installed backends */
  static void load_all_backends();

  /** Load the given library and initialize the backend it contains
   * Returns true if the library was loaded successfully. */
  static bool load_library(const std::string& filename);

  /** Load all libraries contained in the given directory */
  static void load_libraries(const std::string& directory);

  /** Return the cost of the given target on the given backend
   * (0 == not handled, 1 == perfect match, 2 == very good, etc.) */
  static int target_cost(const std::string& backend_name, const std::string& target);
  
  /** Add "backend:generic_target" to the derived_targets list for all
   * backends that support the given target */
  static void check_target(const std::string& target, const std::string& generic_target, 
                           std::list<std::string>& derived_targets);

  /** Instantiate and initialize the backend */
  static Pointer<Backend> instantiate_backend(const std::string& backend_name);
};

typedef Pointer<Backend> BackendPtr;
typedef Pointer<const Backend> BackendCPtr;

}

#endif
