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
#ifndef SHLIBCLAMPIMPL_HPP
#define SHLIBCLAMPIMPL_HPP

#include "LibClamp.hpp"
#include "Instructions.hpp"
#include "Attrib.hpp"
#include "LibMiscImpl.hpp"

namespace SH {

template<int N, typename T>
Generic<N, T> abs(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shABS(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> ceil(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shCEIL(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> floor(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shFLR(t, var);
  return t;
}

template<int N, typename T>
Generic<N, T> round(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shRND(t, var);
  return t;
}

template<int N, typename T1, typename T2>
Generic<N, CT1T2> mod(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> mod(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}
template<int N, typename T1, typename T2>
Generic<N, CT1T2> mod(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}
template<typename T1, typename T2>
Generic<1, CT1T2> mod(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  Attrib<1, TEMP, CT1T2> t;
  shMOD(t, left, right);
  return t;
}

template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> operator%(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  return mod(left, right);
}
template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> operator%(const Generic<N, T1>& left, const Generic<1, T2>& right)
{
  return mod(left, right);
}
template<int N, typename T1, typename T2>
inline
Generic<N, CT1T2> operator%(const Generic<1, T1>& left, const Generic<N, T2>& right)
{
  return mod(left, right);
}
template<typename T1, typename T2>
inline
Generic<1, CT1T2> operator%(const Generic<1, T1>& left, const Generic<1, T2>& right)
{
  return mod(left, right);
}
SHLIB_CONST_SCALAR_OP(mod);
SHLIB_CONST_N_OP_LEFT(mod);
SHLIB_CONST_SCALAR_OP(operator%);
SHLIB_CONST_N_OP_LEFT(operator%);

template<int N, typename T>
Generic<N, T> frac(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shFRAC(t, var);
  return t;
}

template<int N, typename T>
inline
Generic<N, T> pos(const Generic<N, T>& var)
{
  return max(var, fillcast<N>(0.0f));
}

template<int N, typename T1, typename T2>
Generic<N,  CT1T2> max(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMAX(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(max);

template<int N, typename T1, typename T2>
Generic<N,  CT1T2> min(const Generic<N, T1>& left, const Generic<N, T2>& right)
{
  Attrib<N, TEMP, CT1T2> t;
  shMIN(t, left, right);
  return t;
}
SHLIB_CONST_SCALAR_OP(min);

template<int N, typename T>
Generic<1, T> max(const Generic<N, T>& a)
{
  int lhswz[N/2 + N%2];
  for (int i = 0; i < N/2 + N%2; i++) {
    lhswz[i] = i;
  }
  int rhswz[N/2];
  for (int i = 0; i < N/2; i++) {
    rhswz[i] = i + N/2 + N%2;
  }

  return max(max(a.template swiz<N/2 + N%2>(lhswz)), max(a.template swiz<N/2>(rhswz)));
}

template<typename T>
inline
Generic<1, T> max(const Generic<1, T>& a)
{
  return a;
}

template<int N, typename T>
Generic<1, T> min(const Generic<N, T>& a)
{
  int lhswz[N/2 + N%2];
  for (int i = 0; i < N/2 + N%2; i++) {
    lhswz[i] = i;
  }
  int rhswz[N/2];
  for (int i = 0; i < N/2; i++) {
    rhswz[i] = i + N/2 + N%2;
  }

  return min(min(a.template swiz<N/2 + N%2>(lhswz)), min(a.template swiz<N/2>(rhswz)));
}

template<typename T>
inline
Generic<1, T> min(const Generic<1, T>& a)
{
  return a;
}

template<int N, typename T1, typename T2, typename T3>
inline
Generic<N, CT1T2T3> clamp(const Generic<N, T1>& a,
                      const Generic<N, T2>& b, const Generic<N, T3>& c)
{
  return min(max(a, b), c);
}
template<int N, typename T1, typename T2, typename T3>
inline
Generic<N, CT1T2T3> clamp(const Generic<N, T1>& a,
                      const Generic<1, T2>& b, const Generic<1, T3>& c)
{
  return min(max(a, fillcast<N>(b)), fillcast<N>(c));
}

template<typename T1, typename T2, typename T3>
inline
Generic<1, CT1T2T3> clamp(const Generic<1, T1>& a,
                      const Generic<1, T2>& b, const Generic<1, T3>& c)
{
  return min(max(a, b), c);
}
SHLIB_CONST_TRINARY_OP_011(clamp);

template<int N, typename T>
inline
Generic<N, T> sat(const Generic<N, T>& var)
{
  return min(var, fillcast<N>(ConstAttrib1f(1.0)));
}

template<int N, typename T>
Generic<N, T> sign(const Generic<N, T>& var)
{
  Attrib<N, TEMP, T> t;
  shSGN(t, var);
  return t;
}

template <int N, typename T>
inline
Generic<N, T>
linstep(const Generic<N, T>& t, const Generic<N, T>& c, const Generic<N, T>& w)
{
   return clamp((t - c)/w + 0.5f, 0.0f, 1.0f);
}

template<int N, typename T>
Generic<N, T> 
smoothstep(const Generic<N, T>& a, const Generic<N, T>& b, const Generic<N, T>& x)
{
  Generic<N, T> t = (x - a) / (b - a);
  // TODO fix this for other types
  t = clamp(t, 0.0f, 1.0f); 
  return t * t * mad(ConstAttrib1f(-2.0f), t, ConstAttrib1f(3.0f));
}

template <int N, typename T>
inline
Generic<N, T>
smoothpulse(const Generic<N, T>& t, const Generic<N, T>& r0, 
	     const Generic<N, T>& r1, const Generic<N, T>& w)
{
   return smoothstep(t,r0,w) - smoothstep(t,r1,w);
}

} // namespace

#endif
