// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
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
