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
#ifndef SHLIBGEOMETRYIMPL_HPP
#define SHLIBGEOMETRYIMPL_HPP

#include "ShLibClamp.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<typename T1, typename T2>
ShGeneric<3, CT1T2> cross(const ShGeneric<3, T1>& left, const ShGeneric<3, T2>& right)
{
  ShAttrib<3, SH_TEMP, CT1T2> t;
  shXPD(t, left, right);
  return t;
}

template<typename T1, typename T2>
inline
ShGeneric<3, CT1T2> operator^(const ShGeneric<3, T1>& left, const ShGeneric<3, T2>& right)
{
  return cross(left, right);
}

template<int N, typename T>
ShGeneric<N, T> normalize(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shNORM(t, var);
  return t;
}

template<int N1, int N2, typename T1, typename T2>
ShGeneric<1, CT1T2> dot(const ShGeneric<N1, T1>& left, const ShGeneric<N2, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  if (N1 == N2) {
    shDOT(t, left, right);
  } else {
    shError(ShException("The two vectors are not of the same size."));
  }
  return t;
}

template<int N, typename T1, typename T2>
inline
ShGeneric<1,  CT1T2> operator|(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  return dot(left, right);
}
SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> reflect(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  ShGeneric<N, T2> bn = normalize(b);
  return 2 * dot(a, b) * b - a;
}

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> refract(const ShGeneric<N, T1>& v, const ShGeneric<N, T2>& n,
                        const ShGeneric<1, T3>& theta)
{
  ShGeneric<N, T1> vn = normalize(v);
  ShGeneric<N, T2> nn = normalize(n);
  ShGeneric<1, CT1T2T3> c = (-vn|nn);
  ShGeneric<1, CT1T2T3> k = c*c - ShDataTypeConstant<CT1T2T3, SH_HOST>::One;
  k = ShDataTypeConstant<CT1T2T3, SH_HOST>::One + theta*theta*k;
  k = clamp(k, ShDataTypeConstant<CT1T2T3, SH_HOST>::Zero, ShDataTypeConstant<CT1T2T3, SH_HOST>::One); 
  ShGeneric<1, CT1T2T3> a = theta;
  ShGeneric<1, CT1T2T3> b = theta*c - sqrt(k);
  return (a*vn + b*nn);
}

template<int N, typename T1, typename T2>
inline
ShGeneric<N, CT1T2> faceforward(const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  return (2 * (dot(a, b) > 0) - 1) * b;
}

template<typename T1, typename T2, typename T3>
ShGeneric<4, CT1T2T3> lit(const ShGeneric<1, T1>& a,
                          const ShGeneric<1, T2>& b,
                          const ShGeneric<1, T3>& c)
{
  ShAttrib<4, SH_TEMP, CT1T2T3> i;
  ShAttrib<4, SH_TEMP, CT1T2T3> r;
  shLIT(r, join(a, b, c, c));
  return r;
}

template<int N, typename T>
inline
ShGeneric<1, T> distance(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length(a-b);
}

template<int N, typename T>
inline
ShGeneric<1, T> distance_1(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length_1(a-b);
}

template<int N, typename T>
inline
ShGeneric<1, T> distance_inf(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return length_inf(a-b);
}

template<int N, typename T>
inline
ShGeneric<1, T> length(const ShGeneric<N, T>& a)
{
  return sqrt(dot(a, a));
}

template<int N, typename T>
inline
ShGeneric<1, T> length_1(const ShGeneric<N, T>& a)
{
  return sum(abs(a));
}

template<int N, typename T>
inline
ShGeneric<1, T> length_inf(const ShGeneric<N, T>& a)
{
  return max(abs(a));
}

}

#endif
