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

#include <list>
#include <map>
#include <utility>
#include <string>
#include "ShRefCount.hpp"
#include "ShTokenizer.hpp"
#include "ShVariableNode.hpp"
#include "ShCtrlGraph.hpp"

namespace SH {

class ShBackendCode;
class ShBackend;

/** A particular Sh program.
 */
class ShProgramNode : public ShRefCountable {
public:
  ShProgramNode(const std::string& target);

  ~ShProgramNode();
  
  /// Forcefully compile this shader for a particular backend, even if
  /// it has been compiled previously. Use code() to obtain the actual
  /// code.
  /// This operation will fail if this program does not have a
  /// particular target.
  void compile(ShRefCount<ShBackend>& backend);

  /// Forcefully compile this shader for a particular backend, even if
  /// it has been compiled previously. Use code() to obtain the actual code.
  void compile(const std::string& target, ShRefCount<ShBackend>& backend);
  
  /// Obtain the code for a particular backend. Generates it if necessary.
  /// This operation will fail if this program does not have a
  /// particular target.
  ShRefCount<ShBackendCode> code(ShRefCount<ShBackend>& backend);

  /// Obtain the code for a particular backend. Generates it if necessary.
  ShRefCount<ShBackendCode> code(const std::string& target, ShRefCount<ShBackend>& backend);

  /// Notify this shader that a uniform variable has changed.
  void updateUniform(const ShVariableNodePtr& uniform);

  /// The tokenizer for this shader's body. Used only during
  /// construction of the program (before parsing)
  ShTokenizer tokenizer;

  /// The control graph (the parsed form of the token
  /// list). Constructed during the parsing step, when shEndShader()
  /// is called.
  ShRefCount<ShCtrlGraph> ctrlGraph;

  /// Call after contructing the control graph [after optimization!]
  /// to make lists of all the variables used in the shader.
  void collectVariables();
  
  typedef std::list<ShVariableNodePtr> VarList;
  
  VarList inputs; ///< Input variables used in this shader
  VarList outputs; ///< Output variables used in this shader
  VarList temps; ///< Temporary variables used in this shader
  VarList constants; ///< Constants used in this shader
  VarList uniforms; ///< Uniform variables used in this shader
  VarList textures; ///< Textures used in this shader

  /// Can be empty, if there is no target associated with this program.
  std::string target() const { return m_target; }

  /// It may be useful to change a program's target sometimes.
  std::string& target() { return m_target; }
  
private:

  std::string m_target; ///< Can be empty, if there is no target associated with this program.
  
  void collectNodeVars(const ShRefCount<ShCtrlGraphNode>& node);
  void collectVar(const ShVariableNodePtr& node);

  std::map< std::pair< std::string, ShRefCount<ShBackend> >, ShRefCount<ShBackendCode> > m_code; ///< Compiled code is cached here.
};

typedef ShRefCount<ShProgramNode> ShProgram;

extern std::ostream& operator<<( std::ostream& out, const ShProgramNode::VarList &varList );
 
}

#endif

