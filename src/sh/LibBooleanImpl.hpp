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

#include "LibBoolean.hpp"
#include "Instructions.hpp"
#include "Attrib.hpp"

namespace SH {

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator<(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSLT(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator<);
SHLIB_CONST_N_OP_BOTH(operator<);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<=(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<=(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator<=(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator<=(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSLE(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator<=);
SHLIB_CONST_N_OP_BOTH(operator<=);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator>(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSGT(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator>);
SHLIB_CONST_N_OP_BOTH(operator>);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>=(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>=(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator>=(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator>=(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSGE(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator>=);
SHLIB_CONST_N_OP_BOTH(operator>=);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator==(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator==(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator==(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator==(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSEQ(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator==);
SHLIB_CONST_N_OP_BOTH(operator==);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator!=(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator!=(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator!=(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> operator!=(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  shSNE(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator!=);
SHLIB_CONST_N_OP_BOTH(operator!=);

template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> cond(const Generic<N, T1>& condition, 
                           const Generic<N, T2>& left,
                           const Generic<N, T3>& right)
{
  Attrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> cond(const Generic<1, T1>& condition,
                           const Generic<N, T2>& left,
                           const Generic<N, T3>& right)
{
  Attrib<N, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3> cond(const Generic<1, T1>& condition, 
                           const Generic<1, T2>& left,
                           const Generic<1, T3>& right)
{
  Attrib<1, SH_TEMP, CT1T2T3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> cond(const CT1T2& condition, 
                         const Generic<N, T1>& left, 
                         const Generic<N, T2>& right)
{
  Attrib<N, SH_TEMP, CT1T2> t;
  shCOND(t, Attrib<1, SH_CONST, CT1T2>(condition), left, right);
  return t;
}

template<int N, typename T>
Generic<N, T> operator!(const Generic<N, T>& a)
{
  return static_cast<T>(1) - (a > static_cast<T>(0));
}


template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator&&(const Generic<N, T1>& a, const Generic<N, T2>& b)
{
  return min(a,b);
}

template<int N, typename T1, typename T2>
Generic<N, CT1T2> operator||(const Generic<N, T1>& a, const Generic<N, T2>& b)
{
  return max(a,b);
}

template<int N, typename T>
Generic<1, T> any(const Generic<N, T>& a)
{
  Attrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t || a(i);
  }
  return t;
}

template<int N, typename T>
Generic<1, T> all(const Generic<N, T>& a)
{
  Attrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t && a(i);
  }
  return t;
}

}

#endif
