#ifndef SHCONCRETEINTERVALOPIMPL_HPP 
#define SHCONCRETEINTERVALOPIMPL_HPP

#include <numeric>
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"

// TODO replace zeros (SH_OP_DOT) with ShConcreteTypeInfo<T>::ZERO
namespace SH {

/* Partial specialization for different operations */ 

#define SHCIO_UNARY_OP(op, opsrc) \
template<typename T1, typename T2>\
struct ShConcreteRangeOp<op, T1, T2> {\
  static void doop(ShDataVariant<T1, SH_HOST> *dest, \
    const ShDataVariant<T2, SH_HOST> *a,\
    const ShDataVariant<T2, SH_HOST> *b)\
  {\
    typename ShDataVariant<T1, SH_HOST>::iterator D = dest->begin();\
    typename ShDataVariant<T2, SH_HOST>::const_iterator A = a->begin();\
\
    int ao = a->size() > 1; \
    \
    for(;D != dest->end(); A += ao, ++D) {\
      opsrc; \
    }\
  }\
};

#define SHCIO_BINARY_OP(op, opsrc) \
template<typename T1, typename T2>\
struct ShConcreteRangeOp<op, T1, T2> {\
  static void doop(ShDataVariant<T1, SH_HOST> *dest, \
    const ShDataVariant<T2, SH_HOST> *a,\
    const ShDataVariant<T2, SH_HOST> *b)\
  {\
    typename ShDataVariant<T1, SH_HOST>::iterator D = dest->begin();\
    typename ShDataVariant<T2, SH_HOST>::const_iterator A = a->begin();\
    typename ShDataVariant<T2, SH_HOST>::const_iterator B = b->begin();\
\
    int ao = a->size() > 1; \
    int bo = b->size() > 1; \
    \
    for(;D != dest->end(); A += ao, B += bo, ++D) {\
      opsrc; \
    }\
  }\
};

SHCIO_UNARY_OP(SH_OP_LO, (*D) = A->lo());
SHCIO_UNARY_OP(SH_OP_HI, (*D) = A->hi());
SHCIO_UNARY_OP(SH_OP_WIDTH, (*D) = A->width());
SHCIO_UNARY_OP(SH_OP_RADIUS, (*D) = A->radius());
SHCIO_UNARY_OP(SH_OP_CENTER, (*D) = A->center());

SHCIO_BINARY_OP(SH_OP_IVAL, ((*D).lo() = (*A), (*D).hi() = (*B)));
SHCIO_BINARY_OP(SH_OP_UNION, (*D) = range_union(*A, *B));
SHCIO_BINARY_OP(SH_OP_ISCT, (*D) = range_isct(*A, *B));
SHCIO_BINARY_OP(SH_OP_CONTAINS, (*D) = range_contains(*A, *B));

}

#endif
