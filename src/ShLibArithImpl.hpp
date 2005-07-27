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
#ifndef SHLIBARITHIMPL_HPP
#define SHLIBARITHIMPL_HPP

#include "ShLibArith.hpp"
#include "ShInstructions.hpp"

namespace SH {

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator+(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator+(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator+(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator+(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator+);
SH_SHLIB_CONST_N_OP_BOTH(operator+);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator-(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator-(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator-(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator-(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator-);
SH_SHLIB_CONST_N_OP_BOTH(operator-);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator*(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator*(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator*(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator*(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator/(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator/(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2>
operator/(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2>
operator/(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);

template<int N, typename T>
ShGeneric<N, T> exp(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> exp2(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP2(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> exp10(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shEXP10(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> expm1(const ShGeneric<N, T>& var)
{
  return exp(var - 1.0);
}

template<int N, typename T>
ShGeneric<N, T> log(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> log2(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG2(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> log10(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shLOG10(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> logp1(const ShGeneric<N, T>& var)
{
  return log(var + 1.0);
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> pow(const ShGeneric<N, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> pow(const ShGeneric<N, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> pow(const ShGeneric<1, T1>& left, const ShGeneric<N, T2>& right)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<typename T1, typename T2>
ShGeneric<1, CT1T2> pow(const ShGeneric<1, T1>& left, const ShGeneric<1, T2>& right)
{
  ShAttrib<1, SH_TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> mad(const ShGeneric<N, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> mad(const ShGeneric<N, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> mad(const ShGeneric<1, T1>& m1, const ShGeneric<N, T2>& m2, 
                    const ShGeneric<N, T3>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> mad(const ShGeneric<1, T1>& m1, const ShGeneric<1, T2>& m2, 
                    const ShGeneric<1, T3>& a)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}

template<int N, typename T1, typename T2> 
ShGeneric<N, CT1T2> 
mad(const CT1T2& m1, const ShGeneric<N, T1>& m2, const ShGeneric<N, T2>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMAD(t, ShAttrib<1, SH_CONST, CT1T2>(m1), m2, a);
  return t;
}

template<int N, typename T1, typename T2> 
ShGeneric<N, CT1T2>
mad(const ShGeneric<N, T1>& m1, const CT1T2& m2, const ShGeneric<N, T2>& a)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shMAD(t, m1, ShAttrib<1, SH_CONST, CT1T2>(m2), a);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> rcp(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shRCP(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> sqrt(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSQRT(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> rsqrt(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shRSQ(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> cbrt(const ShGeneric<N, T>& var)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCBRT(t, var);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> lerp(const ShGeneric<N, T1>& f, const ShGeneric<N, T2>& a, 
                           const ShGeneric<N, T3>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
ShGeneric<N, CT1T2T3> lerp(const ShGeneric<1, T1>& f, const ShGeneric<N, T2>& a, 
                           const ShGeneric<N, T3>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<typename T1, typename T2, typename T3>
ShGeneric<1, CT1T2T3> lerp(const ShGeneric<1, T1>& f, const ShGeneric<1, T2>& a, 
                           const ShGeneric<1, T3>& b)
{
  ShAttrib<1, SH_TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, typename T1, typename T2>
ShGeneric<N, CT1T2> lerp(const CT1T2& f, const ShGeneric<N, T1>& a, const ShGeneric<N, T2>& b)
{
  ShAttrib<N, SH_TEMP, CT1T2> t;
  shLRP(t, ShAttrib<1, SH_CONST, CT1T2>(f), a, b);
  return t;
}

template<int N, typename T>
ShGeneric<1, T> sum(const ShGeneric<N, T>& var)
{
  ShAttrib<1, SH_TEMP, T> t;
  shCSUM(t, var);
  return t;
}

template<int N, typename T>
ShGeneric<1, T> prod(const ShGeneric<N, T>& var)
{
  ShAttrib<1, SH_TEMP, T> t;
  shCMUL(t, var);
  return t;
}


}

#endif
