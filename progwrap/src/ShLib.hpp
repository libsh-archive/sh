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
#ifndef SHLIB_HPP
#define SHLIB_HPP

#include <cmath>
#include <cassert>
#include "ShVariable.hpp"
#include "ShGeneric.hpp"
#include "ShAttrib.hpp"

/** \defgroup library Library Functions
 */

#define SH_SHLIB_CONST_SCALAR_OP(operation) \
template<typename T> \
ShGeneric<1, T> operation(const ShGeneric<1, T>& left, T right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<typename T> \
ShGeneric<1, T> operation(T left, const ShGeneric<1, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<typename T> \
ShGeneric<1, T> operation(const ShGeneric<1, T>& left, double right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<typename T> \
ShGeneric<1, T> operation(double left, const ShGeneric<1, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, T> operation(const ShGeneric<N, T>& left, T right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<int N, typename T> \
ShGeneric<retsize, T> operation(const ShGeneric<N, T>& left, double right) \
{ \
  return operation(left, ShAttrib<1, SH_CONST, T>(right)); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize) \
template<int N, typename T> \
ShGeneric<retsize, T> operation(T left, const ShGeneric<N, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<int N, typename T> \
ShGeneric<retsize, T> operation(double left, const ShGeneric<N, T>& right) \
{ \
  return operation(ShAttrib<1, SH_CONST, T>(left), right); \
}

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

#include "ShLibArith.hpp"
#include "ShLibBoolean.hpp"
#include "ShLibClamp.hpp"
#include "ShLibGeometry.hpp"
#include "ShLibMatrix.hpp"
#include "ShLibMisc.hpp"
#include "ShLibTrig.hpp"

// Semantic stuff

#define SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& var) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(var)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<N, K2, T, S2>& right) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(left), \
                                       static_cast< ShGeneric<N, T> >(right)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<M, K2, T, S2>& right) \
{ \
  ShGeneric<libretsize, T> t = libop(static_cast< ShGeneric<N, T> >(left), \
                                       static_cast< ShGeneric<M, T> >(right)); \
  return librettype<libretsize, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, ShBindingType K1, ShBindingType K2, typename T, bool S1, bool S2> \
librettype<M, SH_TEMP, T, false> \
libop(const libtype<1, K2, T, S2>& left, const libtype<M, K1, T, S1>& right) \
{ \
  ShGeneric<M, T> t = libop(static_cast< ShGeneric<1, T> >(left), \
                              static_cast< ShGeneric<M, T> >(right)); \
  return librettype<M, SH_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, ShBindingType K1, ShBindingType K2, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> libop(const ShMatrix<M, N, K1, T>& a, \
                                                    const libtype<N, K2, T, S1>& b) \
{ \
  ShGeneric<libretsize, T> t = libop(a, \
                                       static_cast< ShGeneric<N, T> >(b)); \
  return librettype<libretsize, K1, T, S1>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

// All the scalar constant stuff

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, librettype, libretsize) \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(T left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(double left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
}

#define SH_SHLIB_SPECIAL_CONST_SCALAR_OP(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, libtype, 1)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
} \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShAttrib<1, SH_CONST, T>(right)); \
}
#define SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, librettype, libretsize) \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(T left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
} \
template<int N, ShBindingType K1, typename T, bool S1> \
librettype<libretsize, SH_TEMP, T, false> \
libop(double left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShAttrib<1, SH_CONST, T>(left), right); \
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
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator/, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(type, operator/, rettype, N);  \
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
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, dot, rettype, 1);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, dot, rettype, 1);           \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, mod, rettype, N);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, mod, rettype, 1);       \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, frac, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sin, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sqrt, rettype, N);            \
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
