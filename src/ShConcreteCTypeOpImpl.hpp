#ifndef SHCONCRETECTYPEOPIMPL_HPP 
#define SHCONCRETECTYPEOPIMPL_HPP

#include <numeric>
#include <ext/numeric> // TODO implement clone of std::power(T, int) for non-GNU libstdc++ compatibility (using the "Russian peasant algorithm")
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"

// TODO replace zeros (SH_OP_DOT) with ShConcreteTypeInfo<T>::ZERO
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
template<typename T>\
struct ShConcreteCTypeOp<op, T> {\
  static void doop(std::vector<T> *dest, \
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a);\
    int ao = a->size() > 1;\
  \
    typename std::vector<T>::iterator D = dest->begin();\
    typename std::vector<T>::const_iterator A = a->begin();\
    for(; D != dest->end(); A += ao, ++D) {\
      (*D) = opsrc;\
    }\
  }\
};


#define SHCCTO_BINARY_OP(op, opsrc)\
template<typename T>\
struct ShConcreteCTypeOp<op, T> {\
  static void doop(std::vector<T> *dest, \
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a && b);\
    int ao = a->size() > 1;\
    int bo = b->size() > 1;\
  \
    typename std::vector<T>::iterator D = dest->begin();\
    typename std::vector<T>::const_iterator A, B;\
    A = a->begin();\
    B = b->begin();\
    for(; D != dest->end(); A += ao, B += bo, ++D) {\
      (*D) = opsrc;\
    }\
  }\
};


#define SHCCTO_TERNARY_OP(op, opsrc)\
template<typename T>\
struct ShConcreteCTypeOp<op, T> {\
  static void doop(std::vector<T> *dest, \
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) \
  {\
    SH_DEBUG_ASSERT(dest && a && b && c);\
    int ao = a->size() > 1;\
    int bo = b->size() > 1;\
    int co = c->size() > 1;\
  \
    typename std::vector<T>::iterator D = dest->begin();\
    typename std::vector<T>::const_iterator A, B, C;\
    A = a->begin();\
    B = b->begin();\
    C = c->begin();\
    for(; D != dest->end(); A += ao, B += bo, C += co, ++D) {\
      (*D) = opsrc;\
    }\
  }\
};

#define SHCCTO_OP_SPEC(T, op)\
template<>\
struct ShConcreteCTypeOp<op, T> {\
  static void doop(std::vector<T> *dest, \
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0);\
  \
};

#define SHCCTO_OP_CMATH_SPEC(op)\
  SHCCTO_OP_SPEC(double, op);\
  SHCCTO_OP_SPEC(float, op);

// Note that some ops are currently NOT declared for integer types
// (anything that is only specialized for double/float cmath functions 
// will not work with ints)

// Unary ops
SHCCTO_UNARY_OP(SH_OP_ABS, (*A) > ShConcreteTypeInfo<T>::ZERO ? (*A) : -(*A));

SHCCTO_OP_CMATH_SPEC(SH_OP_ACOS);
SHCCTO_OP_CMATH_SPEC(SH_OP_ASIN);

SHCCTO_UNARY_OP(SH_OP_ASN, (*A));

SHCCTO_OP_CMATH_SPEC(SH_OP_ATAN);
SHCCTO_OP_CMATH_SPEC(SH_OP_CBRT);
SHCCTO_OP_CMATH_SPEC(SH_OP_CEIL);

template<typename T>
struct ShConcreteCTypeOp<SH_OP_CMUL, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    T result = ShConcreteTypeInfo<T>::ZERO;
    typename std::vector<T>::const_iterator A = a->begin();
    for(; A != a->end(); ++A) result *= (*A); 
     (*dest)[0] = result;
  }
};

SHCCTO_OP_CMATH_SPEC(SH_OP_COS);

template<typename T>
struct ShConcreteCTypeOp<SH_OP_CSUM, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    T result = ShConcreteTypeInfo<T>::ZERO;
    typename std::vector<T>::const_iterator A = a->begin();
    for(; A != a->end(); ++A) result += (*A); 
     (*dest)[0] = result;
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

template<typename T>
struct ShConcreteCTypeOp<SH_OP_DOT, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::inner_product(a->begin(), a->end(), b->begin(), (T) 0);
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


SHCCTO_BINARY_OP(SH_OP_SEQ, shTypeInfoCond<T>((*A) == (*B)));
SHCCTO_BINARY_OP(SH_OP_SGE, shTypeInfoCond<T>((*A) >= (*B)));
SHCCTO_BINARY_OP(SH_OP_SGT, shTypeInfoCond<T>((*A) > (*B)));
SHCCTO_BINARY_OP(SH_OP_SLE, shTypeInfoCond<T>((*A) <= (*B)));
SHCCTO_BINARY_OP(SH_OP_SLT, shTypeInfoCond<T>((*A) < (*B)));
SHCCTO_BINARY_OP(SH_OP_SNE, shTypeInfoCond<T>((*A) != (*B)));

template<typename T>
struct ShConcreteCTypeOp<SH_OP_XPD, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    (*dest)[0] = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
    (*dest)[1] = -((*a)[0] * (*b)[2] - (*a)[2] * (*b)[0]);
    (*dest)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];
  }
};

// Ternary Ops
SHCCTO_TERNARY_OP(SH_OP_COND, ((*A) > 0 ? (*B) : (*C))); 
SHCCTO_TERNARY_OP(SH_OP_LRP, (*A) * (*B) + (1 - (*A)) * (*C)); 
SHCCTO_TERNARY_OP(SH_OP_MAD, (*A) * (*B) + (*C)); 


}

#endif
