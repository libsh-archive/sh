#ifndef SHCONCRETECTYPEOPIMPL_HPP 
#define SHCONCRETECTYPEOPIMPL_HPP

#include <numeric>
#include <ext/numeric> // TODO implement clone of std::power(T, int) for non-GNU libstdc++ compatibility (using the "Russian peasant algorithm")
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

/* Partial specialization for different operations */ 

/* The operators that depend on cmath different library functions
 * are specialized for float and double types in ShEval.cpp 
 * right now
 */
// TODO make this cleaner?

// TODO use the information from sdt's ShOperationInfo to decide
// whether to do the ao/bo/co = 0 special case for scalar
// ops where we step through the destination tuple but always
// use the same element from the scalar src tuple. 

// macros for componentwise operations
// do a partial specialization on the class
// and define the doop function
#define SHCCTO_UNARY_OP(op, opsrc)\
template<ShValueType V>\
struct ShConcreteCTypeOp<op, V>\
{ \
  typedef ShDataVariant<V, SH_HOST> Variant; \
  typedef ShPointer<Variant> DataPtr; \
  typedef ShPointer<const Variant> DataCPtr; \
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


#define SHCCTO_BINARY_OP(op, opsrc)\
template<ShValueType V>\
struct ShConcreteCTypeOp<op, V>\
{ \
  typedef ShDataVariant<V, SH_HOST> Variant; \
  typedef ShPointer<Variant> DataPtr; \
  typedef ShPointer<const Variant> DataCPtr; \
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


#define SHCCTO_TERNARY_OP(op, opsrc)\
template<ShValueType V>\
struct ShConcreteCTypeOp<op, V>\
{ \
  typedef ShDataVariant<V, SH_HOST> Variant; \
  typedef ShPointer<Variant> DataPtr; \
  typedef ShPointer<const Variant> DataCPtr; \
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

#define SHCCTO_OP_SPEC(V, op)\
template<>\
struct  \
SH_DLLEXPORT \
ShConcreteCTypeOp<op, V>\
{ \
  typedef ShDataVariant<V, SH_HOST> Variant; \
  typedef ShPointer<Variant> DataPtr; \
  typedef ShPointer<const Variant> DataCPtr; \
\
  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0); \
};

#define SHCCTO_OP_CMATH_SPEC(op)\
  SHCCTO_OP_SPEC(SH_DOUBLE, op);\
  SHCCTO_OP_SPEC(SH_FLOAT, op);

// Note that some ops are currently NOT declared for integer types
// (anything that is only specialized for double/float cmath functions 
// will not work with ints)

// Unary ops
SHCCTO_UNARY_OP(SH_OP_ABS, (shDataTypeIsPositive(*A) ? (*A) : -(*A)));

SHCCTO_OP_CMATH_SPEC(SH_OP_ACOS);
SHCCTO_OP_CMATH_SPEC(SH_OP_ASIN);

SHCCTO_UNARY_OP(SH_OP_ASN, (*A));

SHCCTO_OP_CMATH_SPEC(SH_OP_ATAN);
SHCCTO_OP_CMATH_SPEC(SH_OP_CBRT);
SHCCTO_OP_CMATH_SPEC(SH_OP_CEIL);

template<ShValueType V>
struct ShConcreteCTypeOp<SH_OP_CMUL, V>
{ 
  typedef ShDataVariant<V, SH_HOST> Variant; 
  typedef ShPointer<Variant> DataPtr; 
  typedef ShPointer<const Variant> DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::accumulate(a->begin(), a->end(), 
                     ShDataTypeInfo<V, SH_HOST>::Zero, 
                     std::multiplies<typename Variant::DataType>());
  }
};

SHCCTO_OP_CMATH_SPEC(SH_OP_COS);

template<ShValueType V>
struct ShConcreteCTypeOp<SH_OP_CSUM, V>
{ 
  typedef ShDataVariant<V, SH_HOST> Variant; 
  typedef ShPointer<Variant> DataPtr; 
  typedef ShPointer<const Variant> DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::accumulate(a->begin(), a->end(), 
                     ShDataTypeInfo<V, SH_HOST>::Zero, 
                     std::plus<typename Variant::DataType>()); 
  }
};

