#ifndef SHALGEBRA_HPP
#define SHALGEBRA_HPP

#include <string>
#include "ShProgram.hpp"
#include "ShSyntax.hpp"
#include "ShAttrib.hpp"

namespace SH {

/// Replace inputs of b with outputs of a and connect them together.
//
// If B = b->inputs.size() > a->outputs.size() = A,
// result takes inputs of a, the last B-A inputs of b,
// and outputs results of b.
ShProgram connect(const ShProgram& a, const ShProgram& b);

/// Run a and b together (i.e. use both inputs from a and b and both
/// outputs from a and b).
ShProgram combine(const ShProgram& a, const ShProgram& b);

/// Replaces uniform by appending an input to the end of the list
ShProgram replaceUniform(const ShProgram &a, const ShVariable &var); 

/// Equiv. to combine(a,b)
ShProgram operator&(const ShProgram& a, const ShProgram& b);

/// Equiv. to connect(b,a)
ShProgram operator<<(const ShProgram& a, const ShProgram& b);

/// Equiv. to connect(a,b)
ShProgram operator>>(const ShProgram& a, const ShProgram& b);

/// Equiv. to replaceUniform( p, var );
ShProgram operator<<(const ShVariable &var, const ShProgram &p); 

/// Currying operator
template<int N, typename T>
ShProgram operator<<(const ShProgram& a, const ShVariableN<N, T>& v) {
  ShProgram vNibble = SH_BEGIN_PROGRAM() {
    ShAttrib<N, SH_VAR_OUTPUT, T> out;
    out.node()->specialType(v.node()->specialType());
    out = v;
  } SH_END_PROGRAM;
  return connect(vNibble, a); 
}

/// Generic >> operator.  This allows any operation defined
// on programs p << x to also be defined when used as x >> p
template<typename T>
ShProgram operator>>(const ShProgram& p, const T& x) {
  return x << p;
}

/// Generic >> operator.  This allows any operation defined
// on programs x << p to also be used as p >> x
template<typename T>
ShProgram operator>>(const T& x, const ShProgram& p) { 
  return p << x; 
}

template<typename T>
ShProgram keep(std::string name = "") {
  ShProgram prog = SH_BEGIN_PROGRAM() {
    T temp;
    ShAttrib<T::typesize, SH_VAR_INPUT, typename T::ValueType> attr;
    attr.name( name );
    attr.node()->specialType(temp.node()->specialType());

    ShAttrib<T::typesize, SH_VAR_OUTPUT, typename T::ValueType> out;
    out.name( name );
    out.node()->specialType(temp.node()->specialType());
    out = attr;
  } SH_END_PROGRAM;
  return prog;
}

template<typename T>
ShProgram lose(std::string name = "") {
  ShProgram prog = SH_BEGIN_PROGRAM() {
    T temp;
    ShAttrib<T::typesize, SH_VAR_INPUT, typename T::ValueType> attr;
    attr.name( name );
    attr.node()->specialType(temp.node()->specialType());

    ShAttrib4f dummy = dummy;
  } SH_END_PROGRAM;
  return prog;
};

}

#endif
