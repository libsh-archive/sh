#ifndef SHALGEBRA_HPP
#define SHALGEBRA_HPP

#include "ShProgram.hpp"
#include "ShSyntax.hpp"
#include "ShAttrib.hpp"

namespace SH {

/// Replace inputs of b with outputs of a and connect them together.
ShProgram connect(const ShProgram& a, const ShProgram& b);

/// Run a and b together (i.e. use both inputs from a and b and both
/// outputs from a and b).
ShProgram combine(const ShProgram& a, const ShProgram& b);

/// Replaces uniform by appending an input to the end of the list
ShProgram replaceUniform(const ShProgram &a, const ShVariable &var); 

/// Equiv. to connect(a,b)
ShProgram operator<<(const ShProgram& a, const ShProgram& b);
/// Equiv. to combine(a,b)
ShProgram operator&(const ShProgram& a, const ShProgram& b);

template<typename T>
struct keep {
  typedef typename T::ValueType ValType;
  operator ShProgram() {
    ShProgram prog = SH_BEGIN_PROGRAM() {
      T temp;
      ShAttrib<T::typesize, SH_VAR_INPUT, ValType> attr;
      attr.node()->specialType(temp.node()->specialType());

      ShAttrib<T::typesize, SH_VAR_OUTPUT, ValType> out;
      out.node()->specialType(temp.node()->specialType());
      out = attr;
    } SH_END_PROGRAM;
    return prog;
  }
};

template<typename T>
struct lose {
  typedef typename T::ValueType ValType;
  operator ShProgram() {
    ShProgram prog = SH_BEGIN_PROGRAM() {
      T temp;
      ShAttrib<T::typesize, SH_VAR_INPUT, ValType> attr;
      attr.node()->specialType(temp.node()->specialType());

      ShAttrib4f dummy = dummy;
    } SH_END_PROGRAM;
    return prog;
  }
};

}

#endif
