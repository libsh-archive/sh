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

  /// Short name of the backend
  virtual std::string name() const { return "";}

  /// Backend-specific version number
  virtual std::string version() const = 0;

  /// Generate the backend code for a particular shader.
  virtual ShBackendCodePtr generate_code(const std::string& target,
                                         const ShProgramNodeCPtr& shader) = 0;

  virtual ShBackendSetPtr generate_set(const ShProgramSet& s);
  
  /// Execute a stream program, if supported
  virtual void execute(const ShProgramNodeCPtr& program, ShStream& dest) = 0;

  /// Unbind all programs bound under this backend
  virtual void unbind_all();
  
  /// Returns true if the backend can handle the given target
  virtual bool can_handle(const std::string& target) { return false; }


  typedef std::list< ShPointer<ShBackend> > ShBackendList;

  static ShBackendList::iterator begin();
  static ShBackendList::iterator end();

  static ShPointer<ShBackend> lookup(const std::string& name);

  /// Load the given library and initialize the backend it contains
  static void ShBackend::load_library(const std::string& filename);

  /// Add a backend to the list of selected backends
  static bool use_backend(const std::string& name);

  /// Clear the list of selected backends
  static void clear_backends();

  /// Returns a backend that can run the specified target
  static ShPointer<ShBackend> get_backend(const std::string& target);

protected:
  ShBackend();
  
  /// Returns true if the target is compatible with the backend if we
  /// consider the generic type of the backend (i.e. "cpu" or "gpu")
  /// and the kind of program it refers to (e.g. "stream", "vertex" or
  /// "fragment").
  bool can_handle(const std::string& target, const char* type, const char* kind);
  
private:
  static void init();

  static ShBackendList* m_instantiated_backends;
  static ShBackendList* m_selected_backends;
  static std::set<std::string>* m_selected_backend_names;
  static bool m_doneInit;
  static bool m_all_backends_loaded;
};

typedef ShPointer<ShBackend> ShBackendPtr;
typedef ShPointer<const ShBackend> ShBackendCPtr;

}

#endif
