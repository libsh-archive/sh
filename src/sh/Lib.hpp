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
#include "Variable.hpp"
#include "Generic.hpp"
#include "Attrib.hpp"
#include "TypeInfo.hpp"

/** \defgroup library Library Functions
 */

#define CT1T2 typename CommonType<T1, T2>::type
#define CT1T2T3 typename CommonType3<T1, T2, T3>::type
#define CT1T2T3T4 typename CommonType4<T1, T2, T3, T4>::type

#define SHLIB_CONST_SCALAR_OP(operation) \
template<typename T> \
Generic<1, T> \
operation(const Generic<1, T>& left, double right) \
{ \
  return operation(left, Attrib<1, CONST, T>(static_cast<T>(right))); \
} \
template<typename T> \
Generic<1, T> \
operation(double left, const Generic<1, T>& right) \
{ \
  return operation(Attrib<1, CONST, T>(left), right); \
} 

#define SHLIB_CONST_SCALAR_OP_DECL(operation) \
template<typename T> \
Generic<1, T> \
operation(const Generic<1, T>& left, double right); \
 \
template<typename T> \
Generic<1, T> \
operation(double left, const Generic<1, T>& right);

#define SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(const Generic<N, T>& left, double right) \
{ \
  return operation(left, Attrib<1, CONST, T>(static_cast<T>(right))); \
} 

#define SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(const Generic<N, T>& left, double right);

#define SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(double left, const Generic<N, T>& right) \
{ \
  return operation(Attrib<1, CONST, T>(static_cast<T>(left)), right); \
} 

#define SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(double left, const Generic<N, T>& right); 

#define SHLIB_CONST_N_OP_LEFT(operation) \
  SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, N);

#define SHLIB_CONST_N_OP_RIGHT(operation) \
  SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, N);

#define SHLIB_CONST_N_OP_BOTH(operation) \
SHLIB_CONST_N_OP_LEFT(operation); \
SHLIB_CONST_N_OP_RIGHT(operation);

#define SHLIB_CONST_N_OP_RETSIZE_BOTH(operation, retsize) \
SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize); \
SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize);

#define SHLIB_CONST_N_OP_LEFT_DECL(operation) \
  SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, N);

#define SHLIB_CONST_N_OP_RIGHT_DECL(operation) \
  SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, N);

#define SHLIB_CONST_N_OP_BOTH_DECL(operation) \
SHLIB_CONST_N_OP_LEFT_DECL(operation); \
SHLIB_CONST_N_OP_RIGHT_DECL(operation);

#define SHLIB_CONST_N_OP_RETSIZE_BOTH_DECL(operation, retsize) \
SHLIB_CONST_N_OP_RETSIZE_LEFT_DECL(operation, retsize); \
SHLIB_CONST_N_OP_RETSIZE_RIGHT_DECL(operation, retsize);

//@todo type fix scalar types here.  Should be arbitrary templated types instead of
// just T , but that casues overload problems 
#define SHLIB_CONST_TRINARY_OP_011_RETSIZE(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(const Generic<N, T>& a, double b, double c) \
{ \
  return operation(a, Attrib<1, CONST, T>(b), Attrib<1, CONST, T>(c)); \
} 

#define SHLIB_CONST_TRINARY_OP_011_RETSIZE_DECL(operation, retsize) \
template<int N, typename T> \
Generic<retsize, T> \
operation(const Generic<N, T>& a, double b, double c); 

#define SHLIB_CONST_TRINARY_OP_011(operation) \
SHLIB_CONST_TRINARY_OP_011_RETSIZE(operation, N);

#define SHLIB_CONST_TRINARY_OP_011_DECL(operation) \
SHLIB_CONST_TRINARY_OP_011_RETSIZE_DECL(operation, N);

#include "LibArith.hpp"
#include "LibBoolean.hpp"
#include "LibClamp.hpp"
#include "LibGeometry.hpp"
#include "LibMatrix.hpp"
#include "LibMisc.hpp"
#include "LibTrig.hpp"
#include "LibDeriv.hpp"
#include "LibNoise.hpp"
#include "LibSplines.hpp"
#include "LibWorley.hpp"
// If you add anything to this list, make sure that Lib.hpp is
// included before the header guard since these libraries must always
// be loaded by Lib.hpp, no matter who includes them.  See
// LibNoise.hpp for an example.

