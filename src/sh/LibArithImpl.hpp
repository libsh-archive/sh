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
#ifndef SHLIBARITHIMPL_HPP
#define SHLIBARITHIMPL_HPP

#include "LibArith.hpp"
#include "Instructions.hpp"

namespace SH {

template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator+(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator+(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator+(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2>
operator+(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shADD(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator+);
SHLIB_CONST_N_OP_BOTH(operator+);

template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator-(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator-(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator-(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2>
operator-(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shADD(t, left, -right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator-);
SHLIB_CONST_N_OP_BOTH(operator-);

template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator*(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator*(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator*(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2>
operator*(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shMUL(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator*);
SHLIB_CONST_N_OP_BOTH(operator*);

template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator/(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator/(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2>
operator/(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2>
operator/(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shDIV(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(operator/);
SHLIB_CONST_N_OP_LEFT(operator/);

template<int N, typename T>
Generic<N, T> exp(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shEXP(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> exp2(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shEXP2(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> exp10(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shEXP10(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> expm1(const Generic<N, T>& var)
{
  return exp(var - 1.0);
}

template<int N, typename T>
Generic<N, T> log(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shLOG(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> log2(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shLOG2(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> log10(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shLOG10(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> logp1(const Generic<N, T>& var)
{
  return log(var + 1.0);
}

template<int N, typename T1, typename T2>
Generic<N, CT1T2> pow(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> pow(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> pow(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> pow(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shPOW(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(pow);
SHLIB_CONST_N_OP_RIGHT(pow);

template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<N, T1>& m1, const Generic<N, T2>& m2, 
                    const Generic<N, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<N, T1>& m1, const Generic<N, T2>& m2, 
                    const Generic<1, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<N, T1>& m1, const Generic<1, T2>& m2, 
                    const Generic<N, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<N, T1>& m1, const Generic<1, T2>& m2, 
                    const Generic<1, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<1, T1>& m1, const Generic<N, T2>& m2, 
                    const Generic<N, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<1, T1>& m1, const Generic<N, T2>& m2, 
                    const Generic<1, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> mad(const Generic<1, T1>& m1, const Generic<1, T2>& m2, 
                    const Generic<N, T3>& a)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}
template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3> mad(const Generic<1, T1>& m1, const Generic<1, T2>& m2, 
                    const Generic<1, T3>& a)
{
  Attrib<1, TEMP, CT1T2T3> t;
  shMAD(t, m1, m2, a);
  return t;
}

template<int N, typename T1, typename T2> 
Generic<N, CT1T2> 
mad(const CT1T2& m1, const Generic<N, T1>& m2, const Generic<N, T2>& a)
{
  Attrib<N, TEMP, CT1T2> t;
  shMAD(t, Attrib<1, CONST, CT1T2>(m1), m2, a);
  return t;
}
template<int N, typename T1, typename T2> 
Generic<N, CT1T2>
mad(const Generic<N, T1>& m1, const CT1T2& m2, const Generic<N, T2>& a)
{
  Attrib<N, TEMP, CT1T2> t;
  shMAD(t, m1, Attrib<1, CONST, CT1T2>(m2), a);
  return t;
}

template<int N, typename T>
Generic<N, T> rcp(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shRCP(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> sqrt(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shSQRT(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> rsqrt(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shRSQ(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> cbrt(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shCBRT(t, var);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> lerp(const Generic<N, T1>& f, const Generic<N, T2>& a, 
                           const Generic<N, T3>& b)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, typename T1, typename T2, typename T3>
Generic<N, CT1T2T3> lerp(const Generic<1, T1>& f, const Generic<N, T2>& a, 
                           const Generic<N, T3>& b)
{
  Attrib<N, TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<typename T1, typename T2, typename T3>
Generic<1, CT1T2T3> lerp(const Generic<1, T1>& f, const Generic<1, T2>& a, 
                           const Generic<1, T3>& b)
{
  Attrib<1, TEMP, CT1T2T3> t;
  shLRP(t, f, a, b);
  return t;
}

template<int N, typename T1, typename T2>
Generic<N, CT1T2> lerp(const CT1T2& f, const Generic<N, T1>& a, const Generic<N, T2>& b)
{
  Attrib<N, TEMP, CT1T2> t;
  shLRP(t, Attrib<1, CONST, CT1T2>(f), a, b);
  return t;
}

template<int N, typename T>
Generic<1, T> sum(const Generic<N, T>& var)
{
  Attrib<1, TEMP, T> t;
  shCSUM(t, var);
  return t;
}

template<int N, typename T>
Generic<1, T> prod(const Generic<N, T>& var)
{
  Attrib<1, TEMP, T> t;
  shCMUL(t, var);
  return t;
}


}

#endif
