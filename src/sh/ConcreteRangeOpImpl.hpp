#ifndef CONCRETEINTERVALOPIMPL_HPP 
#define CONCRETEINTERVALOPIMPL_HPP

#include <numeric>
#include "Eval.hpp"
#include "Variant.hpp"
#include "Debug.hpp"
#include "Error.hpp"
#include "TypeInfo.hpp"

// TODO replace zeros (OP_DOT) with ConcreteTypeInfo<T>::ZERO
namespace SH {

/* Partial specialization for different operations */ 

#define CIO_UNARY_OP(op, opsrc) \
template<typename T1, typename T2>\
struct ConcreteRangeOp<op, T1, T2> {\
  static void doop(DataVariant<T1, HOST> *dest, \
    const DataVariant<T2, HOST> *a,\
    const DataVariant<T2, HOST> *b)\
  {\
    typename DataVariant<T1, HOST>::iterator D = dest->begin();\
    typename DataVariant<T2, HOST>::const_iterator A = a->begin();\
\
    int ao = a->size() > 1; \
    \
    for(;D != dest->end(); A += ao, ++D) {\
      opsrc; \
    }\
  }\
};

#define CIO_BINARY_OP(op, opsrc) \
template<typename T1, typename T2>\
struct ConcreteRangeOp<op, T1, T2> {\
  static void doop(DataVariant<T1, HOST> *dest, \
    const DataVariant<T2, HOST> *a,\
    const DataVariant<T2, HOST> *b)\
  {\
    typename DataVariant<T1, HOST>::iterator D = dest->begin();\
    typename DataVariant<T2, HOST>::const_iterator A = a->begin();\
    typename DataVariant<T2, HOST>::const_iterator B = b->begin();\
\
    int ao = a->size() > 1; \
    int bo = b->size() > 1; \
    \
    for(;D != dest->end(); A += ao, B += bo, ++D) {\
      opsrc; \
    }\
  }\
};

CIO_UNARY_OP(OP_LO, (*D) = A->lo());
CIO_UNARY_OP(OP_HI, (*D) = A->hi());
CIO_UNARY_OP(OP_WIDTH, (*D) = A->width());
CIO_UNARY_OP(OP_RADIUS, (*D) = A->radius());
CIO_UNARY_OP(OP_CENTER, (*D) = A->center());

CIO_BINARY_OP(OP_IVAL, ((*D).lo() = (*A), (*D).hi() = (*B)));
CIO_BINARY_OP(OP_UNION, (*D) = range_union(*A, *B));
CIO_BINARY_OP(OP_ISCT, (*D) = range_isct(*A, *B));
CIO_BINARY_OP(OP_CONTAINS, (*D) = range_contains(*A, *B));

}

#endif
