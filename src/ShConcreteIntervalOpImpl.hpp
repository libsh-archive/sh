#ifndef SHCONCRETEINTERVALOPIMPL_HPP 
#define SHCONCRETEINTERVALOPIMPL_HPP

#include <numeric>
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"

// TODO replace zeros (SH_OP_DOT) with ShConcreteTypeInfo<V>::ZERO
namespace SH {

/* Partial specialization for different operations */ 

template<ShValueType V1, ShValueType V2>
struct ShConcreteIntervalOp<SH_OP_LO, V1, V2> {
  static void doop(ShDataVariant<V1, SH_HOST> &dest, 
    const ShDataVariant<V2, SH_HOST> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename ShDataVariant<V1, SH_HOST>::iterator D = dest.begin();
    typename ShDataVariant<V2, SH_HOST>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) (*D) = A->lo(); 
  }
};

template<ShValueType V1, ShValueType V2>
struct ShConcreteIntervalOp<SH_OP_HI, V1, V2> {
  static void doop(ShDataVariant<V1, SH_HOST> &dest, 
    const ShDataVariant<V2, SH_HOST> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename ShDataVariant<V1, SH_HOST>::iterator D = dest.begin();
    typename ShDataVariant<V2, SH_HOST>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) (*D) = A->hi(); 
  }
};


template<ShValueType V1, ShValueType V2>
struct ShConcreteIntervalOp<SH_OP_SETLO, V1, V2> {
  static void doop(ShDataVariant<V1, SH_HOST> &dest, 
    const ShDataVariant<V2, SH_HOST> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename ShDataVariant<V1, SH_HOST>::iterator D = dest.begin();
    typename ShDataVariant<V2, SH_HOST>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) D->lo() = (*A); 
  }
};

template<ShValueType V1, ShValueType V2>
struct ShConcreteIntervalOp<SH_OP_SETHI, V1, V2> {
  static void doop(ShDataVariant<V1, SH_HOST> &dest, 
    const ShDataVariant<V2, SH_HOST> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename ShDataVariant<V1, SH_HOST>::iterator D = dest.begin();
    typename ShDataVariant<V2, SH_HOST>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) D->hi() = (*A); 
  }
};

}

#endif
