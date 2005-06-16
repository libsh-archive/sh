// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHLIBVECTOR_HPP
#define SHLIBVECTOR_HPP

#include "ShAttrib.hpp"
#include "ShLib.hpp"

namespace SH {

SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(SH_VECTOR, SH_VECTOR);

template<int N, ShBindingType B1, typename T, bool S1> 
ShAttrib<N, SH_TEMP, T, SH_VECTOR, false>
abs(const ShAttrib<N, B1, T, SH_VECTOR, S1>& var) 
{ 
  ShGeneric<N, T> t = abs(static_cast< ShGeneric<N, T> >(var)); 
  ShAttrib<N, SH_TEMP, T, SH_VECTOR, false> vec(t.node(), t.swizzle(), t.neg()); 
  return vec;
}

template<int N, ShBindingType B1, typename T, bool S1> 
ShAttrib<N, SH_TEMP, T, SH_VECTOR, false> 
normalize(const ShAttrib<N, B1, T, SH_VECTOR, S1>& var) 
{
  ShGeneric<N, T> t = normalize(static_cast< ShGeneric<N, T> >(var)); 
  ShAttrib<N, SH_TEMP, T, SH_VECTOR, false> vec(t.node(), t.swizzle(), t.neg()); 
  return vec;
}

SH_SHLIB_USUAL_SUBTRACT(SH_VECTOR);

SH_SHLIB_LEFT_MATRIX_OPERATION(SH_VECTOR, operator|, M);

template<int N, ShBindingType B1, ShBindingType B2, typename T, bool S1, bool S2>
ShGeneric<1, T>
operator|(const ShAttrib<N, B1, T, SH_VECTOR, S1>& a, const ShAttrib<N, B2, T, SH_VECTOR, S2>& b)
{
  return dot(a, b);
}

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<3, SH_TEMP, T, SH_VECTOR, false> 
operator|(const ShMatrix<4, 4, B1, T>& m, const ShAttrib<3, B2, T, SH_VECTOR, S1>& v)
{
  ShAttrib<3, SH_TEMP, T, SH_VECTOR, false> t;
  for (int i = 0; i < 3; i++) {
    t(i) = dot(m[i](0,1,2), v);
  }
  return t;
}

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<2, SH_TEMP, T, SH_VECTOR, false>
operator|(const ShMatrix<3, 3, B1, T>& m, const ShAttrib<2, B2, T, SH_VECTOR, S1>& v)
{
  ShAttrib<2, SH_TEMP, T, SH_VECTOR, false> t;
  for (int i = 0; i < 2; i++) {
    t(i) = dot(m[i](0,1), v);
  }
  return t;
}

template<ShBindingType B1, ShBindingType B2, typename T, bool S1>
ShAttrib<1, SH_TEMP, T, SH_VECTOR, false>
operator|(const ShMatrix<2, 2, B1, T>& m, const ShAttrib<1, B2, T, SH_VECTOR, S1>& v)
{
  ShAttrib<1, SH_TEMP, T, SH_VECTOR, false> t;
  for (int i = 0; i < 1; i++) {
    t(i) = dot(m[i](0), v);
  }
  return t;
}

}

#endif
