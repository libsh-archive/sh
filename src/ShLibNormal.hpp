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

template<int K1, int K2, typename T, bool S1>
ShNormal<3, SH_VAR_TEMP, T, false> operator|(const ShMatrix<4, 4, K1, T>& m,
                                             const ShNormal<3, K2, T, S1>& v)
{
  ShNormal<3, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 3; i++) {
    t(i) = dot(m[i](0,1,2), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShNormal<2, SH_VAR_TEMP, T, false> operator|(const ShMatrix<3, 3, K1, T>& m,
                                             const ShNormal<2, K2, T, S1>& v)
{
  ShNormal<2, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 2; i++) {
    t(i) = dot(m[i](0,1), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShNormal<1, SH_VAR_TEMP, T, false> operator|(const ShMatrix<2, 2, K1, T>& m,
                                             const ShNormal<1, K2, T, S1>& v)
{
  ShNormal<1, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 1; i++) {
    t(i) = dot(m[i](0), v);
  }
  return t;
}


}

#endif
