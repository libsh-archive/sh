// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#ifndef SHPROGRAM_HPP
#define SHPROGRAM_HPP

#include "ShDllExport.hpp"
#include "ShProgramNode.hpp"
#include "ShBackend.hpp"

namespace SH {

/** Thin wrapper around ShProgramNode.
 */
class
SH_DLLEXPORT ShProgram : public ShMetaForwarder {
public:
  ShProgram();
  ShProgram(const ShProgram& other);
  ShProgram(const std::string& target);
  ShProgram(const ShProgramNodePtr& node);

  ShProgram& operator=(const ShProgram& other);
  
  /// Obtain the node which this ShProgram wraps
  ShProgramNodeCPtr node() const { return m_node; }

  /// Obtain the node which this ShProgram wraps
  ShProgramNodePtr node() { return m_node; }
  
  /// Forcefully compile this program for a particular backend, even if
  /// it has been compiled previously. Use code() to obtain the actual
  /// code.
  /// This operation will fail if this program does not have a
  /// particular target.
  void compile(const ShPointer<ShBackend>& backend) { m_node->compile(backend); }

  /// Forcefully compile this program for a particular backend, even if
  /// it has been compiled previously. Use code() to obtain the actual code.
  void compile(const std::string& target, const ShPointer<ShBackend>& backend)
  {
    m_node->compile(target, backend);
  }

  /// Obtain a listing of the inputs, outputs and uniforms used by
  /// this program.
  std::string describe_interface() const
  {
    return m_node->describe_interface();
  }
  
  /// Obtain the code for currently active backend. 
  /// This operation will fail if this program does not have a
  /// particular target.
  ShPointer<ShBackendCode> code() { return m_node->code(); }
  
  /// Obtain the code for a particular backend. Generates it if necessary.
  /// This operation will fail if this program does not have a
  /// particular target.
  ShPointer<ShBackendCode> code(const ShPointer<ShBackend>& backend) {
    return m_node->code(backend);
  }

  /// Obtain the code for a particular backend. Generates it if necessary.
  ShPointer<ShBackendCode> code(const std::string& target, const ShPointer<ShBackend>& backend)
  {
    return m_node->code(target, backend);
  }

  /// Notify this program that a uniform variable has changed.
  void updateUniform(const ShVariableNodePtr& uniform)
  {
    m_node->updateUniform(uniform);
  }

  std::string target() const { return m_node->target(); }

  /// It may be useful to change a program's target sometimes.
  std::string& target() { return m_node->target(); }
  
private:

  ShProgramNodePtr m_node;
};

}

#endif

