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
#ifndef SHLIB_HPP
#define SHLIB_HPP

#include <cmath>
#include <cassert>
#include "ShVariable.hpp"
#include "ShGeneric.hpp"
#include "ShAttrib.hpp"
#include "ShTypeInfo.hpp"

/** \defgroup library Library Functions
 */

#define CT1T2 typename ShCommonType<T1, T2>::type
#define CT1T2T3 typename ShCommonType3<T1, T2, T3>::type
#define CT1T2T3T4 typename ShCommonType4<T1, T2, T3, T4>::type

#define SH_SHLIB_CONST_SCALAR_OP(operation) \
template<typename T> \
ShGeneric<1, typename ShCommonType<T, float>::type> \
operation(const ShGeneric<1, T>& left, float right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, float>(right)); \
} \
template<typename T> \
ShGeneric<1, typename ShCommonType<T, float>::type> \
operation(float left, const ShGeneric<1, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, float>(left), right); \
} 

#define SH_SHLIB_CONST_SCALAR_OP_DECL(operation) \
template<typename T> \
ShGeneric<1, typename ShCommonType<T, float>::type > \
operation(const ShGeneric<1, T>& left, float right); \
 \
template<typename T> \
ShGeneric<1, typename ShCommonType<T, float>::type > \
operation(float left, const ShGeneric<1, T>& right);

#define SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type > \
operation(const ShGeneric<N, T>& left, float right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, float>(right)); \
} 

#define SH_SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type> \
operation(const ShGeneric<N, T>& left, float right);

#define SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type> \
operation(float left, const ShGeneric<N, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, float>(left), right); \
} 

#define SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type> \
operation(float left, const ShGeneric<N, T>& right); 

#define SH_SHLIB_CONST_N_OP_LEFT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, N);

#define SH_SHLIB_CONST_N_OP_RIGHT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, N);

#define SH_SHLIB_CONST_N_OP_BOTH(operation) \
SH_SHLIB_CONST_N_OP_LEFT(operation); \
SH_SHLIB_CONST_N_OP_RIGHT(operation);

#define SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(operation, retsize) \
SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize); \
SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize);

#define SH_SHLIB_CONST_N_OP_LEFT_DECL(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, N);

#define SH_SHLIB_CONST_N_OP_RIGHT_DECL(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, N);

#define SH_SHLIB_CONST_N_OP_BOTH_DECL(operation) \
SH_SHLIB_CONST_N_OP_LEFT_DECL(operation); \
SH_SHLIB_CONST_N_OP_RIGHT_DECL(operation);

#define SH_SHLIB_CONST_N_OP_RETSIZE_BOTH_DECL(operation, retsize) \
SH_SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, retsize); \
SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, retsize);

//@todo type fix scalar types here.  Should be arbitrary templated types instead of
// just T , but that casues overload problems 
#define SH_SHLIB_CONST_TRINARY_OP_011_RETSIZE(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type> \
operation(const ShGeneric<N, T>& a, float b, float c) \
{ \
  return operation(a, ShAttrib<1, SH_CONST, float>(b), ShAttrib<1, SH_CONST, float>(c)); \
} 

#define SH_SHLIB_CONST_TRINARY_OP_011_RETSIZE_DECL(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, typename ShCommonType<T, float>::type> \
operation(const ShGeneric<N, T>& a, float b, float c); 

#define SH_SHLIB_CONST_TRINARY_OP_011(operation) \
SH_SHLIB_CONST_TRINARY_OP_011_RETSIZE(operation, N);

#define SH_SHLIB_CONST_TRINARY_OP_011_DECL(operation) \
SH_SHLIB_CONST_TRINARY_OP_011_RETSIZE_DECL(operation, N);

#include "ShLibArith.hpp"
#include "ShLibBoolean.hpp"
#include "ShLibClamp.hpp"
#include "ShLibGeometry.hpp"
#include "ShLibMatrix.hpp"
#include "ShLibMisc.hpp"
#include "ShLibTrig.hpp"
#include "ShLibDeriv.hpp"
#include "ShLibNoise.hpp"
#include "ShLibSplines.hpp"
#include "ShLibWorley.hpp"
// If you add anything to this list, make sure that ShLib.hpp is
// included before the header guard since these libraries must always
// be loaded by ShLib.hpp, no matter who includes them.  See
// ShLibNoise.hpp for an example.

#undef CT1T2 
#undef CT1T2T3 
#undef CT1T2T3T4 

// Semantic stuff

#define SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
ShAttrib<libretsize, SH_TEMP, T, librettype, false> \
libop(const ShAttrib<N, K1, T, libtype, S1>& var) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(var)); \
  return ShAttrib<libretsize, SH_TEMP, T, librettype, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, ShBindingType K2, typename T1, typename T2, bool S1, bool S2> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false> \
