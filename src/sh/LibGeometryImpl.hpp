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
#ifndef SHLIBGEOMETRYIMPL_HPP
#define SHLIBGEOMETRYIMPL_HPP

#include "LibClamp.hpp"
#include "Attrib.hpp"
#include "Instructions.hpp"

namespace SH {

template<typename T1, typename T2>
Generic<3, CT1T2> cross(const Generic<3, T1>& left, const Generic<3, T2>& right)
{
  Attrib<3, SH_TEMP, CT1T2> t;
  shXPD(t, left, right);
  return t;
}

template<typename T1, typename T2>
inline
Generic<3, CT1T2> operator^(const Generic<3, T1>& left, const Generic<3, T2>& right)
{
  return cross(left, right);
}

template<int N, typename T>
Generic<N, T> normalize(const Generic<N, T>& var)
{
  Attrib<N, SH_TEMP, T> t;
  shNORM(t, var);
  return t;
}

template<int N1, int N2, typename T1, typename T2>
Generic<1, CT1T2> dot(const Generic<N1, T1>& left, const Generic<N2, T2>& right)
{
  Attrib<1, SH_TEMP, CT1T2> t;
  if (N1 == N2) {
    shDOT(t, left, right);
  } else {
    error(Exception("The two vectors are not of the same size."));
  }
  return t;
}

template<int N, typename T1, typename T2>
inline
Generic<1,  CT1T2> operator|(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  return dot(left, right);
}
SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

template<int N, typename T1, typename T2>
Generic<N, CT1T2> reflect(const Generic<N, T1>& a, const Generic<N, T2>& b)
{
  Generic<N, T2> bn = normalize(b);
  return 2 * dot(a, bn) * bn - a;
}

template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> refract(const Generic<N, T1>& v, const Generic<N, T2>& n,
                            const Generic<1, T3>& eta)
{
  Attrib1f tir;
  return refract_tir(v, n, eta, tir);
}

template<int N, typename T1, typename T2, typename T3, typename T4>
Generic<N, CT1T2T3> refract_tir(const Generic<N, T1>& v, const Generic<N, T2>& n,
                                const Generic<1, T3>& eta, Generic<1, T4>& tir)
{
  Generic<N, T1> vn = normalize(v);
  Generic<N, T2> nn = normalize(n);
  Generic<1, CT1T2T3> c = (vn|nn);

  Generic<1, T3> theta = rcp(eta);

  Generic<1, CT1T2T3> k = c*c - DataTypeConstant<CT1T2T3, HOST>::One;
  k = DataTypeConstant<CT1T2T3, HOST>::One + theta*theta*k;
  tir = k < 0.0;
  k = clamp(k, DataTypeConstant<CT1T2T3, HOST>::Zero, DataTypeConstant<CT1T2T3, HOST>::One); 
  Generic<1, CT1T2T3> a = -theta;
  Generic<1, CT1T2T3> b = theta*c - sqrt(k);

  return cond(tir, cast<N>(0.0), (a*vn + b*nn));
}

template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> faceforward(const Generic<N, T1>& a, const Generic<N, T2>& b)
{
  return (2 * (dot(a, b) > 0) - 1) * b;
}

template<typename T1, typename T2, typename T3>
Generic<4, CT1T2T3> lit(const Generic<1, T1>& a,
                          const Generic<1, T2>& b,
                          const Generic<1, T3>& c)
{
  Attrib<4, SH_TEMP, CT1T2T3> i;
  Attrib<4, SH_TEMP, CT1T2T3> r;
  shLIT(r, join(a, b, c, c));
  return r;
}

template<int N, typename T>
inline
Generic<1, T> distance(const Generic<N, T>& a, const Generic<N, T>& b)
{
  return length(a-b);
}

template<int N, typename T>
inline
Generic<1, T> distance_1(const Generic<N, T>& a, const Generic<N, T>& b)
{
  return length_1(a-b);
}

template<int N, typename T>
inline
Generic<1, T> distance_inf(const Generic<N, T>& a, const Generic<N, T>& b)
{
  return length_inf(a-b);
}

template<int N, typename T>
inline
Generic<1, T> length(const Generic<N, T>& a)
{
  return sqrt(dot(a, a));
}

template<int N, typename T>
inline
Generic<1, T> length_1(const Generic<N, T>& a)
{
  return sum(abs(a));
}

template<int N, typename T>
inline
Generic<1, T> length_inf(const Generic<N, T>& a)
{
  return max(abs(a));
}

}

#endif
