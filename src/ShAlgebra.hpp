#ifndef SHALGEBRA_HPP
#define SHALGEBRA_HPP

#include <string>
#include "ShProgram.hpp"
#include "ShSyntax.hpp"
#include "ShAttrib.hpp"

namespace SH {

/// Replace inputs of b with outputs of a and connect them together.
// Let A = a->outputs.size(), B = b->inputs.size().
// If A > B, extra outputs are kept at end
// If A < B, extra inputs are kept at end
ShProgram connect(const ShProgram& a, const ShProgram& b);

/// Run a and b together (i.e. use both inputs from a and b and both
/// outputs from a and b).
ShProgram combine(const ShProgram& a, const ShProgram& b);

/// Run a and b together (i.e. use both inputs from a and b and both
// outputs from a and b).
// This combine detects pairs of inputs with matching names and types.
// If this occurs, the later input is discarded and replaced with
// the earlier one.
//
// E.g. if a has inputs x, y, k, x, z and b has inputs w, y, x, v
// The result has inputs x, y, k, z, w, v
ShProgram namedCombine(const ShProgram& a, const ShProgram& b);

/// Replace inputs of b with outputs of a and connect them together.
// The outputs of a and inputs of b must all be named.
// 
// For each output of a in positional order, this connects the output with an 
// input of b of the same name/type that is not already connected with
// another output of a.
// Extra inputs remain at the end.  Extra outputs remain iff keepExtra = true 
ShProgram namedConnect(const ShProgram& a, const ShProgram& b, bool keepExtra = false );

// Renames all inputs named oldName to newName
ShProgram renameInput(const ShProgram &a, std::string oldName, std::string newName);

// Renames all outputs named oldName to newName
ShProgram renameOutput(const ShProgram &a, std::string oldName, std::string newName);

// Swizzles outputs of a to match inputs of b
// This only works on programs with inputs/outputs that all have unique names 
// Also, the inputs of b must be a subset of the outputs of a
ShProgram namedAlign(const ShProgram &a, const ShProgram &b);

/// Replaces uniform by appending an input to the end of the list
ShProgram replaceUniform(const ShProgram &a, const ShVariable &var); 

/// Equiv. to combine(a,b)
ShProgram operator&(const ShProgram& a, const ShProgram& b);

/// Equiv. to connect(b,a)
ShProgram operator<<(const ShProgram& a, const ShProgram& b);

/// Equiv. to replaceUniform( p, var );
ShProgram operator>>(const ShProgram &p, const ShVariable &var); 

/// Currying operator
template<int N, typename T>
ShProgram operator<<(const ShProgram& a, const ShVariableN<N, T>& v) {
  ShProgram vNibble = SH_BEGIN_PROGRAM() {
    ShAttrib<N, SH_OUTPUT, T> out;
    out.node()->specialType(v.node()->specialType());
    out = v;
  } SH_END_PROGRAM;
  return connect(vNibble, a); 
}

}

#endif