SHCCTO_OP_CMATH_SPEC(SH_OP_EXP);
SHCCTO_OP_CMATH_SPEC(SH_OP_EXP2);
SHCCTO_OP_CMATH_SPEC(SH_OP_EXP10);
SHCCTO_OP_CMATH_SPEC(SH_OP_FLR);
SHCCTO_OP_CMATH_SPEC(SH_OP_FRAC);
SHCCTO_OP_CMATH_SPEC(SH_OP_LOG);
SHCCTO_OP_CMATH_SPEC(SH_OP_LOG2);
SHCCTO_OP_CMATH_SPEC(SH_OP_LOG10);
SHCCTO_OP_CMATH_SPEC(SH_OP_NORM);

SHCCTO_UNARY_OP(SH_OP_RCP, 1 / (*A));

SHCCTO_OP_CMATH_SPEC(SH_OP_RND);
SHCCTO_OP_CMATH_SPEC(SH_OP_RSQ);
SHCCTO_OP_CMATH_SPEC(SH_OP_SIN);

SHCCTO_UNARY_OP(SH_OP_SGN, (*A) > 0 ? 1 : (*A) < 0 ? -1 : 0); 

SHCCTO_OP_CMATH_SPEC(SH_OP_SQRT);
SHCCTO_OP_CMATH_SPEC(SH_OP_TAN);


// Binary ops
SHCCTO_BINARY_OP(SH_OP_ADD, (*A) + (*B));

SHCCTO_OP_CMATH_SPEC(SH_OP_ATAN2);

SHCCTO_BINARY_OP(SH_OP_DIV, (*A) / (*B));

template<ShValueType V>
struct ShConcreteCTypeOp<SH_OP_DOT, V>
{ 
  typedef ShDataVariant<V, SH_HOST> Variant; 
  typedef ShPointer<Variant> DataPtr; 
  typedef ShPointer<const Variant> DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::inner_product(a->begin(), a->end(), b->begin(), 
                      ShDataTypeInfo<V, SH_HOST>::Zero);
  }
};

SHCCTO_BINARY_OP(SH_OP_MAX, std::max((*A), (*B))); 
SHCCTO_BINARY_OP(SH_OP_MIN, std::min((*A), (*B))); 

SHCCTO_BINARY_OP(SH_OP_MOD, (*A) % (*B)); 
SHCCTO_OP_CMATH_SPEC(SH_OP_MOD);

SHCCTO_BINARY_OP(SH_OP_MUL, (*A) * (*B));

// TODO implemeng power for win32
SHCCTO_BINARY_OP(SH_OP_POW, __gnu_cxx::power((*A), (*B)));  // only works for integer B
SHCCTO_OP_CMATH_SPEC(SH_OP_POW);


SHCCTO_BINARY_OP(SH_OP_SEQ, (shDataTypeCond<V, SH_HOST>((*A) == (*B))));
SHCCTO_BINARY_OP(SH_OP_SGE, (shDataTypeCond<V, SH_HOST>((*A) >= (*B))));
SHCCTO_BINARY_OP(SH_OP_SGT, (shDataTypeCond<V, SH_HOST>((*A) > (*B))));
SHCCTO_BINARY_OP(SH_OP_SLE, (shDataTypeCond<V, SH_HOST>((*A) <= (*B))));
SHCCTO_BINARY_OP(SH_OP_SLT, (shDataTypeCond<V, SH_HOST>((*A) < (*B))));
SHCCTO_BINARY_OP(SH_OP_SNE, (shDataTypeCond<V, SH_HOST>((*A) != (*B))));

template<ShValueType V>
struct ShConcreteCTypeOp<SH_OP_XPD, V>
{ 
  typedef ShDataVariant<V, SH_HOST> Variant; 
  typedef ShPointer<Variant> DataPtr; 
  typedef ShPointer<const Variant> DataCPtr; 

  static void doop(DataPtr dest, DataCPtr a, DataCPtr b = 0, DataCPtr c = 0) 
  {
    // handle case where dest = a and both not swizzed/negged
    // or similar with dest = b.
    typename Variant::DataType t0, t1;
    t0 = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
    t1 = -((*a)[0] * (*b)[2] - (*a)[2] * (*b)[0]);
    (*dest)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];
    (*dest)[0] = t0;
    (*dest)[1] = t1;
  }
};

// Ternary Ops
SHCCTO_TERNARY_OP(SH_OP_COND, ((*A) > 0 ? (*B) : (*C))); 
SHCCTO_TERNARY_OP(SH_OP_LRP, (*A) * (*B) + (1 - (*A)) * (*C)); 
SHCCTO_TERNARY_OP(SH_OP_MAD, (*A) * (*B) + (*C)); 


}

#endif
