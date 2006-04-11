// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
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
#ifndef SHLIBPOSITION_HPP
#define SHLIBPOSITION_HPP

#include "Attrib.hpp"
#include "Lib.hpp"

namespace SH {

SHLIB_USUAL_OPERATIONS_RETTYPE(POSITION, POINT);
SHLIB_BINARY_RETTYPE_OPERATION(POSITION, operator-, VECTOR, N);
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(POSITION, operator-, VECTOR, 1);

SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(POSITION, operator|, POINT, M);

// TODO: Special cases for homogeneous matrix multiplication etc.

template<BindingType B1, BindingType B2, typename T, bool S1>
Attrib<3, TEMP, T, POINT, false> 
operator|(const Matrix<4, 4, B1, T>& m, const Attrib<3, B2, T, POSITION, S1>& v)
{
  Attrib<4, TEMP, T, POINT, false> t;
  t(0,1,2) = v;
  t(3) = Attrib<1, CONST, T>(1.0f);
  Attrib<4, TEMP, T, POINT, false> r = m | t;
  return r(0,1,2)/r(3);
}

template<BindingType B1, BindingType B2, typename T, bool S1>
Attrib<2, TEMP, T, POINT, false>
operator|(const Matrix<3, 3, B1, T>& m, const Attrib<2, B2, T, POSITION, S1>& v)
{
  Attrib<3, TEMP, T, POINT, false> t;
  t(0,1) = v;
  t(2) = Attrib<1, CONST, T>(1.0f);
  Attrib<3, TEMP, T, POINT, false> r = m | t;
  return r(0,1)/r(2);
}

template<BindingType B1, BindingType B2, typename T, bool S1>
Attrib<1, TEMP, T, POSITION, false>
operator|(const Matrix<2, 2, B1, T>& m, const Attrib<1, B2, T, POSITION, S1>& v)
{
  Attrib<2, TEMP, T, POINT, false> t;
  t(0) = v;
  t(1) = Attrib<1, CONST, T>(1.0f);
  Attrib<2, TEMP, T, POINT, false> r = m | t;
  return r(0)/r(1);
}

}


#endif
