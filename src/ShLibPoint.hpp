#ifndef SHLIBPOINT_HPP
#define SHLIBPOINT_HPP

#include "ShPoint.hpp"
#include "ShVector.hpp"
#include "ShLib.hpp"

namespace SH {
SH_SHLIB_BINARY_OPERATION(ShPoint, operator+, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPoint, operator-, ShVector, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShPoint, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShPoint, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShPoint, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShPoint, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShPoint, abs, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, acos, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, asin, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, cos, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, frac, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, sin, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShPoint, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShPoint, operator|, M);

// TODO: Special cases for homogeneous matrix multiplication etc.

}

#endif
