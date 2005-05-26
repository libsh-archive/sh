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
#ifndef SHBACKEND_HPP
#define SHBACKEND_HPP

#include <vector>
#include <iosfwd>
#include <string>
#include "ShDllExport.hpp"
#include "ShRefCount.hpp"
#include "ShProgram.hpp"
#include "ShProgramSet.hpp"
#include "ShVariableNode.hpp"

#ifndef WIN32
  struct lt_dlhandle_struct;
#endif

namespace SH  {

class ShStream;

class
SH_DLLEXPORT ShBackendCode : public ShRefCountable {
public:
  virtual ~ShBackendCode();

  /// Used by a register allocater to signal that a register should be
  /// allocated to var. Return true iff the allocation succeeded.
  virtual bool allocateRegister(const ShVariableNodePtr& var) = 0;

  /// Used by the register allocator to signal that the register used
  /// by var can be used by other registers in future
  /// allocateRegister() calls.
  virtual void freeRegister(const ShVariableNodePtr& var) = 0;
  
  /// Upload this shader code to the GPU.
  virtual void upload() = 0;

  /// Bind this shader code after it has been uploaded.
  virtual void bind() = 0;

  /// Unbind this shader.
  virtual void unbind() = 0;

  /// Upload any textures and uniform parameters which are out-of-date but required
  virtual void update() = 0;

  /// Update the value of a uniform parameter after it has changed.
  virtual void updateUniform(const ShVariableNodePtr& uniform) = 0;

  virtual std::ostream& print(std::ostream& out) = 0;

  /// Prints input and output specification in target-specific format
  // (Useful for how to format long tuple input on targets 
  // that only support limited tuple lengths) 
  virtual std::ostream& describe_interface(std::ostream& out) = 0;
};

typedef ShPointer<ShBackendCode> ShBackendCodePtr;
typedef ShPointer<const ShBackendCode> ShBackendCodeCPtr;

// A backend-specific set of programs, to be linked together/bound at once
class
SH_DLLEXPORT ShBackendSet : public ShRefCountable {
public:
  virtual ~ShBackendSet();

  virtual void link() = 0;

  virtual void bind() = 0;
  virtual void unbind() = 0;
};

typedef ShPointer<ShBackendSet> ShBackendSetPtr;
typedef ShPointer<const ShBackendSet> ShBackendSetCPtr;

class ShTransformer;
class
SH_DLLEXPORT ShBackend : public ShRefCountable {
public:
  virtual ~ShBackend();

  /** Short name of the backend (e.g. "arb", "cc", "glsl")*/
  virtual std::string name() const { return "";}

  /** Backend-specific version number */
  virtual std::string version() const = 0;

  /** Generate the backend code for a particular shader. */
  virtual ShBackendCodePtr generate_code(const std::string& target,
                                         const ShProgramNodeCPtr& shader) = 0;

  virtual ShBackendSetPtr generate_set(const ShProgramSet& s);
  
  /** Execute a stream program, if supported */
  virtual void execute(const ShProgramNodeCPtr& program, ShStream& dest) = 0;
  
  /** Unbind all programs bound under the backend */
  virtual void unbind_all_programs();

  /** Unbind all programs bound under all backends */
  static void unbind_all_backends();

  /** Add a backend to the list of selected backends */
  static bool use_backend(const std::string& name);

  /** Clear the list of selected backends */
  static void clear_backends();

  /** Returns a backend that can run the specified target */
  static ShPointer<ShBackend> get_backend(const std::string& target);

#ifndef WIN32
  typedef lt_dlhandle_struct* LibraryHandle;
#else
  typedef void* LibraryHandle;
#endif
  typedef std::map<std::string, LibraryHandle> LibraryMap;
  typedef std::map<std::string, ShPointer<ShBackend> > BackendMap;
  typedef std::set<std::string> BackendSet;

protected:
  ShBackend();
  
private:
  /* The string related to the backend name in these data is stored as
     "libsharb", "libshcc", "libshglsl", etc. */
  static BackendMap* m_instantiated_backends;
  static BackendSet* m_selected_backends;
  static LibraryMap* m_loaded_libraries;
  static bool m_done_init;
  static bool m_all_backends_loaded;

  /** Initialize the data structures */
  static void init();

  /** Convert the user-visible backend name (e.g. "arb") to a filename
   * Returns an empty string if a library cannot be found. */
  static std::string lookup_filename(const std::string& name);

  /** Load the given library and initialize the backend it contains
   * Returns true if the library was loaded successfully. */
  static bool load_library(const std::string& filename);

  /** Load all libraries contained in the given directory */
  static void load_libraries(const std::string& directory);

  /** Return the cost of the given target on the given backend
   * (0 == not handled, 1 == perfect match, 2 == very good, etc.) */
  static int target_cost(const std::string& backend_name, const std::string& target);
  
  /** Instantiate and initialize the backend */
  static ShPointer<ShBackend> instantiate_backend(const std::string& backend_name);
};

typedef ShPointer<ShBackend> ShBackendPtr;
typedef ShPointer<const ShBackend> ShBackendCPtr;

}

#endif
