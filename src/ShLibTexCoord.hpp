#ifndef SHLIBTEXCOORD_HPP
#define SHLIBTEXCOORD_HPP

#include "ShTexCoord.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShTexCoord, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShTexCoord, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShTexCoord, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShTexCoord, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShTexCoord, abs, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, acos, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, asin, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, cos, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, frac, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, sin, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShTexCoord, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShTexCoord, operator|, M);

}

#endif
