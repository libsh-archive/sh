#ifndef SHLIBPOINT_HPP
#define SHLIBPOINT_HPP

#include "ShPoint.hpp"
#include "ShVector.hpp"
#include "ShConstant.hpp"
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

template<int K1, int K2, typename T, bool S1>
ShPoint<3, SH_VAR_TEMP, T, false> operator|(const ShMatrix<4, 4, K1, T>& m,
                                            const ShPoint<3, K2, T, S1>& v)
{
  ShPoint<4, SH_VAR_TEMP, T, false> t;
  t(0,1,2) = v;
  t(3) = ShConstant1f(1.0);
  ShPoint<4, SH_VAR_TEMP, T, false> r = m | t;
  return r(0,1,2)/r(3);
}

template<int K1, int K2, typename T, bool S1>
ShPoint<2, SH_VAR_TEMP, T, false> operator|(const ShMatrix<3, 3, K1, T>& m,
                                            const ShPoint<2, K2, T, S1>& v)
{
  ShPoint<3, SH_VAR_TEMP, T, false> t;
  t(0,1) = v;
  t(2) = ShConstant1f(1.0);
  ShPoint<3, SH_VAR_TEMP, T, false> r = m | t;
  return r(0,1)/r(2);
}

template<int K1, int K2, typename T, bool S1>
ShPoint<1, SH_VAR_TEMP, T, false> operator|(const ShMatrix<2, 2, K1, T>& m,
                                            const ShPoint<1, K2, T, S1>& v)
{
  ShPoint<2, SH_VAR_TEMP, T, false> t;
  t(0) = v;
  t(1) = ShConstant1f(1.0);
  ShPoint<2, SH_VAR_TEMP, T, false> r = m | t;
  return r(0)/r(1);
}

}

#endif
