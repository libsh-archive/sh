#ifndef SHSHADER_HPP
#define SHSHADER_HPP

#include "ShRefCount.hpp"
#include "ShTokenizer.hpp"
#include "ShCtrlGraph.hpp"

namespace SH {

/** A particular shader.
 * A shader consists mostly of a parser for its body and some metadata
 * (e.g. whether it is a vertex or a fragment shader).
 */
class ShShaderNode : public ShRefCountable {
public:
  ShShaderNode(int kind);

  /// The tokenizer for this shader's body
  ShTokenizer tokenizer;

  /// The control graph (the parsed form of the token list)
  ShCtrlGraphPtr ctrlGraph;
  
private:
  int m_kind;
};

typedef ShRefCount<ShShaderNode> ShShader;
 
}

#endif
