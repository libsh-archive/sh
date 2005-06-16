// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHLIBPOINT_HPP
#define SHLIBPOINT_HPP

#include "ShAttrib.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_USUAL_OPERATIONS(SH_POINT)
SH_SHLIB_BINARY_RETTYPE_OPERATION(SH_POINT, operator-, SH_VECTOR, N)
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(SH_POINT, operator-, SH_VECTOR, 1)

SH_SHLIB_LEFT_MATRIX_OPERATION(SH_POINT, operator|, M)

// TODO: Special cases for homogeneous matrix multiplication etc.

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<3, SH_TEMP, T, SH_POINT, false>
operator|(const ShMatrix<4, 4, B1, T>& m, const ShAttrib<3, B2, T, SH_POINT, S1>& v)
{
  ShAttrib<4, SH_TEMP, T, SH_POINT, false> t;
  t(0,1,2) = v;
  t(3) = ShAttrib<1, SH_CONST, T>(1.0f);
  ShAttrib<4, SH_TEMP, T, SH_POINT, false> r = m | t;
  return r(0,1,2)/r(3);
}

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<2, SH_TEMP, T, SH_POINT, false>
operator|(const ShMatrix<3, 3, B1, T>& m, const ShAttrib<2, B2, T, SH_POINT, S1>& v)
{
  ShAttrib<3, SH_TEMP, T, SH_POINT, false> t;
  t(0,1) = v;
  t(2) = ShAttrib<1, SH_CONST, T>(1.0f);
  ShAttrib<3, SH_TEMP, T, SH_POINT, false> r = m | t;
  return r(0,1)/r(2);
}

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<1, SH_TEMP, T, SH_POINT, false>
operator|(const ShMatrix<2, 2, B1, T>& m, const ShAttrib<1, B2, T, SH_POINT, S1>& v)
{
  ShAttrib<2, SH_TEMP, T, SH_POINT, false> t;
  t(0) = v;
  t(1) = ShAttrib<1, SH_CONST, T>(1.0f);
  ShAttrib<2, SH_TEMP, T, SH_POINT, false> r = m | t;
  return r(0)/r(1);
}

}

#endif
