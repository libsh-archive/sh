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
#ifndef SHCONCRETECTYPEOPIMPL_HPP 
#define SHCONCRETECTYPEOPIMPL_HPP

#include <numeric>
#include <cmath>
#include "Eval.hpp"
#include "Variant.hpp"
#include "Debug.hpp"
#include "Error.hpp"
#include "TypeInfo.hpp"

namespace {
/** Integer power (T2 must be an integer) - Russian peasant algorithm */
template<typename T1, typename T2>
T1 _sh_intpow(T1 x, T2 pow)
{
  T1 result = 1;
  if (pow < 0) { 
    x = 1 / x;
    pow = -pow;
  }
  for (;pow; pow >>= 1, x *= x) {
    if (pow & 1) result *= x;
  }
  return result;
}
}

namespace SH {

/* Partial specialization for different operations */ 

/* The operators that depend on cmath different library functions
 * are specialized for float and double types in Eval.cpp 
 * right now
 */
// TODO make this cleaner?

// TODO use the information from sdt's OperationInfo to decide
// whether to do the ao/bo/co = 0 special case for scalar
// ops where we step through the destination tuple but always
// use the same element from the scalar src tuple. 

// macros for componentwise operations
// do a partial specialization on the class
// and define the doop function
#define CCTO_UNARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteCTypeOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    DEBUG_ASSERT(dest && a);\
    int ao = a->size() > 1;\
  \
    typename Variant::iterator D = dest->begin();\
    typename Variant::const_iterator A = a->begin();\
    for(; D != dest->end(); A += ao, ++D) {\
      (*D) = opsrc;\
    }\
  } \
};


#define CCTO_BINARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteCTypeOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    DEBUG_ASSERT(dest && a && b);\
    int ao = a->size() > 1;\
    int bo = b->size() > 1;\
  \
    typename Variant::iterator D = dest->begin();\
    typename Variant::const_iterator A, B;\
    A = a->begin();\
    B = b->begin();\
    for(; D != dest->end(); A += ao, B += bo, ++D) {\
      (*D) = opsrc;\
    }\
  } \
};


#define CCTO_TERNARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteCTypeOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    DEBUG_ASSERT(dest && a && b && c);\
    int ao = a->size() > 1;\
    int bo = b->size() > 1;\
    int co = c->size() > 1;\
  \
    typename Variant::iterator D = dest->begin();\
    typename Variant::const_iterator A, B, C;\
    A = a->begin();\
    B = b->begin();\
    C = c->begin();\
    for(; D != dest->end(); A += ao, B += bo, C += co, ++D) {\
      (*D) = opsrc;\
    }\
  }\
};

#define CCTO_OP_SPEC(T, op)\
template<>\
struct  \
DLLEXPORT \
ConcreteCTypeOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0); \
};

#define CCTO_OP_CMATH_SPEC(op)\
  CCTO_OP_SPEC(double, op);\
  CCTO_OP_SPEC(float, op);

// Note that some ops are currently NOT declared for integer types
// (anything that is only specialized for double/float cmath functions 
// will not work with ints)

// Unary ops
CCTO_UNARY_OP(OP_ABS, (dataTypeIsPositive(*A) ? (*A) : -(*A)));

CCTO_OP_CMATH_SPEC(OP_ACOS);
CCTO_OP_CMATH_SPEC(OP_ACOSH);
CCTO_OP_CMATH_SPEC(OP_ASIN);
CCTO_OP_CMATH_SPEC(OP_ASINH);

CCTO_UNARY_OP(OP_ASN, (*A));

CCTO_OP_CMATH_SPEC(OP_ATAN);
CCTO_OP_CMATH_SPEC(OP_ATANH);
CCTO_OP_CMATH_SPEC(OP_CBRT);
CCTO_OP_CMATH_SPEC(OP_CEIL);

template<typename T>
struct ConcreteCTypeOp<OP_CMUL, T>
{ 
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::accumulate(a->begin(), a->end(), 
                     DataTypeInfo<T, HOST>::One, 
                     std::multiplies<typename Variant::CppDataType>());
  }
};

CCTO_OP_CMATH_SPEC(OP_COS);

template<typename T>
struct ConcreteCTypeOp<OP_CSUM, T>
{ 
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::accumulate(a->begin(), a->end(), 
                     DataTypeInfo<T, HOST>::Zero, 
                     std::plus<typename Variant::CppDataType>()); 
  }
};

