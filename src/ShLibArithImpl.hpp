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
#ifndef SHLIBARITHIMPL_HPP
#define SHLIBARITHIMPL_HPP

#include "ShLibArith.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator+(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator+(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator+(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2>
operator+(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shADD(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator-(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator-(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator-(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shADD(t, left, -right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2>
operator-(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shADD(t, left, -right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator*(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator*(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator*(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMUL(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2>
operator*(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shMUL(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator/(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator/(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2>
operator/(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shDIV(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2>
operator/(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shDIV(t, left, right);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> exp(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shEXP(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> exp2(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shEXP2(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> exp10(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shEXP10(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> expm1(const ShGeneric<N, V>& var)
{
  return exp(var - 1.0);
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> log(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shLOG(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> log2(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shLOG2(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> log10(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shLOG10(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> logp1(const ShGeneric<N, V>& var)
{
  return log(var + 1.0);
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> pow(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> pow(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shPOW(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2> pow(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shPOW(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> mad(const ShGeneric<N, V1>& m1, const ShGeneric<N, V2>& m2, 
                    const ShGeneric<N, V3>& a)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> mad(const ShGeneric<N, V1>& m1, const ShGeneric<1, V2>& m2, 
                    const ShGeneric<N, V3>& a)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> mad(const ShGeneric<1, V1>& m1, const ShGeneric<N, V2>& m2, 
                    const ShGeneric<N, V3>& a)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<1, CV1V2V3> mad(const ShGeneric<1, V1>& m1, const ShGeneric<1, V2>& m2, 
                    const ShGeneric<1, V3>& a)
{
  ShAttrib<1, SH_TEMP, CV1V2V3> t;
  shMAD(t, m1, m2, a);
  return t;
}

//template<int N, ShValueType V> 
//inline
//ShGeneric<N, V> mad(T m1, const ShGeneric<N, V>& m2, const ShGeneric<N, V>& a)
//{
//  ShAttrib<N, SH_TEMP, V> t;
//  shMAD(t, ShAttrib<1, SH_CONST, V>(m1), m2, a);
//  return t;
//}
//template<int N, ShValueType V>
//inline
//ShGeneric<N, V> mad(const ShGeneric<N, V>& m1, V m2, const ShGeneric<N, V>& a)
//{
//  ShAttrib<N, SH_TEMP, V> t;
//  shMAD(t, m1, ShAttrib<1, SH_CONST, V>(m2), a);
//  return t;
//}

template<int N, ShValueType V1, ShValueType V2> 
inline
ShGeneric<N, CV1V2> mad(double m1, const ShGeneric<N, V1>& m2, const ShGeneric<N, V2>& a)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMAD(t, ShAttrib<1, SH_CONST, CV1V2>(m1), m2, a);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> mad(const ShGeneric<N, V1>& m1, double m2, const ShGeneric<N, V2>& a)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMAD(t, m1, ShAttrib<1, SH_CONST, CV1V2>(m2), a);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> rcp(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shRCP(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> sqrt(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shSQRT(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> rsqrt(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shRSQ(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> cbrt(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shCBRT(t, var);
  return t;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> lerp(const ShGeneric<N, V1>& f, const ShGeneric<N, V2>& a, 
                     const ShGeneric<N, V3>& b)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> lerp(const ShGeneric<1, V1>& f, const ShGeneric<N, V2>& a, 
                     const ShGeneric<N, V3>& b)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shLRP(t, f, a, b);
  return t;
}

template<ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<1, CV1V2V3> lerp(const ShGeneric<1, V1>& f, const ShGeneric<1, V2>& a, 
                     const ShGeneric<1, V3>& b)
{
  ShAttrib<1, SH_TEMP, CV1V2V3> t;
  shLRP(t, f, a, b);
  return t;
}
//@todo type see explanation in LibArith.hpp file
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> lerp(double f, const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<1, V> sum(const ShGeneric<N, V>& var)
{
  ShAttrib<1, SH_TEMP, V> t;
  shCSUM(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<1, V> prod(const ShGeneric<N, V>& var)
{
  ShAttrib<1, SH_TEMP, V> t;
  shCMUL(t, var);
  return t;
}


}

#endif
