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
#ifndef SHLIBGEOMETRYIMPL_HPP
#define SHLIBGEOMETRYIMPL_HPP

#include "ShLibClamp.hpp"
#include "ShAttrib.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<ShValueType V1, ShValueType V2>
inline
ShGeneric<3, CV1V2> cross(const ShGeneric<3, V1>& left, const ShGeneric<3, V2>& right)
{
  ShAttrib<3, SH_TEMP, CV1V2> t;
  shXPD(t, left, right);
  return t;
}

template<ShValueType V1, ShValueType V2>
inline
ShGeneric<3, CV1V2> operator^(const ShGeneric<3, V1>& left, const ShGeneric<3, V2>& right)
{
  return cross(left, right);
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> normalize(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shNORM(t, var);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2> dot(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shDOT(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<1,  CV1V2> operator|(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  return dot(left, right);
}
SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> reflect(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b)
{
  ShGeneric<N, V2> bn = normalize(b);
  return 2 * dot(a, b) * b - a;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> refract(const ShGeneric<N, V1>& v, const ShGeneric<N, V2>& n,
                        const ShGeneric<1, V3>& theta)
{
  ShGeneric<N, V1> vn = normalize(v);
  ShGeneric<N, V2> nn = normalize(n);
  ShGeneric<1, CV1V2V3> c = (vn|nn);
  ShGeneric<1, CV1V2V3> k = c*c - ShDataTypeConstant<CV1V2V3, SH_HOST>::One;
  k = ShDataTypeConstant<CV1V2V3, SH_HOST>::One + theta*theta*k;
  k = clamp(k, ShDataTypeConstant<CV1V2V3, SH_HOST>::Zero, ShDataTypeConstant<CV1V2V3, SH_HOST>::One); 
  ShGeneric<1, CV1V2V3> a = theta;
  ShGeneric<1, CV1V2V3> b = theta*c + sqrt(k);
  return (a*vn + b*nn);
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> faceforward(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b)
{
  return (2 * (dot(a, b) > 0) - 1) * b;
}

template<ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<4, CV1V2V3> lit(const ShGeneric<1, V1>& a,
                          const ShGeneric<1, V2>& b,
                          const ShGeneric<1, V3>& c)
{
  ShAttrib<4, SH_TEMP, CV1V2V3> r;
  r(0,3) = ShAttrib<2, SH_CONST, CV1V2V3>(1, 1);
  r(1) = pos(a);
  r(2) = (a < 0 && b < 0) * pow(b, c);
  return r;
}

template<int N, ShValueType V>
ShGeneric<1, V> distance(const ShGeneric<N, V>& a, const ShGeneric<N, V>& b)
{
  return length(a-b);
}

template<int N, ShValueType V>
ShGeneric<1, V> distance_1(const ShGeneric<N, V>& a, const ShGeneric<N, V>& b)
{
  return length_1(a-b);
}

template<int N, ShValueType V>
ShGeneric<1, V> distance_inf(const ShGeneric<N, V>& a, const ShGeneric<N, V>& b)
{
  return length_inf(a-b);
}

template<int N, ShValueType V>
ShGeneric<1, V> length(const ShGeneric<N, V>& a)
{
  return sqrt(dot(a, a));
}

template<int N, ShValueType V>
ShGeneric<1, V> length_1(const ShGeneric<N, V>& a, const ShGeneric<N, V>& b)
{
  return sum(abs(a));
}

template<int N, ShValueType V>
ShGeneric<1, V> length_inf(const ShGeneric<N, V>& a, const ShGeneric<N, V>& b)
{
  return max(abs(a));
}


}

#endif