CCTO_OP_CMATH_SPEC(OP_EXP);
CCTO_OP_CMATH_SPEC(OP_EXP2);
CCTO_OP_CMATH_SPEC(OP_EXP10);
CCTO_OP_CMATH_SPEC(OP_FLR);
CCTO_OP_CMATH_SPEC(OP_FRAC);
CCTO_OP_CMATH_SPEC(OP_LOG);
CCTO_OP_CMATH_SPEC(OP_LOG2);
CCTO_OP_CMATH_SPEC(OP_LOG10);
CCTO_OP_CMATH_SPEC(OP_NORM);

CCTO_UNARY_OP(OP_RCP, 1 / (*A));

CCTO_OP_CMATH_SPEC(OP_RND);
CCTO_OP_CMATH_SPEC(OP_RSQ);
CCTO_OP_CMATH_SPEC(OP_SIN);

CCTO_UNARY_OP(OP_SGN, (*A) > 0 ? 1 : (*A) < 0 ? -1 : 0); 

CCTO_OP_CMATH_SPEC(OP_SQRT);
CCTO_OP_CMATH_SPEC(OP_TAN);

template<typename T>
struct ConcreteCTypeOp<OP_LIT, T>
{ 
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    typename Variant::CppDataType x, y, w;
    x = (*a)[0];
    if (x < 0) x = 0;
    y = (*a)[1];
    if (y < 0) y = 0;
    w = (*a)[2];
    if (w < -128) w = -128;
    if (w > 128) w = 128;
    (*dest)[0] = 1;
    (*dest)[1] = x;
    (*dest)[2] = (x > 0) ? std::pow(y,w) : 0;
    (*dest)[3] = 1;
  }
};

template<typename T>
struct ConcreteCTypeOp<OP_HASH, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    DEBUG_ASSERT(dest && a);
    typename Variant::iterator D = dest->begin();
    typename Variant::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A);
    // TODO: implement the real algorithm
  }
};

template<typename T>
struct ConcreteCTypeOp<OP_NOISE, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    DEBUG_ASSERT(dest && a);
    typename Variant::iterator D = dest->begin();
    typename Variant::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A);
    // TODO: implement the real algorithm
  }
};

// Binary ops
CCTO_BINARY_OP(OP_ADD, (*A) + (*B));

CCTO_OP_CMATH_SPEC(OP_ATAN2);

CCTO_BINARY_OP(OP_DIV, (*A) / (*B));

template<typename T>
struct ConcreteCTypeOp<OP_DOT, T>
{ 
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::inner_product(a->begin(), a->end(), b->begin(), 
                      DataTypeInfo<T, HOST>::Zero);
  }
};

CCTO_BINARY_OP(OP_MAX, std::max((*A), (*B))); 
CCTO_BINARY_OP(OP_MIN, std::min((*A), (*B))); 

CCTO_BINARY_OP(OP_MOD, ((*A) - (*B) * (int)std::floor((double)(*A) / (*B)))); 
CCTO_OP_CMATH_SPEC(OP_MOD);

CCTO_BINARY_OP(OP_MUL, (*A) * (*B));

// TODO implemeng power for win32
CCTO_BINARY_OP(OP_POW, _sh_intpow((*A), (*B)));  // only works for integers
CCTO_OP_CMATH_SPEC(OP_POW);


CCTO_BINARY_OP(OP_SEQ, (dataTypeCond<T, HOST>((*A) == (*B))));
CCTO_BINARY_OP(OP_SGE, (dataTypeCond<T, HOST>((*A) >= (*B))));
CCTO_BINARY_OP(OP_SGT, (dataTypeCond<T, HOST>((*A) > (*B))));
CCTO_BINARY_OP(OP_SLE, (dataTypeCond<T, HOST>((*A) <= (*B))));
CCTO_BINARY_OP(OP_SLT, (dataTypeCond<T, HOST>((*A) < (*B))));
CCTO_BINARY_OP(OP_SNE, (dataTypeCond<T, HOST>((*A) != (*B))));

template<typename T>
struct ConcreteCTypeOp<OP_XPD, T>
{ 
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // handle case where dest = a and both not swizzed/negged
    // or similar with dest = b.
    typename Variant::CppDataType t0, t1;
    t0 = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
    t1 = -((*a)[0] * (*b)[2] - (*a)[2] * (*b)[0]);
    (*dest)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];
    (*dest)[0] = t0;
    (*dest)[1] = t1;
  }
};

// Ternary Ops
CCTO_TERNARY_OP(OP_COND, ((*A) > 0 ? (*B) : (*C))); 
CCTO_TERNARY_OP(OP_LRP, (*A) * (*B) + (1 - (*A)) * (*C)); 
CCTO_TERNARY_OP(OP_MAD, (*A) * (*B) + (*C)); 

}

#endif