libop(const ShAttrib<N, K1, T1, libtype, S1>& left, const ShAttrib<N, K2, T2, libtype, S2>& right) \
{ \
  ShGeneric<libretsize, typename ShCommonType<T1, T2>::type> t = libop(\
      static_cast< ShGeneric<N, T1> >(left), static_cast< ShGeneric<N, T2> >(right)); \
  return ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false>(t.node(), \
      t.swizzle(), t.neg()); \
}

#define SH_SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, ShBindingType K1, ShBindingType K2, typename T1, typename T2, bool S1, bool S2> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false> \
libop(const ShAttrib<N, K1, T1, libtype, S1>& left, const ShAttrib<M, K2, T2, libtype, S2>& right) \
{ \
  ShGeneric<libretsize, typename ShCommonType<T1, T2>::type> t = libop(\
      static_cast< ShGeneric<N, T1> >(left), static_cast< ShGeneric<M, T2> >(right)); \
  return ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, ShBindingType K1, ShBindingType K2, typename T1, typename T2, bool S1, bool S2> \
ShAttrib<M, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false> \
libop(const ShAttrib<1, K2, T1, libtype, S2>& left, const ShAttrib<M, K1, T2, libtype, S1>& right) \
{ \
  ShGeneric<M, typename ShCommonType<T1, T2>::type> t = libop( \
      static_cast< ShGeneric<1, T1> >(left), static_cast< ShGeneric<M, T2> >(right)); \
  return ShAttrib<M, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, ShBindingType K1, ShBindingType K2, typename T1, typename T2, bool S1> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T1, T2>::type, librettype, false> \
libop(const ShMatrix<M, N, K1, T1>& a, const ShAttrib<N, K2, T2, libtype, S1>& b) \
{ \
  ShGeneric<libretsize, typename ShCommonType<T1, T2>::type> t = libop(a, \
      static_cast< ShGeneric<N, T2> >(b)); \
  return ShAttrib<libretsize, K1, typename ShCommonType<T1, T2>::type, librettype, S1>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

// All the scalar constant stuff

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, librettype, libretsize) \
template<ShBindingType K, typename T, bool S> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T, float>::type, librettype, false> \
libop(const ShAttrib<1, K, T, libtype, S>& left, float right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, float>(right)); \
} \
template<ShBindingType K, typename T, bool S> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T, float>::type, librettype, false> \
libop(float left, const ShAttrib<1, K, T, libtype, S>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, float>(left), right); \
} 

#define SH_SHLIB_SPECIAL_CONST_SCALAR_OP(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, libtype, 1)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K, typename T, bool S> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T, float>::type, librettype, false> \
libop(const ShAttrib<N, K, T, libtype, S>& left, float right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, float>(right)); \
} 

#define SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K, typename T, bool S> \
ShAttrib<libretsize, SH_TEMP, typename ShCommonType<T, float>::type, librettype, false> \
libop(float left, const ShAttrib<N, K, T, libtype, S>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, float>(left), right); \
} 

#define SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(libtype, operation, librettype, libretsize) \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, operation, librettype, libretsize); \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, operation, librettype, libretsize);

#define SH_SHLIB_SPECIAL_CONST_N_OP_BOTH(libtype, operation) \
SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, operation); \
SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, operation);

// Standard stuff

#define SH_SHLIB_USUAL_OPERATIONS(type) \
  SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, type)

#define SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, rettype) \
  SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype)\
  SH_SHLIB_UNARY_RETTYPE_OPERATION(type, normalize, rettype, N);       \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(type, abs, rettype, N);           

#define SH_SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype) \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator+, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator+, rettype, 1);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator*, rettype, N); \
SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator*, rettype);    \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator*, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator*, rettype, N);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator/, rettype, N); \
SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator/, rettype);    \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator/, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator/, rettype, N);  \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pow, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pow, rettype, 1);        \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, pow, rettype, N);       \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator==, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator==, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator!=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator!=, rettype, 1); \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, acos, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, asin, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, cos, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, cosh, rettype, N);             \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, dot, rettype, 1);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, dot, rettype, 1);           \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, mod, rettype, N);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, mod, rettype, 1);       \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, frac, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sin, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sinh, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sqrt, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, tan, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, tanh, rettype, N);            \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, min, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, min, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pos, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pos, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, max, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, max, rettype, 1);

// Points have different subtraction, so we don't include them in our "usuals"
#define SH_SHLIB_USUAL_SUBTRACT(type) \
SH_SHLIB_BINARY_OPERATION(type, operator-, N);      \
SH_SHLIB_SPECIAL_CONST_SCALAR_OP(type, operator-);  \


#endif
