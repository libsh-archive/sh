#ifndef SHSHADER_HPP
#define SHSHADER_HPP

#include <list>
#include <map>
#include "ShRefCount.hpp"
#include "ShTokenizer.hpp"
#include "ShCtrlGraph.hpp"
#include "ShVariableNode.hpp"

namespace SH {

/// How many kinds of shaders there are in the pipeline at most.
const int shShaderKinds = 2;

class ShBackendCode;
class ShBackend;

/** A particular shader.
 * A shader consists mostly of a parser for its body and some metadata
 * (e.g. whether it is a vertex or a fragment shader).
 */
class ShShaderNode : public ShRefCountable {
public:
  ShShaderNode(int kind);

  /// Forcefully compile this shader for a particular backend, even if
  /// it has been compiled previously. Use code() to obtain the actual code.
  void compile(ShRefCount<ShBackend>& backend);
  
  /// Obtain the code for a particular backend. Generates it if necessary.
  ShRefCount<ShBackendCode> code(ShRefCount<ShBackend>& backend);

  /// Notify this shader that a uniform variable has changed.
  void updateUniform(const ShVariableNodePtr& uniform);

  /// The tokenizer for this shader's body
  ShTokenizer tokenizer;

  /// The control graph (the parsed form of the token list)
  ShCtrlGraphPtr ctrlGraph;

  /// Call after contructing the control graph [after optimization!]
  /// to make lists of all the variables used in the shader.
  void collectVariables();
  
  typedef std::list<ShVariableNodePtr> VarList;
  
  VarList inputs; ///< Input variables used in this shader
  VarList outputs; ///< Output variables used in this shader
  VarList temps; ///< Temporary variables used in this shader
  VarList constants; ///< Constants used in this shader
  VarList uniforms; ///< Uniform variables used in this shader

  int kind() const { return m_kind; }
  
private:

  void collectNodeVars(const ShCtrlGraphNodePtr& node);
  void collectVar(const ShVariableNodePtr& node);

  int m_kind;

  std::map< ShRefCount<ShBackend>, ShRefCount<ShBackendCode> > m_code;
};

typedef ShRefCount<ShShaderNode> ShShader;
 
}

#endif

