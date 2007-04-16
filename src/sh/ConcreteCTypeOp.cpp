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
#include <cmath>
#include "Math.hpp"
#include <numeric>
#include "Operation.hpp"
#include "Eval.hpp"

namespace SH {

  // TODO some of these specializations don't need to be in the cpp
  // since they work for most things...

/**************************************************************
 *** Use Specialization to define particular ops 
 **************************************************************/
#define CCTO_UNARY_OP_SPEC(T, op, opsrc)\
void ConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
{\
  int ao = a->size() > 1;\
\
  Variant::iterator D = dest->begin();\
  Variant::const_iterator A = a->begin();\
  for(; D != dest->end(); A += ao, ++D) {\
    (*D) = opsrc;\
  }\
}

#define CCTO_UNARY_OP_SPEC_TMPL(T, op, opsrc)\
template<>\
CCTO_UNARY_OP_SPEC(T, op, opsrc)


#define CCTO_BINARY_OP_SPEC(T, op, opsrc)\
void ConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
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

#define CCTO_TERNARY_OP_SPEC(T, op, opsrc)\
void ConcreteCTypeOp<op, T>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) \
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
CCTO_UNARY_OP_SPEC_TMPL(double, OP_ABS, std::fabs(*A));
CCTO_UNARY_OP_SPEC_TMPL(float,  OP_ABS, fabsf(*A));
CCTO_UNARY_OP_SPEC(double, OP_ACOS, std::acos(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ACOS, acosf(*A));
CCTO_UNARY_OP_SPEC(double, OP_ASIN, std::asin(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ASIN, asinf(*A));
CCTO_UNARY_OP_SPEC(double, OP_ATAN, std::atan(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ATAN, atanf(*A));
CCTO_UNARY_OP_SPEC(double, OP_CBRT, std::pow(*A, 1.0 / 3.0));
CCTO_UNARY_OP_SPEC(float,  OP_CBRT, powf(*A, 1.0f / 3.0f));
CCTO_UNARY_OP_SPEC(double, OP_CEIL, std::ceil(*A));
CCTO_UNARY_OP_SPEC(float,  OP_CEIL, ceilf(*A));
CCTO_UNARY_OP_SPEC(double, OP_COS, std::cos(*A));
CCTO_UNARY_OP_SPEC(float,  OP_COS, cosf(*A));
CCTO_UNARY_OP_SPEC_TMPL(double, OP_COSH, std::cosh(*A));
CCTO_UNARY_OP_SPEC_TMPL(float,  OP_COSH, coshf(*A));
CCTO_UNARY_OP_SPEC(double, OP_EXP, std::exp(*A));
CCTO_UNARY_OP_SPEC(float,  OP_EXP, expf(*A));
CCTO_UNARY_OP_SPEC(double, OP_EXP2, std::pow(2.0, *A));
CCTO_UNARY_OP_SPEC(float,  OP_EXP2, powf(2.0f, *A));
CCTO_UNARY_OP_SPEC(double, OP_EXP10, std::pow(10.0, *A));
CCTO_UNARY_OP_SPEC(float,  OP_EXP10, powf(10.0f, *A));
CCTO_UNARY_OP_SPEC(double, OP_FLR, std::floor(*A));
CCTO_UNARY_OP_SPEC(float,  OP_FLR, floorf(*A));
CCTO_UNARY_OP_SPEC(double, OP_FRAC, (*A) - std::floor(*A));
CCTO_UNARY_OP_SPEC(float,  OP_FRAC, (*A) - floorf(*A));
CCTO_UNARY_OP_SPEC(double, OP_LOG, std::log(*A)); 
CCTO_UNARY_OP_SPEC(float,  OP_LOG, logf(*A)); 
CCTO_UNARY_OP_SPEC(double, OP_LOG2, std::log(*A) / std::log(2.0)); 
CCTO_UNARY_OP_SPEC(float,  OP_LOG2, log2f(*A) / log2f(2.0f)); 
CCTO_UNARY_OP_SPEC(double, OP_LOG10, std::log(*A) / std::log(10.0)); 
CCTO_UNARY_OP_SPEC(float,  OP_LOG10, logf(*A) / logf(10.0f)); 

#ifdef _WIN32
CCTO_UNARY_OP_SPEC(double, OP_ACOSH, std::log((*A) + std::sqrt((*A) * (*A) - 1)));
CCTO_UNARY_OP_SPEC(float,  OP_ACOSH, logf((*A) + sqrtf((*A) * (*A) - 1)));
CCTO_UNARY_OP_SPEC(double, OP_ASINH, std::log((*A) + std::sqrt((*A) * (*A) + 1)));
CCTO_UNARY_OP_SPEC(float,  OP_ASINH, logf((*A) + sqrtf((*A) * (*A) + 1)));
CCTO_UNARY_OP_SPEC(double, OP_ATANH, std::log((1.0 + (*A))/(1.0 - (*A)))/2.0);
CCTO_UNARY_OP_SPEC(float,  OP_ATANH, logf((1.0 + (*A))/(1.0 - (*A)))/2.0);
#else
CCTO_UNARY_OP_SPEC(double, OP_ACOSH, ::acosh(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ACOSH, acoshf(*A));
CCTO_UNARY_OP_SPEC(double, OP_ASINH, ::asinh(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ASINH, asinhf(*A));
CCTO_UNARY_OP_SPEC(double, OP_ATANH, ::atanh(*A));
CCTO_UNARY_OP_SPEC(float,  OP_ATANH, atanhf(*A));
#endif

void ConcreteCTypeOp<OP_NORM, double>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  double m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}


void ConcreteCTypeOp<OP_NORM, float>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  float m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0f));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}

CCTO_UNARY_OP_SPEC_TMPL(double, OP_RCP, 1.0 / (*A)); 
CCTO_UNARY_OP_SPEC_TMPL(float,  OP_RCP, 1.0f / (*A)); 
CCTO_UNARY_OP_SPEC(double, OP_RND, std::floor(*A + 0.5)); 
CCTO_UNARY_OP_SPEC(float,  OP_RND, floorf(*A + 0.5f)); 
CCTO_UNARY_OP_SPEC(double, OP_RSQ, 1.0 / std::sqrt(*A)); 
CCTO_UNARY_OP_SPEC(float,  OP_RSQ, 1.0f / sqrtf(*A)); 
CCTO_UNARY_OP_SPEC_TMPL(double, OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
CCTO_UNARY_OP_SPEC_TMPL(float , OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
CCTO_UNARY_OP_SPEC(double, OP_SIN, std::sin(*A)); 
CCTO_UNARY_OP_SPEC(float,  OP_SIN, sinf(*A)); 
CCTO_UNARY_OP_SPEC_TMPL(double, OP_SINH, std::sinh(*A)); 
CCTO_UNARY_OP_SPEC_TMPL(float,  OP_SINH, sinhf(*A)); 
CCTO_UNARY_OP_SPEC(double, OP_SQRT, std::sqrt(*A)); 
CCTO_UNARY_OP_SPEC(float,  OP_SQRT, sqrtf(*A)); 
CCTO_UNARY_OP_SPEC(double, OP_TAN, std::tan(*A)); 
CCTO_UNARY_OP_SPEC(float,  OP_TAN, tanf(*A)); 
CCTO_UNARY_OP_SPEC_TMPL(double, OP_TANH, std::tanh(*A)); 
CCTO_UNARY_OP_SPEC_TMPL(float,  OP_TANH, tanhf(*A)); 

/* Specializations for binary ops */
CCTO_BINARY_OP_SPEC(double, OP_ATAN2, std::atan2((*A), (*B)));
CCTO_BINARY_OP_SPEC(float,  OP_ATAN2, atan2f((*A), (*B)));


// Binary Ops
CCTO_BINARY_OP_SPEC(double, OP_MOD, ((*A) - (*B) * std::floor((*A) / (*B)))); 
CCTO_BINARY_OP_SPEC(float,  OP_MOD, ((*A) - (*B) * floorf((*A) / (*B)))); 
CCTO_BINARY_OP_SPEC(double, OP_POW, std::pow((*A), (*B))); 
CCTO_BINARY_OP_SPEC(float,  OP_POW, powf((*A), (*B))); 

}
