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
#ifndef SHCONCRETEREGULAROPIMPL_HPP 
#define SHCONCRETEREGULAROPIMPL_HPP
#include <numeric>
#include <cmath>
#include "Eval.hpp"
#include "Debug.hpp"
#include "Error.hpp"
#include "TypeInfo.hpp"
#include "Variant.hpp"

namespace SH {

/* Partial specialization for different operations */ 
/* The operators that depend on cmath different library functions
 * are specialized for float and double types in Eval.cpp 
 * right now
 */
// TODO make this cleaner?
// TODO use the information from sdt's OperationInfo to decide
// whether to do the ao/bo/co special case for scalar
// ops where we step through the destination tuple but always
// use the same element from the scalar src tuple. 
// macros for componentwise operations
// do a partial specialization on the class
// and define the doop function
//
#define CRO_UNARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteRegularOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a);\
    int ao = a->size() > 1;\
  \
    typename Variant::iterator D = dest->begin();\
    typename Variant::const_iterator A = a->begin();\
    for(; D != dest->end(); A += ao, ++D) {\
      (*D) = opsrc;\
    }\
  } \
};

#define CRO_BINARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteRegularOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a && b);\
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

#define CRO_TERNARY_OP(op, opsrc)\
template<typename T>\
struct ConcreteRegularOp<op, T>\
{ \
  typedef DataVariant<T, HOST> Variant; \
  typedef Variant* DataPtr; \
  typedef const Variant* DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a && b && c);\
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

// Unary ops
CRO_UNARY_OP(OP_ABS, abs(*A));
CRO_UNARY_OP(OP_ACOS, acos(*A));
CRO_UNARY_OP(OP_ASIN, asin(*A));
CRO_UNARY_OP(OP_ASN, (*A));
CRO_UNARY_OP(OP_ATAN, atan(*A));
CRO_UNARY_OP(OP_CBRT, cbrt(*A));
CRO_UNARY_OP(OP_CEIL, ceil(*A));

#ifdef _WIN32
CRO_UNARY_OP(OP_ACOSH, log((*A) + sqrt((*A) * (*A) - 1)));
CRO_UNARY_OP(OP_ASINH, log((*A) + sqrt((*A) * (*A) + 1)));
CRO_UNARY_OP(OP_ATANH, log((1.0 + (*A))/(1.0 - (*A)))/2.0);
#else
CRO_UNARY_OP(OP_ACOSH, acosh(*A));
CRO_UNARY_OP(OP_ASINH, asinh(*A));
CRO_UNARY_OP(OP_ATANH, atanh(*A));
#endif

template<typename T>
struct ConcreteRegularOp<OP_CMUL, T>
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

CRO_UNARY_OP(OP_COS, cos(*A));
CRO_UNARY_OP(OP_COSH, cosh(*A));

template<typename T>
struct ConcreteRegularOp<OP_CSUM, T>
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

CRO_UNARY_OP(OP_EXP, exp(*A));
CRO_UNARY_OP(OP_EXP2, exp2(*A));
CRO_UNARY_OP(OP_EXP10, exp10(*A));
CRO_UNARY_OP(OP_FLR, floor(*A));
CRO_UNARY_OP(OP_FRAC, frac(*A));
CRO_UNARY_OP(OP_LOG, log(*A));
CRO_UNARY_OP(OP_LOG2, log(*A));
CRO_UNARY_OP(OP_LOG10, log10(*A));

template<typename T>
struct ConcreteRegularOp<OP_LIT, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 
  typedef typename Variant::CppDataType DataType;

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    DataType x, y, w;
    x = (*a)[0];
    x = max(x, static_cast<DataType>(0));
    y = (*a)[1];
    y = max(y, static_cast<DataType>(0));
    w = (*a)[2];
    w = min(w, static_cast<DataType>(128)); 
    w = max(w, static_cast<DataType>(-128)); 
    (*dest)[0] = 1;
    (*dest)[1] = x;
    // @todo - assumes bool \in {0,1} - maybe should not, or should write this
    // down somewhere as a requirement for new/user-defined storage types
    (*dest)[2] = (x > static_cast<DataType>(0)) * pow(y,w); 
    (*dest)[3] = 1;
  }
};

template<typename T>
struct ConcreteRegularOp<OP_HASH, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    SH_DEBUG_ASSERT(dest && a);
    typename Variant::iterator D = dest->begin();
    typename Variant::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A);
    // TODO: implement the real algorithm
  }
};

template<typename T>
struct ConcreteRegularOp<OP_NOISE, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    SH_DEBUG_ASSERT(dest && a);
    typename Variant::iterator D = dest->begin();
    typename Variant::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A);
    // TODO: implement the real algorithm
  }
};

template<typename T>
struct ConcreteRegularOp<OP_NORM, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    SH_DEBUG_ASSERT(dest && a);
    typename Variant::CppDataType m = sqrt(std::inner_product(a->begin(), a->end(), 
          a->begin(), DataTypeInfo<T, HOST>::Zero));
  
    typename Variant::iterator D = dest->begin();
    typename Variant::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A) / m;
  }
};

CRO_UNARY_OP(OP_RCP, rcp(*A));
CRO_UNARY_OP(OP_RND, rnd(*A));
CRO_UNARY_OP(OP_RSQ, rsq(*A));
CRO_UNARY_OP(OP_SIN, sin(*A));
CRO_UNARY_OP(OP_SINH, sinh(*A));
CRO_UNARY_OP(OP_SGN, sgn(*A));
CRO_UNARY_OP(OP_SQRT, sqrt(*A));
CRO_UNARY_OP(OP_TAN, tan(*A));
CRO_UNARY_OP(OP_TANH, tan(*A));

// Binary ops
CRO_BINARY_OP(OP_ADD, (*A) + (*B));
CRO_BINARY_OP(OP_ATAN2, atan2((*A), (*B)));
CRO_BINARY_OP(OP_DIV, (*A) / (*B));
CRO_BINARY_OP(OP_MAX, max((*A), (*B))); 
CRO_BINARY_OP(OP_MIN, min((*A), (*B))); 
CRO_BINARY_OP(OP_MOD, (*A) % (*B)); 
CRO_BINARY_OP(OP_MUL, (*A) * (*B));
CRO_BINARY_OP(OP_POW, pow((*A), (*B)));

template<typename T>
struct ConcreteRegularOp<OP_DOT, T>
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

CRO_BINARY_OP(OP_SEQ, (*A) == (*B));
CRO_BINARY_OP(OP_SGE, (*A) >= (*B));
CRO_BINARY_OP(OP_SGT, (*A) > (*B));
CRO_BINARY_OP(OP_SLE, (*A) <= (*B));
CRO_BINARY_OP(OP_SLT, (*A) < (*B));
CRO_BINARY_OP(OP_SNE, (*A) != (*B));

template<typename T>
struct ConcreteRegularOp<OP_XPD, T>
{
  typedef DataVariant<T, HOST> Variant; 
  typedef Variant* DataPtr; 
  typedef const Variant* DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    (*dest)[0] = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
    (*dest)[1] = -((*a)[0] * (*b)[2] - (*a)[2] * (*b)[0]);
    (*dest)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];
  }
};

// Ternary Ops
CRO_TERNARY_OP(OP_COND, cond(*A, *B, *C)); 
CRO_TERNARY_OP(OP_LRP, lerp(*A, *B, *C));
CRO_TERNARY_OP(OP_MAD, (*A) * (*B) + (*C));

}
#endif
