#ifndef SHLIBVECTOR_HPP
#define SHLIBVECTOR_HPP

#include "ShVector.hpp"
#include "ShLib.hpp"

namespace SH {
SH_SHLIB_BINARY_OPERATION(ShVector, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShVector, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShVector, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShVector, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShVector, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShVector, abs, N);
SH_SHLIB_UNARY_OPERATION(ShVector, acos, N);
SH_SHLIB_UNARY_OPERATION(ShVector, asin, N);
SH_SHLIB_UNARY_OPERATION(ShVector, cos, N);
SH_SHLIB_UNARY_OPERATION(ShVector, frac, N);
SH_SHLIB_UNARY_OPERATION(ShVector, sin, N);
SH_SHLIB_UNARY_OPERATION(ShVector, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShVector, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShVector, operator|, M);

template<int N, int K1, int K2, typename T, bool S1, bool S2>
ShVariableN<1, T> operator|(const ShVector<N, K1, T, S1>& a,
                            const ShVector<N, K2, T, S2>& b)
{
  return dot(a, b);
}

// TODO: Special cases for homogeneous matrix multiplication etc.

}

#endif
