#ifndef SHCONCRETEREGULAROPIMPL_HPP 
#define SHCONCRETEREGULAROPIMPL_HPP
#include <numeric>
#include "ShEval.hpp"
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
//
#define SHCRO_UNARY_OP(op, opsrc)\
template<typename T>\
struct ShConcreteRegularOp<op, T> {\
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

#define SHCRO_BINARY_OP(op, opsrc)\
template<typename T>\
struct ShConcreteRegularOp<op, T> {\
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

#define SHCRO_TERNARY_OP(op, opsrc)\
template<typename T>\
struct ShConcreteRegularOp<op, T> {\
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

// Unary ops
SHCRO_UNARY_OP(SH_OP_ABS, abs(*A));
SHCRO_UNARY_OP(SH_OP_ACOS, acos(*A));
SHCRO_UNARY_OP(SH_OP_ASIN, asin(*A));
SHCRO_UNARY_OP(SH_OP_ASN, (*A));
SHCRO_UNARY_OP(SH_OP_ATAN, atan(*A));
SHCRO_UNARY_OP(SH_OP_CBRT, cbrt(*A));
SHCRO_UNARY_OP(SH_OP_CEIL, ceil(*A));

template<typename T>
struct ShConcreteRegularOp<SH_OP_CMUL, T> {
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

SHCRO_UNARY_OP(SH_OP_COS, cos(*A));

template<typename T>
struct ShConcreteRegularOp<SH_OP_CSUM, T> {
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

SHCRO_UNARY_OP(SH_OP_EXP, exp(*A));
SHCRO_UNARY_OP(SH_OP_EXP2, exp2(*A));
SHCRO_UNARY_OP(SH_OP_EXP10, exp10(*A));
SHCRO_UNARY_OP(SH_OP_FLR, floor(*A));
SHCRO_UNARY_OP(SH_OP_FRAC, frac(*A));
SHCRO_UNARY_OP(SH_OP_LOG, log(*A));
SHCRO_UNARY_OP(SH_OP_LOG2, log(*A));
SHCRO_UNARY_OP(SH_OP_LOG10, log10(*A));

template<typename T>
struct ShConcreteRegularOp<SH_OP_NORM, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    SH_DEBUG_ASSERT(dest && a);
    T m = sqrt(std::inner_product(a->begin(), a->end(), a->begin(), (T) 0));
  
    typename std::vector<T>::iterator D = dest->begin();
    typename std::vector<T>::const_iterator A = a->begin();
    for(; D != dest->end(); ++A, ++D) (*D) = (*A) / m;
  }
};

SHCRO_UNARY_OP(SH_OP_RCP, rcp(*A));
SHCRO_UNARY_OP(SH_OP_RND, rnd(*A));
SHCRO_UNARY_OP(SH_OP_RSQ, rsq(*A));
SHCRO_UNARY_OP(SH_OP_SIN, sin(*A));
SHCRO_UNARY_OP(SH_OP_SGN, sgn(*A)); 
SHCRO_UNARY_OP(SH_OP_SQRT, sqrt(*A)); 
SHCRO_UNARY_OP(SH_OP_TAN, tan(*A)); 

// Binary ops
SHCRO_BINARY_OP(SH_OP_ADD, (*A) + (*B));
SHCRO_BINARY_OP(SH_OP_ATAN2, atan2((*A), (*B)));
SHCRO_BINARY_OP(SH_OP_DIV, (*A) / (*B));
SHCRO_BINARY_OP(SH_OP_MAX, max((*A), (*B))); 
SHCRO_BINARY_OP(SH_OP_MIN, min((*A), (*B))); 
SHCRO_BINARY_OP(SH_OP_MOD, (*A) % (*B)); 
SHCRO_BINARY_OP(SH_OP_MUL, (*A) * (*B));
SHCRO_BINARY_OP(SH_OP_POW, pow((*A), (*B)));

template<typename T>
struct ShConcreteRegularOp<SH_OP_DOT, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    // dest->size should be 1 and a->size == b->size
    (*dest)[0] = std::inner_product(a->begin(), a->end(), b->begin(), ShConcreteTypeInfo<T>::ZERO);
  }
};

SHCRO_BINARY_OP(SH_OP_SEQ, (*A) == (*B));
SHCRO_BINARY_OP(SH_OP_SGE, (*A) >= (*B));
SHCRO_BINARY_OP(SH_OP_SGT, (*A) > (*B));
SHCRO_BINARY_OP(SH_OP_SLE, (*A) <= (*B));
SHCRO_BINARY_OP(SH_OP_SLT, (*A) < (*B));
SHCRO_BINARY_OP(SH_OP_SNE, (*A) != (*B));

template<typename T>
struct ShConcreteRegularOp<SH_OP_XPD, T> {
  static void doop(std::vector<T> *dest, 
    const std::vector<T> *a, const std::vector<T> *b = 0, const std::vector<T> *c = 0) 
  {
    (*dest)[0] = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
    (*dest)[1] = -((*a)[0] * (*b)[2] - (*a)[2] * (*b)[0]);
    (*dest)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];
  }
};

// Ternary Ops
SHCRO_TERNARY_OP(SH_OP_COND, cond(*A, *B, *C)); 
SHCRO_TERNARY_OP(SH_OP_LRP, lerp(*A, *B, *C));
SHCRO_TERNARY_OP(SH_OP_MAD, (*A) * (*B) + (*C));

}
#endif