#undef CT1T2 
#undef CT1T2T3 
#undef CT1T2T3T4 

// Semantic stuff

#define SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, BindingType K1, typename T, bool S1> \
Attrib<libretsize, TEMP, T, librettype, false> \
libop(const Attrib<N, K1, T, libtype, S1>& var) \
{ \
  Generic<libretsize, T> t = libop(static_cast< Generic<N, T> >(var)); \
  return Attrib<libretsize, TEMP, T, librettype, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, BindingType K1, BindingType K2, typename T1, typename T2, bool S1, bool S2> \
Attrib<libretsize, TEMP, typename CommonType<T1, T2>::type, librettype, false> \
libop(const Attrib<N, K1, T1, libtype, S1>& left, const Attrib<N, K2, T2, libtype, S2>& right) \
{ \
  Generic<libretsize, typename CommonType<T1, T2>::type> t = libop(\
      static_cast< Generic<N, T1> >(left), static_cast< Generic<N, T2> >(right)); \
  return Attrib<libretsize, TEMP, typename CommonType<T1, T2>::type, librettype, false>(t.node(), \
      t.swizzle(), t.neg()); \
}

#define SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, BindingType K1, BindingType K2, typename T1, typename T2, bool S1, bool S2> \
Attrib<libretsize, TEMP, typename CommonType<T1, T2>::type, librettype, false> \
libop(const Attrib<N, K1, T1, libtype, S1>& left, const Attrib<M, K2, T2, libtype, S2>& right) \
{ \
  Generic<libretsize, typename CommonType<T1, T2>::type> t = libop(\
      static_cast< Generic<N, T1> >(left), static_cast< Generic<M, T2> >(right)); \
  return Attrib<libretsize, TEMP, typename CommonType<T1, T2>::type, librettype, false>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, BindingType K1, BindingType K2, typename T1, typename T2, bool S1, bool S2> \
Attrib<M, TEMP, typename CommonType<T1, T2>::type, librettype, false> \
libop(const Attrib<1, K2, T1, libtype, S2>& left, const Attrib<M, K1, T2, libtype, S1>& right) \
{ \
  Generic<M, typename CommonType<T1, T2>::type> t = libop( \
      static_cast< Generic<1, T1> >(left), static_cast< Generic<M, T2> >(right)); \
  return Attrib<M, TEMP, typename CommonType<T1, T2>::type, librettype, false>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, BindingType K1, BindingType K2, typename T1, typename T2, bool S1> \
Attrib<libretsize, TEMP, typename CommonType<T1, T2>::type, librettype, false> \
libop(const Matrix<M, N, K1, T1>& a, const Attrib<N, K2, T2, libtype, S1>& b) \
{ \
  Generic<libretsize, typename CommonType<T1, T2>::type> t = libop(a, \
      static_cast< Generic<N, T2> >(b)); \
  return Attrib<libretsize, K1, typename CommonType<T1, T2>::type, librettype, S1>(\
      t.node(), t.swizzle(), t.neg()); \
}

#define SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

// All the scalar constant stuff

#define SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, librettype, libretsize) \
template<BindingType K, typename T, bool S> \
Attrib<libretsize, TEMP, T, librettype, false> \
libop(const Attrib<1, K, T, libtype, S>& left, double right) \
{ \
  return libop(left, Attrib<1, CONST, T>(right)); \
} \
template<BindingType K, typename T, bool S> \
Attrib<libretsize, TEMP, T, librettype, false> \
libop(double left, const Attrib<1, K, T, libtype, S>& right) \
{ \
  return libop(Attrib<1, CONST, T>(left), right); \
} 

#define SHLIB_SPECIAL_CONST_SCALAR_OP(libtype, libop) \
  SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, libtype, 1)

#define SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, librettype, libretsize) \
template<int N, BindingType K, typename T, bool S> \
Attrib<libretsize, TEMP, T, librettype, false> \
libop(const Attrib<N, K, T, libtype, S>& left, double right) \
{ \
  return libop(left, Attrib<1, CONST, T>(right)); \
} 

#define SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, libop) \
  SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, libtype, N)

#define SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, librettype, libretsize) \
template<int N, BindingType K, typename T, bool S> \
Attrib<libretsize, TEMP, T, librettype, false> \
libop(double left, const Attrib<N, K, T, libtype, S>& right) \
{ \
  return libop(Attrib<1, CONST, T>(left), right); \
} 

#define SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, libop) \
  SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, libtype, N)

