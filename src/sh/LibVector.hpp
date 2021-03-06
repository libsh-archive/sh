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
#ifndef SHLIBVECTOR_HPP
#define SHLIBVECTOR_HPP

#include "Attrib.hpp"
#include "Lib.hpp"

namespace SH {

SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(SH_VECTOR, SH_VECTOR);

template<int N, BindingType B1, typename T, bool S1> 
Attrib<N, SH_TEMP, T, SH_VECTOR, false>
abs(const Attrib<N, B1, T, SH_VECTOR, S1>& var) 
{ 
  Generic<N, T> t = abs(static_cast< Generic<N, T> >(var)); 
  Attrib<N, SH_TEMP, T, SH_VECTOR, false> vec(t.node(), t.swizzle(), t.neg()); 
  return vec;
}

template<int N, BindingType B1, typename T, bool S1> 
Attrib<N, SH_TEMP, T, SH_VECTOR, false> 
normalize(const Attrib<N, B1, T, SH_VECTOR, S1>& var) 
{
  Generic<N, T> t = normalize(static_cast< Generic<N, T> >(var)); 
  Attrib<N, SH_TEMP, T, SH_VECTOR, false> vec(t.node(), t.swizzle(), t.neg()); 
  return vec;
}

SHLIB_USUAL_SUBTRACT(SH_VECTOR);

SHLIB_LEFT_MATRIX_OPERATION(SH_VECTOR, operator|, M);

template<int N, BindingType B1, BindingType B2, typename T, bool S1, bool S2>
Generic<1, T>
operator|(const Attrib<N, B1, T, SH_VECTOR, S1>& a, const Attrib<N, B2, T, SH_VECTOR, S2>& b)
{
  return dot(a, b);
}

}

#endif
