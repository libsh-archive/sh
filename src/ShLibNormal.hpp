#ifndef SHLIBNORMAL_HPP
#define SHLIBNORMAL_HPP

#include "ShNormal.hpp"
#include "ShVector.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_BINARY_OPERATION(ShNormal, operator+, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator-, N);
SH_SHLIB_UNEQ_BINARY_OPERATION(ShNormal, operator*, N);

SH_SHLIB_LEFT_SCALAR_OPERATION(ShNormal, operator*);

SH_SHLIB_UNEQ_BINARY_OPERATION(ShNormal, operator/, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator^, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator<, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator<=, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator>, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator>=, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator==, N);
SH_SHLIB_BINARY_OPERATION(ShNormal, operator!=, N);

SH_SHLIB_UNARY_OPERATION(ShNormal, abs, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, acos, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, asin, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, cos, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, frac, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, sin, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, sqrt, N);
SH_SHLIB_UNARY_OPERATION(ShNormal, normalize, N);

SH_SHLIB_LEFT_MATRIX_OPERATION(ShNormal, operator|, M);

template<int N, int K1, int K2, typename T, bool S1, bool S2>
ShVariableN<1, T> operator|(const ShNormal<N, K1, T, S1>& a,
                            const ShNormal<N, K2, T, S2>& b)
{
  return dot(a, b);
}

template<int N, int K1, int K2, typename T, bool S1, bool S2>
ShVariableN<1, T> operator|(const ShVector<N, K1, T, S1>& a,
                            const ShNormal<N, K2, T, S2>& b)
{
  return dot(a, b);
}

template<int N, int K1, int K2, typename T, bool S1, bool S2>
ShVariableN<1, T> operator|(const ShNormal<N, K1, T, S1>& a,
                            const ShVector<N, K2, T, S2>& b)
{
  return dot(a, b);
}

}

#endif