#define SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(libtype, operation, librettype, libretsize) \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, operation, librettype, libretsize); \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, operation, librettype, libretsize);

#define SHLIB_SPECIAL_CONST_N_OP_BOTH(libtype, operation) \
SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, operation); \
SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, operation);

// Standard stuff

#define SHLIB_USUAL_OPERATIONS(type) \
  SHLIB_USUAL_OPERATIONS_RETTYPE(type, type)

#define SHLIB_USUAL_OPERATIONS_RETTYPE(type, rettype) \
  SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype)\
  SHLIB_UNARY_RETTYPE_OPERATION(type, normalize, rettype, N);       \
  SHLIB_UNARY_RETTYPE_OPERATION(type, abs, rettype, N);           

#define SHLIB_USUAL_NON_UNIT_OPS_RETTYPE(type, rettype) \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator+, rettype, N);      \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator+, rettype, 1);  \
                                                        \
SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator*, rettype, N); \
SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator*, rettype);    \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator*, rettype, 1);  \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator*, rettype, N);  \
                                                        \
SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator/, rettype, N); \
SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator/, rettype);    \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator/, rettype, 1);  \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator/, rettype, N);  \
                                                        \
SHLIB_BINARY_RETTYPE_OPERATION(type, pow, rettype, N);            \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pow, rettype, 1);        \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, pow, rettype, N);       \
                                                        \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator<, rettype, N);      \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<, rettype, 1);  \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator<=, rettype, N);     \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<=, rettype, 1); \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator>, rettype, N);      \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>, rettype, 1);  \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator>=, rettype, N);     \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>=, rettype, 1); \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator==, rettype, N);     \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator==, rettype, 1); \
SHLIB_BINARY_RETTYPE_OPERATION(type, operator!=, rettype, N);     \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator!=, rettype, 1); \
                                                        \
SHLIB_UNARY_RETTYPE_OPERATION(type, acos, rettype, N);            \
SHLIB_UNARY_RETTYPE_OPERATION(type, asin, rettype, N);            \
SHLIB_UNARY_RETTYPE_OPERATION(type, cos, rettype, N);             \
SHLIB_UNARY_RETTYPE_OPERATION(type, cosh, rettype, N);             \
SHLIB_BINARY_RETTYPE_OPERATION(type, dot, rettype, 1);           \
SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, dot, rettype, 1);           \
SHLIB_BINARY_RETTYPE_OPERATION(type, mod, rettype, N);           \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, mod, rettype, 1);       \
                                                        \
SHLIB_UNARY_RETTYPE_OPERATION(type, frac, rettype, N);            \
SHLIB_UNARY_RETTYPE_OPERATION(type, sin, rettype, N);             \
SHLIB_UNARY_RETTYPE_OPERATION(type, sinh, rettype, N);             \
SHLIB_UNARY_RETTYPE_OPERATION(type, sqrt, rettype, N);            \
SHLIB_UNARY_RETTYPE_OPERATION(type, tan, rettype, N);            \
SHLIB_UNARY_RETTYPE_OPERATION(type, tanh, rettype, N);            \
                                                        \
SHLIB_BINARY_RETTYPE_OPERATION(type, min, rettype, N);            \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, min, rettype, 1);        \
SHLIB_BINARY_RETTYPE_OPERATION(type, pos, rettype, N);            \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pos, rettype, 1);        \
SHLIB_BINARY_RETTYPE_OPERATION(type, max, rettype, N);            \
SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, max, rettype, 1);

// Points have different subtraction, so we don't include them in our "usuals"
#define SHLIB_USUAL_SUBTRACT(type) \
SHLIB_BINARY_OPERATION(type, operator-, N);      \
SHLIB_SPECIAL_CONST_SCALAR_OP(type, operator-);  \


#endif
