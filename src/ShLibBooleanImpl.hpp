// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Viberiu S. Popa,
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
#ifndef SHLIBBOOLEANIMPL_HPP
#define SHLIBBOOLEANIMPL_HPP

#include "ShLibBoolean.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"

namespace SH {

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLT(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLT(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator<(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSLT(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator<=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSLE(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator<=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSLE(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGT(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGT(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator>(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSGT(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator>=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSGE(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator>=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSGE(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator==(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator==(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator==(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSEQ(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator==(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSEQ(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator!=(const ShGeneric<N, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator!=(const ShGeneric<N, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSNE(t, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator!=(const ShGeneric<1, V1>& left, const ShGeneric<N, V2>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2> t;
  shSNE(t, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2>
ShGeneric<1, CV1V2> operator!=(const ShGeneric<1, V1>& left, const ShGeneric<1, V2>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2> t;
  shSNE(t, left, right);
  return t;
}

template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> cond(const ShGeneric<N, V1>& condition, const ShGeneric<N, V2>& left,
                     const ShGeneric<N, V3>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<N, CV1V2V3> cond(const ShGeneric<1, V1>& condition, const ShGeneric<N, V2>& left,
                     const ShGeneric<N, V3>& right)
{
  ShAttrib<N, SH_TEMP, CV1V2V3> t;
  shCOND(t, condition, left, right);
  return t;
}
template<ShValueType V1, ShValueType V2, ShValueType V3>
ShGeneric<1, CV1V2V3> cond(const ShGeneric<1, V1>& condition, const ShGeneric<1, V2>& left,
                     const ShGeneric<1, V3>& right)
{
  ShAttrib<1, SH_TEMP, CV1V2V3> t;
  shCOND(t, condition, left, right);
  return t;
}

// TODO

template<int N, ShValueType V>
ShGeneric<N, V> operator!(const ShGeneric<N, V>& a)
{
  return 1.0f - (a > 0.0f);
}


template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator&&(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b)
{
  return min(a,b);
}

template<int N, ShValueType V1, ShValueType V2>
ShGeneric<N, CV1V2> operator||(const ShGeneric<N, V1>& a, const ShGeneric<N, V2>& b)
{
  return max(a,b);
}

template<int N, ShValueType V>
ShGeneric<1, V> any(const ShGeneric<N, V>& a)
{
  ShAttrib<1, SH_TEMP, V> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t || a(i);
  }
  return t;
}

template<int N, ShValueType V>
ShGeneric<1, V> all(const ShGeneric<N, V>& a)
{
  ShAttrib<1, SH_TEMP, V> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t && a(i);
  }
  return t;
}

}

#endif
