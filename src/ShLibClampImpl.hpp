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
#ifndef SHLIBCLAMPIMPL_HPP
#define SHLIBCLAMPIMPL_HPP

#include "ShLibClamp.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"
#include "ShLibMiscImpl.hpp"

namespace SH {

template<int N, ShValueType V>
inline
ShGeneric<N, V> abs(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shABS(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> ceil(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shCEIL(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> floor(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shFLR(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> round(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shRND(t, var);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> mod(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMOD(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> mod(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMOD(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2> mod(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shMOD(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> operator%(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  return mod(left, right);
}
template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N, CV1V2> operator%(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  return mod(left, right);
}
template<ShValueType V1, ShValueType V2>
inline
ShGeneric<1, CV1V2> operator%(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  return mod(left, right);
}
SH_SHLIB_CONST_SCALAR_OP(mod);
SH_SHLIB_CONST_N_OP_LEFT(mod);
SH_SHLIB_CONST_SCALAR_OP(operator%);
SH_SHLIB_CONST_N_OP_LEFT(operator%);

template<int N, ShValueType V>
inline
ShGeneric<N, V> frac(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shFRAC(t, var);
  return t;
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> pos(const ShGeneric<N, V>& var)
{
  return max(var, fillcast<N>(0.0f));
}

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N,  CV1V2> max(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMAX(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(max);

template<int N, ShValueType V1, ShValueType V2>
inline
ShGeneric<N,  CV1V2> min(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shMIN(t, left, right);
  return t;
}
SH_SHLIB_CONST_SCALAR_OP(min);

template<int N, ShValueType V>
ShGeneric<1, V> max(const ShGeneric<N, V>& a)
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

template<ShValueType V>
ShGeneric<1, V> max(const ShGeneric<1, V>& a)
{
  return a;
}

template<int N, ShValueType V>
ShGeneric<1, V> min(const ShGeneric<N, V>& a)
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

template<ShValueType V>
ShGeneric<1, V> min(const ShGeneric<1, V>& a)
{
  return a;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> clamp(const ShGeneric<N, V1>& a,
                      const ShGeneric<N, V2>& b, const ShGeneric<N, V3>& c)
{
  return min(max(a, b), c);
}
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<N, CV1V2V3> clamp(const ShGeneric<N, V1>& a,
                      const ShGeneric<1, V2>& b, const ShGeneric<1, V3>& c)
{
  return min(max(a, fillcast<N>(b)), fillcast<N>(c));
}

template<ShValueType V1, ShValueType V2, ShValueType V3>
inline
ShGeneric<1, CV1V2V3> clamp(const ShGeneric<1, V1>& a,
                      const ShGeneric<1, V2>& b, const ShGeneric<1, V3>& c)
{
  return min(max(a, b), c);
}
SH_SHLIB_CONST_TRINARY_OP_011(clamp);

template<int N, ShValueType V>
inline
ShGeneric<N, V> sat(const ShGeneric<N, V>& var)
{
  return min(var, fillcast<N>(ShConstAttrib1f(1.0)));
}

template<int N, ShValueType V>
inline
ShGeneric<N, V> sign(const ShGeneric<N, V>& var)
{
  ShAttrib<N, SH_TEMP, V> t;
  shSGN(t, var);
  return t;
}

}

#endif
