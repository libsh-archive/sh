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

template<int K1, int K2, typename T, bool S1>
ShVector<3, SH_VAR_TEMP, T, false> operator|(const ShMatrix<4, 4, K1, T>& m,
                                             const ShVector<3, K2, T, S1>& v)
{
  ShVector<3, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 3; i++) {
    t(i) = dot(m[i](0,1,2), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShVector<2, SH_VAR_TEMP, T, false> operator|(const ShMatrix<3, 3, K1, T>& m,
                                             const ShVector<2, K2, T, S1>& v)
{
  ShVector<2, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 2; i++) {
    t(i) = dot(m[i](0,1), v);
  }
  return t;
}

template<int K1, int K2, typename T, bool S1>
ShVector<1, SH_VAR_TEMP, T, false> operator|(const ShMatrix<2, 2, K1, T>& m,
                                             const ShVector<1, K2, T, S1>& v)
{
  ShVector<1, SH_VAR_TEMP, T, false> t;
  for (int i = 0; i < 1; i++) {
    t(i) = dot(m[i](0), v);
  }
  return t;
}

}

#endif
