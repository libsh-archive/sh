#ifndef SHALGEBRA_HPP
#define SHALGEBRA_HPP

#include "ShProgram.hpp"

namespace SH {

/// Replace inputs of b with outputs of a and connect them together.
ShProgram connect(const ShProgram& a, const ShProgram& b);

/// Run a and b together (i.e. use both inputs from a and b and both
/// outputs from a and b).
ShProgram combine(const ShProgram& a, const ShProgram& b);

}

#endif
