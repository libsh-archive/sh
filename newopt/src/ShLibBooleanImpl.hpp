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
#ifndef SHLIBBOOLEANIMPL_HPP
#define SHLIBBOOLEANIMPL_HPP

#include "ShLibBoolean.hpp"
#include "ShInstructions.hpp"
#include "ShAttrib.hpp"

namespace SH {

template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator<(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSLT(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator<=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator<=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSLE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator>(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSGT(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator>=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator>=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSGE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator==(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator==(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSEQ(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<N, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<N, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> operator!=(const ShGeneric<1, T>& left, const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> operator!=(const ShGeneric<1, T>& left, const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shSNE(t, left, right);
  return t;
}

template<int N, typename T>
ShGeneric<N, T> cond(const ShGeneric<N, T>& condition, const ShGeneric<N, T>& left,
                     const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}
template<int N, typename T>
ShGeneric<N, T> cond(const ShGeneric<1, T>& condition, const ShGeneric<N, T>& left,
                     const ShGeneric<N, T>& right)
{
  ShAttrib<N, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}
template<typename T>
ShGeneric<1, T> cond(const ShGeneric<1, T>& condition, const ShGeneric<1, T>& left,
                     const ShGeneric<1, T>& right)
{
  ShAttrib<1, SH_TEMP, T> t;
  shCOND(t, condition, left, right);
  return t;
}

// TODO

template<int N, typename T>
ShGeneric<N, T> operator!(const ShGeneric<N, T>& a)
{
  return 1.0f - (a > 0.0f);
}


template<int N, typename T>
ShGeneric<N, T> operator&&(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return min(a,b);
}

template<int N, typename T>
ShGeneric<N, T> operator||(const ShGeneric<N, T>& a, const ShGeneric<N, T>& b)
{
  return max(a,b);
}

template<int N, typename T>
ShGeneric<1, T> any(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t || a(i);
  }
  return t;
}

template<int N, typename T>
ShGeneric<1, T> all(const ShGeneric<N, T>& a)
{
  ShAttrib<1, SH_TEMP, T> t = a(0);
  for (int i = 1; i < N; i++) {
    t = t && a(i);
  }
  return t;
}

}

#endif
