#ifndef SHEVALIMPL_HPP 
#define SHEVALIMPL_HPP

#include <numeric>
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"
#include "ShTypeInfo.hpp"

namespace SH {

template<ShOperation OP, typename T>
void ShRegularOp<OP, T>::operator()( 
    ShVariantPtr dest, ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const
{
  typename std::vector<T> *destVec;
  const typename std::vector<T> *aVec, *bVec, *cVec;

  SH_DEBUG_ASSERT(dest && a);
  destVec = &shref_dynamic_cast<ShDataVariant<T> >(dest)->data();
  aVec = &shref_dynamic_cast<const ShDataVariant<T> >(a)->data();

  if(b) bVec = &shref_dynamic_cast<const ShDataVariant<T> >(b)->data(); 
  else bVec = 0;

  if(c) cVec = &shref_dynamic_cast<const ShDataVariant<T> >(c)->data();  
  else cVec = 0;

  ShRegularOpChooser<OP, T>::Op::doop(destVec, aVec, bVec, cVec);
}

template<ShOperation OP, typename T1, typename T2>
void ShIntervalOp<OP, T1, T2>::operator()( 
    ShVariantPtr dest, ShVariantCPtr a, ShVariantCPtr b, ShVariantCPtr c) const
{

  SH_DEBUG_ASSERT(dest && a);

  typename std::vector<T1> &destVec = 
    shref_dynamic_cast<ShDataVariant<T1> >(dest)->data();

  const typename std::vector<T2> &aVec = 
    shref_dynamic_cast<const ShDataVariant<T2> >(a)->data();

  ShConcreteIntervalOp<OP, T1, T2>::doop(destVec, aVec);
}

template<typename T>
void _shInitFloatOps() {
  int typeIndex = shTypeIndex<T>();
  ShEval* eval = ShEval::instance();

  eval->addOp(SH_OP_ABS, new ShRegularOp<SH_OP_ABS, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_ACOS, new ShRegularOp<SH_OP_ACOS, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_ASIN, new ShRegularOp<SH_OP_ASIN, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_ASN, new ShRegularOp<SH_OP_ASN, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_ATAN, new ShRegularOp<SH_OP_ATAN, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_CBRT, new ShRegularOp<SH_OP_CBRT, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_CEIL, new ShRegularOp<SH_OP_CEIL, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_COS, new ShRegularOp<SH_OP_COS, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_CSUM, new ShRegularOp<SH_OP_CSUM, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_CMUL, new ShRegularOp<SH_OP_CMUL, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_EXP, new ShRegularOp<SH_OP_EXP, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_EXP2, new ShRegularOp<SH_OP_EXP2, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_EXP10, new ShRegularOp<SH_OP_EXP10, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_FLR, new ShRegularOp<SH_OP_FLR, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_FRAC, new ShRegularOp<SH_OP_FRAC, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_LOG, new ShRegularOp<SH_OP_LOG, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_LOG2, new ShRegularOp<SH_OP_LOG2, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_LOG10, new ShRegularOp<SH_OP_LOG10, T>(), typeIndex, typeIndex);
  //eval->addOp(SH_OP_NEG, new ShRegularOp<SH_OP_NEG, T>());
  eval->addOp(SH_OP_NORM, new ShRegularOp<SH_OP_NORM, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_RCP, new ShRegularOp<SH_OP_RCP, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_RND, new ShRegularOp<SH_OP_RND, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_RSQ, new ShRegularOp<SH_OP_RSQ, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_SIN, new ShRegularOp<SH_OP_SIN, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_SGN, new ShRegularOp<SH_OP_SGN, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_SQRT, new ShRegularOp<SH_OP_SQRT, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_TAN, new ShRegularOp<SH_OP_TAN, T>(), typeIndex, typeIndex);

  eval->addOp(SH_OP_ADD, new ShRegularOp<SH_OP_ADD, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_ATAN2, new ShRegularOp<SH_OP_ATAN2, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_DIV, new ShRegularOp<SH_OP_DIV, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_DOT, new ShRegularOp<SH_OP_DOT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MAX, new ShRegularOp<SH_OP_MAX, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MIN, new ShRegularOp<SH_OP_MIN, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MOD, new ShRegularOp<SH_OP_MOD, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MUL, new ShRegularOp<SH_OP_MUL, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_POW, new ShRegularOp<SH_OP_POW, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SEQ, new ShRegularOp<SH_OP_SEQ, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SGE, new ShRegularOp<SH_OP_SGE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SGT, new ShRegularOp<SH_OP_SGT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SLE, new ShRegularOp<SH_OP_SLE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SLT, new ShRegularOp<SH_OP_SLT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SNE, new ShRegularOp<SH_OP_SNE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_XPD, new ShRegularOp<SH_OP_XPD, T>(), typeIndex, typeIndex, typeIndex);

  eval->addOp(SH_OP_LRP, new ShRegularOp<SH_OP_LRP, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MAD, new ShRegularOp<SH_OP_MAD, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_COND, new ShRegularOp<SH_OP_COND, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
}

template<typename T>
void _shInitIntOps() {
  int typeIndex = shTypeIndex<T>();
  ShEval* eval = ShEval::instance();

  // @todo type add some int ops even if they don't normally make sense
  // (e.g. no-ops like FLR, CEIL, etc.)
  // because otherwise
  eval->addOp(SH_OP_ABS, new ShRegularOp<SH_OP_ABS, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_ASN, new ShRegularOp<SH_OP_ASN, T>(), typeIndex, typeIndex);
  //eval->addOp(SH_OP_NEG, new ShRegularOp<SH_OP_NEG, T>());
  eval->addOp(SH_OP_CSUM, new ShRegularOp<SH_OP_CSUM, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_CMUL, new ShRegularOp<SH_OP_CMUL, T>(), typeIndex, typeIndex);
  eval->addOp(SH_OP_SGN, new ShRegularOp<SH_OP_SGN, T>(), typeIndex, typeIndex);

  eval->addOp(SH_OP_ADD, new ShRegularOp<SH_OP_ADD, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_DIV, new ShRegularOp<SH_OP_DIV, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_DOT, new ShRegularOp<SH_OP_DOT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MAX, new ShRegularOp<SH_OP_MAX, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MIN, new ShRegularOp<SH_OP_MIN, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MOD, new ShRegularOp<SH_OP_MOD, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MUL, new ShRegularOp<SH_OP_MUL, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_POW, new ShRegularOp<SH_OP_POW, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SEQ, new ShRegularOp<SH_OP_SEQ, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SGE, new ShRegularOp<SH_OP_SGE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SGT, new ShRegularOp<SH_OP_SGT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SLE, new ShRegularOp<SH_OP_SLE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SLT, new ShRegularOp<SH_OP_SLT, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_SNE, new ShRegularOp<SH_OP_SNE, T>(), typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_XPD, new ShRegularOp<SH_OP_XPD, T>(), typeIndex, typeIndex, typeIndex);

  eval->addOp(SH_OP_LRP, new ShRegularOp<SH_OP_LRP, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_MAD, new ShRegularOp<SH_OP_MAD, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
  eval->addOp(SH_OP_COND, new ShRegularOp<SH_OP_COND, T>(), typeIndex, typeIndex, typeIndex, typeIndex);
}

template<typename T>
void _shInitIntervalOps() {
  int typeIndex = shTypeIndex<T>();
  int intervalTypeIndex = shTypeIndex<ShInterval<T> >();

  ShEval* eval = ShEval::instance();

  eval->addOp(SH_OP_LO, new ShIntervalOp<SH_OP_LO, T, ShInterval<T> >(), 
      typeIndex, intervalTypeIndex);
  eval->addOp(SH_OP_HI, new ShIntervalOp<SH_OP_HI, T, ShInterval<T> >(), 
      typeIndex, intervalTypeIndex);

  eval->addOp(SH_OP_SETLO, new ShIntervalOp<SH_OP_SETLO, ShInterval<T>, T>(), 
      intervalTypeIndex, typeIndex);
  eval->addOp(SH_OP_SETHI, new ShIntervalOp<SH_OP_SETHI, ShInterval<T>, T>(), 
      intervalTypeIndex, typeIndex);
}

}

#endif
