#ifndef SHEVALIMPL_HPP 
#define SHEVALIMPL_HPP

#include <numeric>
#include "ShEval.hpp"
#include "ShVariant.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"

namespace SH {

template<ShOperation OP, ShValueType V>
void ShRegularOp<OP, V>::operator()( 
    ShVariant* dest, const ShVariant* a, const ShVariant* b, const ShVariant* c) const
{
  ShDataVariant<V, SH_HOST>* destVec;
  const ShDataVariant<V, SH_HOST>  *aVec, *bVec, *cVec;

  SH_DEBUG_ASSERT(dest && a);
  destVec = variant_cast<V, SH_HOST>(dest);
  aVec = variant_cast<V, SH_HOST>(a);

  if(b) bVec = variant_cast<V, SH_HOST>(b);
  else bVec = 0;

  if(c) cVec = variant_cast<V, SH_HOST>(c);
  else cVec = 0;

  ShRegularOpChooser<OP, V>::Op::doop(destVec, aVec, bVec, cVec);
}

template<ShOperation OP, ShValueType V1, ShValueType V2>
void ShIntervalOp<OP, V1, V2>::operator()( 
    ShVariant* dest, const ShVariant* a, const ShVariant* b, const ShVariant* c) const
{

  SH_DEBUG_ASSERT(dest && a);

  ShDataVariant<V1, SH_HOST>* destVec = variant_cast<V1, SH_HOST>(dest);

  const ShDataVariant<V2, SH_HOST>* aVec = variant_cast<V2, SH_HOST>(a);

  ShConcreteIntervalOp<OP, V1, V2>::doop(*destVec, *aVec);
}

template<ShValueType V>
void _shInitFloatOps() {
  ShEval* eval = ShEval::instance();

  eval->addOp(SH_OP_ABS, new ShRegularOp<SH_OP_ABS, V>(), V, V);
  eval->addOp(SH_OP_ACOS, new ShRegularOp<SH_OP_ACOS, V>(), V, V);
  eval->addOp(SH_OP_ASIN, new ShRegularOp<SH_OP_ASIN, V>(), V, V);
  eval->addOp(SH_OP_ASN, new ShRegularOp<SH_OP_ASN, V>(), V, V);
  eval->addOp(SH_OP_ATAN, new ShRegularOp<SH_OP_ATAN, V>(), V, V);
  eval->addOp(SH_OP_CBRT, new ShRegularOp<SH_OP_CBRT, V>(), V, V);
  eval->addOp(SH_OP_CEIL, new ShRegularOp<SH_OP_CEIL, V>(), V, V);
  eval->addOp(SH_OP_COS, new ShRegularOp<SH_OP_COS, V>(), V, V);
  eval->addOp(SH_OP_CSUM, new ShRegularOp<SH_OP_CSUM, V>(), V, V);
  eval->addOp(SH_OP_CMUL, new ShRegularOp<SH_OP_CMUL, V>(), V, V);
  eval->addOp(SH_OP_EXP, new ShRegularOp<SH_OP_EXP, V>(), V, V);
  eval->addOp(SH_OP_EXP2, new ShRegularOp<SH_OP_EXP2, V>(), V, V);
  eval->addOp(SH_OP_EXP10, new ShRegularOp<SH_OP_EXP10, V>(), V, V);
  eval->addOp(SH_OP_FLR, new ShRegularOp<SH_OP_FLR, V>(), V, V);
  eval->addOp(SH_OP_FRAC, new ShRegularOp<SH_OP_FRAC, V>(), V, V);
  eval->addOp(SH_OP_LOG, new ShRegularOp<SH_OP_LOG, V>(), V, V);
  eval->addOp(SH_OP_LOG2, new ShRegularOp<SH_OP_LOG2, V>(), V, V);
  eval->addOp(SH_OP_LOG10, new ShRegularOp<SH_OP_LOG10, V>(), V, V);
  //eval->addOp(SH_OP_NEG, new ShRegularOp<SH_OP_NEG, V>());
  eval->addOp(SH_OP_NORM, new ShRegularOp<SH_OP_NORM, V>(), V, V);
  eval->addOp(SH_OP_RCP, new ShRegularOp<SH_OP_RCP, V>(), V, V);
  eval->addOp(SH_OP_RND, new ShRegularOp<SH_OP_RND, V>(), V, V);
  eval->addOp(SH_OP_RSQ, new ShRegularOp<SH_OP_RSQ, V>(), V, V);
  eval->addOp(SH_OP_SIN, new ShRegularOp<SH_OP_SIN, V>(), V, V);
  eval->addOp(SH_OP_SGN, new ShRegularOp<SH_OP_SGN, V>(), V, V);
  eval->addOp(SH_OP_SQRT, new ShRegularOp<SH_OP_SQRT, V>(), V, V);
  eval->addOp(SH_OP_TAN, new ShRegularOp<SH_OP_TAN, V>(), V, V);

  eval->addOp(SH_OP_ADD, new ShRegularOp<SH_OP_ADD, V>(), V, V, V);
  eval->addOp(SH_OP_ATAN2, new ShRegularOp<SH_OP_ATAN2, V>(), V, V, V);
  eval->addOp(SH_OP_DIV, new ShRegularOp<SH_OP_DIV, V>(), V, V, V);
  eval->addOp(SH_OP_DOT, new ShRegularOp<SH_OP_DOT, V>(), V, V, V);
  eval->addOp(SH_OP_MAX, new ShRegularOp<SH_OP_MAX, V>(), V, V, V);
  eval->addOp(SH_OP_MIN, new ShRegularOp<SH_OP_MIN, V>(), V, V, V);
  eval->addOp(SH_OP_MOD, new ShRegularOp<SH_OP_MOD, V>(), V, V, V);
  eval->addOp(SH_OP_MUL, new ShRegularOp<SH_OP_MUL, V>(), V, V, V);
  eval->addOp(SH_OP_POW, new ShRegularOp<SH_OP_POW, V>(), V, V, V);
  eval->addOp(SH_OP_SEQ, new ShRegularOp<SH_OP_SEQ, V>(), V, V, V);
  eval->addOp(SH_OP_SGE, new ShRegularOp<SH_OP_SGE, V>(), V, V, V);
  eval->addOp(SH_OP_SGT, new ShRegularOp<SH_OP_SGT, V>(), V, V, V);
  eval->addOp(SH_OP_SLE, new ShRegularOp<SH_OP_SLE, V>(), V, V, V);
  eval->addOp(SH_OP_SLT, new ShRegularOp<SH_OP_SLT, V>(), V, V, V);
  eval->addOp(SH_OP_SNE, new ShRegularOp<SH_OP_SNE, V>(), V, V, V);
  eval->addOp(SH_OP_XPD, new ShRegularOp<SH_OP_XPD, V>(), V, V, V);

  eval->addOp(SH_OP_LRP, new ShRegularOp<SH_OP_LRP, V>(), V, V, V, V);
  eval->addOp(SH_OP_MAD, new ShRegularOp<SH_OP_MAD, V>(), V, V, V, V);
  eval->addOp(SH_OP_COND, new ShRegularOp<SH_OP_COND, V>(), V, V, V, V);
}

template<ShValueType V>
void _shInitIntOps() {
  ShEval* eval = ShEval::instance();

  // guaranteed int result ops (ignore overflow)
  eval->addOp(SH_OP_ABS, new ShRegularOp<SH_OP_ABS, V>(), V, V);
  eval->addOp(SH_OP_ASN, new ShRegularOp<SH_OP_ASN, V>(), V, V);
  //eval->addOp(SH_OP_NEG, new ShRegularOp<SH_OP_NEG, V>());
  eval->addOp(SH_OP_CSUM, new ShRegularOp<SH_OP_CSUM, V>(), V, V);
  eval->addOp(SH_OP_CMUL, new ShRegularOp<SH_OP_CMUL, V>(), V, V);
  eval->addOp(SH_OP_SGN, new ShRegularOp<SH_OP_SGN, V>(), V, V);

  eval->addOp(SH_OP_ADD, new ShRegularOp<SH_OP_ADD, V>(), V, V, V);
  eval->addOp(SH_OP_DIV, new ShRegularOp<SH_OP_DIV, V>(), V, V, V);
  eval->addOp(SH_OP_DOT, new ShRegularOp<SH_OP_DOT, V>(), V, V, V);
  eval->addOp(SH_OP_MAX, new ShRegularOp<SH_OP_MAX, V>(), V, V, V);
  eval->addOp(SH_OP_MIN, new ShRegularOp<SH_OP_MIN, V>(), V, V, V);
  eval->addOp(SH_OP_MOD, new ShRegularOp<SH_OP_MOD, V>(), V, V, V);
  eval->addOp(SH_OP_MUL, new ShRegularOp<SH_OP_MUL, V>(), V, V, V);
  eval->addOp(SH_OP_POW, new ShRegularOp<SH_OP_POW, V>(), V, V, V);
  eval->addOp(SH_OP_SEQ, new ShRegularOp<SH_OP_SEQ, V>(), V, V, V);
  eval->addOp(SH_OP_SGE, new ShRegularOp<SH_OP_SGE, V>(), V, V, V);
  eval->addOp(SH_OP_SGT, new ShRegularOp<SH_OP_SGT, V>(), V, V, V);
  eval->addOp(SH_OP_SLE, new ShRegularOp<SH_OP_SLE, V>(), V, V, V);
  eval->addOp(SH_OP_SLT, new ShRegularOp<SH_OP_SLT, V>(), V, V, V);
  eval->addOp(SH_OP_SNE, new ShRegularOp<SH_OP_SNE, V>(), V, V, V);
  eval->addOp(SH_OP_XPD, new ShRegularOp<SH_OP_XPD, V>(), V, V, V);

  eval->addOp(SH_OP_LRP, new ShRegularOp<SH_OP_LRP, V>(), V, V, V, V);
  eval->addOp(SH_OP_MAD, new ShRegularOp<SH_OP_MAD, V>(), V, V, V, V);
  eval->addOp(SH_OP_COND, new ShRegularOp<SH_OP_COND, V>(), V, V, V, V);

  // unary ASN equivalents for ints 
  eval->addOp(SH_OP_CEIL, new ShRegularOp<SH_OP_ASN, V>(), V, V);
  eval->addOp(SH_OP_FLR, new ShRegularOp<SH_OP_ASN, V>(), V, V);
  eval->addOp(SH_OP_FRAC, new ShRegularOp<SH_OP_ASN, V>(), V, V);
  eval->addOp(SH_OP_RND, new ShRegularOp<SH_OP_ASN, V>(), V, V);

  // all other ops must use float (since they can potentially give float
  // results)
}

template<ShValueType V, ShValueType IntervalV>
void _shInitIntervalOps() {
  ShEval* eval = ShEval::instance();

  eval->addOp(SH_OP_LO, new ShIntervalOp<SH_OP_LO, V, IntervalV>(), 
      V, IntervalV);
  eval->addOp(SH_OP_HI, new ShIntervalOp<SH_OP_HI, V, IntervalV>(), 
      V, IntervalV);

  eval->addOp(SH_OP_SETLO, new ShIntervalOp<SH_OP_SETLO, IntervalV, V>(), 
      IntervalV, V);
  eval->addOp(SH_OP_SETHI, new ShIntervalOp<SH_OP_SETHI, IntervalV, V>(), 
      IntervalV, V);
}

}

#endif
