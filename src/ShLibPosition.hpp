// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLIBPOSITION_HPP
#define SHLIBPOSITION_HPP

#include "ShPosition.hpp"
#include "ShVector.hpp"
#include "ShPoint.hpp"
#include "ShConstant.hpp"
#include "ShLib.hpp"

namespace SH {
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator+, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator-, ShVector, N);
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(ShPosition, operator*, ShPoint, N);

SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(ShPosition, operator*, ShPoint);

SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(ShPosition, operator/, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator^, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator<, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator<=, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator>, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator>=, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator==, ShPoint, N);
SH_SHLIB_BINARY_RETTYPE_OPERATION(ShPosition, operator!=, ShPoint, N);

SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, abs, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, acos, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, asin, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, cos, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, frac, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, sin, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, sqrt, ShPoint, N);
SH_SHLIB_UNARY_RETTYPE_OPERATION(ShPosition, normalize, ShPoint, N);

SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(ShPosition, operator|, ShPoint, M);

// TODO: Special cases for homogeneous matrix multiplication etc.

template<int K1, int K2, typename T, bool S1>
ShPoint<3, SH_VAR_TEMP, T, false> operator|(const ShMatrix<4, 4, K1, T>& m,
                                            const ShPosition<3, K2, T, S1>& v)
{
  ShPoint<4, SH_VAR_TEMP, T, false> t;
  t(0,1,2) = v;
  t(3) = ShConstant1f(1.0);
  ShPoint<4, SH_VAR_TEMP, T, false> r = m | t;
  return r(0,1,2)/r(3);
}

template<int K1, int K2, typename T, bool S1>
ShPoint<2, SH_VAR_TEMP, T, false> operator|(const ShMatrix<3, 3, K1, T>& m,
                                            const ShPosition<2, K2, T, S1>& v)
{
  ShPoint<3, SH_VAR_TEMP, T, false> t;
  t(0,1) = v;
  t(2) = ShConstant1f(1.0);
  ShPoint<3, SH_VAR_TEMP, T, false> r = m | t;
  return r(0,1)/r(2);
}

template<int K1, int K2, typename T, bool S1>
ShPosition<1, SH_VAR_TEMP, T, false> operator|(const ShMatrix<2, 2, K1, T>& m,
                                            const ShPosition<1, K2, T, S1>& v)
{
  ShPoint<2, SH_VAR_TEMP, T, false> t;
  t(0) = v;
  t(1) = ShConstant1f(1.0);
  ShPoint<2, SH_VAR_TEMP, T, false> r = m | t;
  return r(0)/r(1);
}

}


#endif
