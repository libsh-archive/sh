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

template<typename T1, typename T2>
struct ShConcreteIntervalOp<SH_OP_LO, T1, T2> {
  static void doop(std::vector<T1> &dest, 
    const std::vector<T2> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename std::vector<T1>::iterator D = dest.begin();
    typename std::vector<T2>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) (*D) = A->lo(); 
  }
};

template<typename T1, typename T2>
struct ShConcreteIntervalOp<SH_OP_HI, T1, T2> {
  static void doop(std::vector<T1> &dest, 
    const std::vector<T2> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename std::vector<T1>::iterator D = dest.begin();
    typename std::vector<T2>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) (*D) = A->hi(); 
  }
};


template<typename T1, typename T2>
struct ShConcreteIntervalOp<SH_OP_SETLO, T1, T2> {
  static void doop(std::vector<T1> &dest, 
    const std::vector<T2> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename std::vector<T1>::iterator D = dest.begin();
    typename std::vector<T2>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) D->lo() = (*A); 
  }
};

template<typename T1, typename T2>
struct ShConcreteIntervalOp<SH_OP_SETHI, T1, T2> {
  static void doop(std::vector<T1> &dest, 
    const std::vector<T2> &a)
  {
    SH_DEBUG_ASSERT(dest.size() == a.size());
    typename std::vector<T1>::iterator D = dest.begin();
    typename std::vector<T2>::const_iterator A = a.begin();
    
    for(;A != a.end(); ++A, ++D) D->hi() = (*A); 
  }
};

}

#endif
