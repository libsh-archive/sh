#ifndef SHLIBATTRIB_HPP
#define SHLIBATTRIB_HPP

#include "ShAttrib.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_BINARY_OPERATION(ShAttrib, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShAttrib, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShAttrib, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShAttrib, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShAttrib, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShAttrib, abs, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, acos, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, asin, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, cos, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, frac, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, sin, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShAttrib, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShAttrib, operator|, M);

}

#endif
