#ifndef SHLIBCOLOR_HPP
#define SHLIBCOLOR_HPP

#include "ShColor.hpp"
#include "ShLib.hpp"

namespace SH {
SH_SHLIB_BINARY_OPERATION(ShColor, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShColor, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShColor, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShColor, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShColor, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShColor, abs, N);
SH_SHLIB_UNARY_OPERATION(ShColor, acos, N);
SH_SHLIB_UNARY_OPERATION(ShColor, asin, N);
SH_SHLIB_UNARY_OPERATION(ShColor, cos, N);
SH_SHLIB_UNARY_OPERATION(ShColor, frac, N);
SH_SHLIB_UNARY_OPERATION(ShColor, sin, N);
SH_SHLIB_UNARY_OPERATION(ShColor, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShColor, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShColor, operator|, M);

}

#endif
