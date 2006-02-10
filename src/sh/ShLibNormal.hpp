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
#ifndef SHLIBNORMAL_HPP
#define SHLIBNORMAL_HPP

#include "ShAttrib.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(SH_NORMAL, SH_NORMAL);

template<int N, ShBindingType B1, typename T, bool S1>
ShAttrib<N, SH_TEMP, T, SH_NORMAL, false>
abs(const ShAttrib<N, B1, T, SH_NORMAL, S1>& var)
{
  ShGeneric<N, T> t = abs(static_cast< ShGeneric<N, T> >(var));
  ShAttrib<N, SH_TEMP, T, SH_NORMAL, false> vec(t.node(), t.swizzle(), t.neg());
  return vec;
}

template<int N, ShBindingType B1, typename T, bool S1>
ShAttrib<N, SH_TEMP, T, SH_NORMAL, false>
normalize(const ShAttrib<N, B1, T, SH_NORMAL, S1>& var)
{
  ShGeneric<N, T> t = normalize(static_cast< ShGeneric<N, T> >(var));
  ShAttrib<N, SH_TEMP, T, SH_NORMAL, false> vec(t.node(), t.swizzle(), t.neg());
  return vec;
}

SH_SHLIB_USUAL_SUBTRACT(SH_NORMAL);

SH_SHLIB_LEFT_MATRIX_OPERATION(SH_NORMAL, operator|, M);

template<int N, ShBindingType B1, ShBindingType B2, typename T, bool S1, bool S2>
ShGeneric<1, T>
operator|(const ShAttrib<N, B1, T, SH_NORMAL, S1>& a, const ShAttrib<N, B2, T, SH_NORMAL, S2>& b)
{
  return dot(a, b);
}

template<int N, ShBindingType B1, ShBindingType B2, typename T, bool S1, bool S2>
ShGeneric<1, T> 
operator|(const ShAttrib<N, B1, T, SH_VECTOR, S1>& a, const ShAttrib<N, B2, T, SH_NORMAL, S2>& b)
{
  return dot(a, b);
}

template<int N, ShBindingType B1, ShBindingType B2, typename T, bool S1, bool S2>
ShGeneric<1, T>
operator|(const ShAttrib<N, B1, T, SH_NORMAL, S1>& a, const ShAttrib<N, B2, T, SH_VECTOR, S2>& b)
{
  return dot(a, b);
}

}

#endif
