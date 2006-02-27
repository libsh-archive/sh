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
#ifndef SHLIBBOOLEANIMPL_HPP
#define SHLIBBOOLEANIMPL_HPP

#include "ShLibBoolean.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"

namespace SH {

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator<(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator<);
SH_SHLIB_CONST_N_OP_BOTH(operator<);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator<=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator<=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator<=);
SH_SHLIB_CONST_N_OP_BOTH(operator<=);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator>(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator>);
SH_SHLIB_CONST_N_OP_BOTH(operator>);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator>=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator>=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator>=);
SH_SHLIB_CONST_N_OP_BOTH(operator>=);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator==(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator==(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator==);
SH_SHLIB_CONST_N_OP_BOTH(operator==);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator!=(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> operator!=(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator!=);
SH_SHLIB_CONST_N_OP_BOTH(operator!=);

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> cond(const ShGeneric<N, T1>& condition, 
                           const ShGeneric<N, T2>& left,
                           const ShGeneric<N, T3>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> cond(const ShGeneric<1, T1>& condition,
                           const ShGeneric<N, T2>& left,
                           const ShGeneric<N, T3>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> cond(const ShGeneric<1, T1>& condition, 
                           const ShGeneric<1, T2>& left,
                           const ShGeneric<1, T3>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> cond(const CT1T2& condition, 
                         const ShGeneric<N, T1>& left, 
                         const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shCOND(t, ShAttrib<1, SH_CONST, CT1T2>(condition), left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator!(const ShGeneric<N, T>& a)
{
  return static_cast<T>(1) - (a > static_cast<T>(0));
}


template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator&&(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  return min(a,b);
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> operator||(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  return max(a,b);
}

template<int N, typename T>
ShGeneric<1, T> any(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t || a(i);
  }
  return t;
}

template<int N, typename T>
ShGeneric<1, T> all(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t && a(i);
  }
  return t;
}

}

#endif
