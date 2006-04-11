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
#ifndef SHEVALIMPL_HPP 
#define SHEVALIMPL_HPP

#include <numeric>
#include "Eval.hpp"
#include "Variant.hpp"
#include "Debug.hpp"
#include "Error.hpp"

namespace SH {

template<Operation OP, typename T>
void RegularOp<OP, T>::operator()( 
    Variant* dest, const Variant* a, const Variant* b, const Variant* c) const
{
  DataVariant<T, HOST>* destVec;
  const DataVariant<T, HOST>  *aVec, *bVec, *cVec;

  DEBUG_ASSERT(dest && a);
  destVec = variant_cast<T, HOST>(dest);
  aVec = variant_cast<T, HOST>(a);

  if(b) bVec = variant_cast<T, HOST>(b);
  else bVec = 0;

  if(c) cVec = variant_cast<T, HOST>(c);
  else cVec = 0;

  RegularOpChooser<OP, T>::Op::doop(destVec, aVec, bVec, cVec);
}

template<typename T>
void _initFloatOps() {
  Eval* eval = Eval::instance();
  const ValueType V = StorageTypeInfo<T>::value_type;

  eval->addOp(OP_ABS, new RegularOp<OP_ABS, T>(), V, V);
  eval->addOp(OP_ACOS, new RegularOp<OP_ACOS, T>(), V, V);
  eval->addOp(OP_ACOSH, new RegularOp<OP_ACOSH, T>(), V, V);
  eval->addOp(OP_ASIN, new RegularOp<OP_ASIN, T>(), V, V);
  eval->addOp(OP_ASINH, new RegularOp<OP_ASINH, T>(), V, V);
  eval->addOp(OP_ASN, new RegularOp<OP_ASN, T>(), V, V);
  eval->addOp(OP_ATAN, new RegularOp<OP_ATAN, T>(), V, V);
  eval->addOp(OP_ATANH, new RegularOp<OP_ATANH, T>(), V, V);
  eval->addOp(OP_CBRT, new RegularOp<OP_CBRT, T>(), V, V);
  eval->addOp(OP_CEIL, new RegularOp<OP_CEIL, T>(), V, V);
  eval->addOp(OP_COS, new RegularOp<OP_COS, T>(), V, V);
  eval->addOp(OP_COSH, new RegularOp<OP_COSH, T>(), V, V);
  eval->addOp(OP_CSUM, new RegularOp<OP_CSUM, T>(), V, V);
  eval->addOp(OP_CMUL, new RegularOp<OP_CMUL, T>(), V, V);
  eval->addOp(OP_EXP, new RegularOp<OP_EXP, T>(), V, V);
  eval->addOp(OP_EXP2, new RegularOp<OP_EXP2, T>(), V, V);
  eval->addOp(OP_EXP10, new RegularOp<OP_EXP10, T>(), V, V);
  eval->addOp(OP_FLR, new RegularOp<OP_FLR, T>(), V, V);
  eval->addOp(OP_FRAC, new RegularOp<OP_FRAC, T>(), V, V);
  eval->addOp(OP_HASH, new RegularOp<OP_HASH, T>(), V, V);
  eval->addOp(OP_LOG, new RegularOp<OP_LOG, T>(), V, V);
  eval->addOp(OP_LOG2, new RegularOp<OP_LOG2, T>(), V, V);
  eval->addOp(OP_LOG10, new RegularOp<OP_LOG10, T>(), V, V);
  //eval->addOp(OP_NEG, new RegularOp<OP_NEG, T>());
  eval->addOp(OP_NOISE, new RegularOp<OP_NOISE, T>(), V, V);
  eval->addOp(OP_NORM, new RegularOp<OP_NORM, T>(), V, V);
  eval->addOp(OP_RCP, new RegularOp<OP_RCP, T>(), V, V);
  eval->addOp(OP_RND, new RegularOp<OP_RND, T>(), V, V);
  eval->addOp(OP_RSQ, new RegularOp<OP_RSQ, T>(), V, V);
  eval->addOp(OP_SIN, new RegularOp<OP_SIN, T>(), V, V);
  eval->addOp(OP_SINH, new RegularOp<OP_SINH, T>(), V, V);
  eval->addOp(OP_SGN, new RegularOp<OP_SGN, T>(), V, V);
  eval->addOp(OP_SQRT, new RegularOp<OP_SQRT, T>(), V, V);
  eval->addOp(OP_TAN, new RegularOp<OP_TAN, T>(), V, V);
  eval->addOp(OP_TANH, new RegularOp<OP_TANH, T>(), V, V);

  eval->addOp(OP_ADD, new RegularOp<OP_ADD, T>(), V, V, V);
  eval->addOp(OP_ATAN2, new RegularOp<OP_ATAN2, T>(), V, V, V);
  eval->addOp(OP_DIV, new RegularOp<OP_DIV, T>(), V, V, V);
  eval->addOp(OP_DOT, new RegularOp<OP_DOT, T>(), V, V, V);
  eval->addOp(OP_MAX, new RegularOp<OP_MAX, T>(), V, V, V);
  eval->addOp(OP_MIN, new RegularOp<OP_MIN, T>(), V, V, V);
  eval->addOp(OP_MOD, new RegularOp<OP_MOD, T>(), V, V, V);
  eval->addOp(OP_MUL, new RegularOp<OP_MUL, T>(), V, V, V);
  eval->addOp(OP_POW, new RegularOp<OP_POW, T>(), V, V, V);
  eval->addOp(OP_SEQ, new RegularOp<OP_SEQ, T>(), V, V, V);
  eval->addOp(OP_SGE, new RegularOp<OP_SGE, T>(), V, V, V);
  eval->addOp(OP_SGT, new RegularOp<OP_SGT, T>(), V, V, V);
  eval->addOp(OP_SLE, new RegularOp<OP_SLE, T>(), V, V, V);
  eval->addOp(OP_SLT, new RegularOp<OP_SLT, T>(), V, V, V);
  eval->addOp(OP_SNE, new RegularOp<OP_SNE, T>(), V, V, V);
  eval->addOp(OP_XPD, new RegularOp<OP_XPD, T>(), V, V, V);

  eval->addOp(OP_LIT, new RegularOp<OP_LIT, T>(), V, V);
  eval->addOp(OP_LRP, new RegularOp<OP_LRP, T>(), V, V, V, V);
  eval->addOp(OP_MAD, new RegularOp<OP_MAD, T>(), V, V, V, V);
  eval->addOp(OP_COND, new RegularOp<OP_COND, T>(), V, V, V, V);
}

template<typename T>
void _initIntOps() {
  Eval* eval = Eval::instance();
  const ValueType V = StorageTypeInfo<T>::value_type;

  // guaranteed int result ops (ignore overflow)
  eval->addOp(OP_ABS, new RegularOp<OP_ABS, T>(), V, V);
  eval->addOp(OP_ASN, new RegularOp<OP_ASN, T>(), V, V);
  //eval->addOp(OP_NEG, new RegularOp<OP_NEG, T>());
  eval->addOp(OP_CSUM, new RegularOp<OP_CSUM, T>(), V, V);
  eval->addOp(OP_CMUL, new RegularOp<OP_CMUL, T>(), V, V);
  eval->addOp(OP_SGN, new RegularOp<OP_SGN, T>(), V, V);

  eval->addOp(OP_ADD, new RegularOp<OP_ADD, T>(), V, V, V);
  eval->addOp(OP_DIV, new RegularOp<OP_DIV, T>(), V, V, V);
  eval->addOp(OP_DOT, new RegularOp<OP_DOT, T>(), V, V, V);
  eval->addOp(OP_MAX, new RegularOp<OP_MAX, T>(), V, V, V);
  eval->addOp(OP_MIN, new RegularOp<OP_MIN, T>(), V, V, V);
  eval->addOp(OP_MOD, new RegularOp<OP_MOD, T>(), V, V, V);
  eval->addOp(OP_MUL, new RegularOp<OP_MUL, T>(), V, V, V);
  eval->addOp(OP_POW, new RegularOp<OP_POW, T>(), V, V, V);
  eval->addOp(OP_SEQ, new RegularOp<OP_SEQ, T>(), V, V, V);
  eval->addOp(OP_SGE, new RegularOp<OP_SGE, T>(), V, V, V);
  eval->addOp(OP_SGT, new RegularOp<OP_SGT, T>(), V, V, V);
  eval->addOp(OP_SLE, new RegularOp<OP_SLE, T>(), V, V, V);
  eval->addOp(OP_SLT, new RegularOp<OP_SLT, T>(), V, V, V);
  eval->addOp(OP_SNE, new RegularOp<OP_SNE, T>(), V, V, V);
  eval->addOp(OP_XPD, new RegularOp<OP_XPD, T>(), V, V, V);

  eval->addOp(OP_LRP, new RegularOp<OP_LRP, T>(), V, V, V, V);
  eval->addOp(OP_MAD, new RegularOp<OP_MAD, T>(), V, V, V, V);
  eval->addOp(OP_COND, new RegularOp<OP_COND, T>(), V, V, V, V);

  // unary ASN equivalents for ints 
  eval->addOp(OP_CEIL, new RegularOp<OP_ASN, T>(), V, V);
  eval->addOp(OP_FLR, new RegularOp<OP_ASN, T>(), V, V);
  eval->addOp(OP_FRAC, new RegularOp<OP_ASN, T>(), V, V);
  eval->addOp(OP_RND, new RegularOp<OP_ASN, T>(), V, V);

  // all other ops must use float (since they can potentially give float
  // results)
}

}

#endif
