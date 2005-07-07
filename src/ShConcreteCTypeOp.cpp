// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <cmath>
#include "ShMath.hpp"
#include <numeric>
#include "ShOperation.hpp"
#include "ShEval.hpp"

namespace SH {

  // TODO some of these specializations don't need to be in the cpp
  // since they work for most things...

/**************************************************************
 *** Use Specialization to define particular ops 
 **************************************************************/
#define SHCCTO_UNARY_OP_SPEC_TMPL(T, op, opsrc)\
template<>\
SHCCTO_UNARY_OP_SPEC(T, op, opsrc)

#define SHCCTO_UNARY_OP_SPEC(T, op, opsrc)\
void ShConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
{\
  int ao = a->size() > 1;\
\
  Variant::iterator D = dest->begin();\
  Variant::const_iterator A = a->begin();\
  for(; D != dest->end(); A += ao, ++D) {\
    (*D) = opsrc;\
  }\
}\

#define SHCCTO_BINARY_OP_SPEC(T, op, opsrc)\
void ShConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
{\
  int ao = a->size() > 1;\
  int bo = b->size() > 1;\
\
  Variant::iterator D = dest->begin();\
  Variant::const_iterator A, B;\
  A = a->begin();\
  B = b->begin();\
  for(; D != dest->end(); A += ao, B += bo, ++D) {\
    (*D) = opsrc;\
  }\
}

#define SHCCTO_TERNARY_OP_SPEC(T, op, opsrc)\
void ShConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
{\
  int ao = a->size() > 1;\
  int bo = b->size() > 1;\
  int co = c->size() > 1;\
\
  Variant::iterator D = dest->begin();\
  Variant::const_iterator A, B, C;\
  A = a->begin();\
  B = b->begin();\
  C = c->begin();\
  for(; D != dest->end(); A += ao, B += bo, C += co, ++D) {\
    (*D) = opsrc;\
  }\
}   

/* Specializations for unary ops */
SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_ABS, std::fabs(*A));
SHCCTO_UNARY_OP_SPEC_TMPL(float,  SH_OP_ABS, fabsf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_ACOS, std::acos(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_ACOS, acosf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_ASIN, std::asin(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_ASIN, asinf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_ATAN, std::atan(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_ATAN, atanf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_CBRT, std::pow(*A, 1.0 / 3.0));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_CBRT, powf(*A, 1.0f / 3.0f));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_CEIL, std::ceil(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_CEIL, ceilf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_COS, std::cos(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_COS, cosf(*A));
SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_COSH, std::cosh(*A));
SHCCTO_UNARY_OP_SPEC_TMPL(float,  SH_OP_COSH, coshf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_EXP, std::exp(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_EXP, expf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_EXP2, std::pow(2.0, *A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_EXP2, powf(2.0f, *A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_EXP10, std::pow(10.0, *A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_EXP10, powf(10.0f, *A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_FLR, std::floor(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_FLR, floorf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_FRAC, (*A) - std::floor(*A));
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_FRAC, (*A) - floorf(*A));
SHCCTO_UNARY_OP_SPEC(double, SH_OP_LOG, std::log(*A)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_LOG, logf(*A)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_LOG2, std::log(*A) / std::log(2.0)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_LOG2, log2f(*A) / log2f(2.0f)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_LOG10, std::log(*A) / std::log(10.0)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_LOG10, logf(*A) / logf(10.0f)); 


void ShConcreteCTypeOp<SH_OP_NORM, double>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  double m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}


void ShConcreteCTypeOp<SH_OP_NORM, float>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  float m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0f));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}

SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_RCP, 1.0 / (*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(float,  SH_OP_RCP, 1.0f / (*A)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_RND, std::floor(*A + 0.5)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_RND, floorf(*A + 0.5f)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_RSQ, 1.0 / std::sqrt(*A)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_RSQ, 1.0f / sqrtf(*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
SHCCTO_UNARY_OP_SPEC_TMPL(float , SH_OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_SIN, std::sin(*A)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_SIN, sinf(*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_SINH, std::sinh(*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(float,  SH_OP_SINH, sinhf(*A)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_SQRT, std::sqrt(*A)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_SQRT, sqrtf(*A)); 
SHCCTO_UNARY_OP_SPEC(double, SH_OP_TAN, std::tan(*A)); 
SHCCTO_UNARY_OP_SPEC(float,  SH_OP_TAN, tanf(*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(double, SH_OP_TANH, std::tanh(*A)); 
SHCCTO_UNARY_OP_SPEC_TMPL(float,  SH_OP_TANH, tanhf(*A)); 

/* Specializations for binary ops */
SHCCTO_BINARY_OP_SPEC(double, SH_OP_ATAN2, std::atan2((*A), (*B)));
SHCCTO_BINARY_OP_SPEC(float,  SH_OP_ATAN2, atan2f((*A), (*B)));


// Binary Ops
SHCCTO_BINARY_OP_SPEC(double, SH_OP_MOD, ((*A) - (*B) * std::floor((*A) / (*B)))); 
SHCCTO_BINARY_OP_SPEC(float,  SH_OP_MOD, ((*A) - (*B) * floorf((*A) / (*B)))); 
SHCCTO_BINARY_OP_SPEC(double, SH_OP_POW, std::pow((*A), (*B))); 
SHCCTO_BINARY_OP_SPEC(float,  SH_OP_POW, powf((*A), (*B))); 

}
