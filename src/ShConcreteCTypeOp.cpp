// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
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
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_ABS, std::fabs(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_ABS, fabsf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_ACOS, std::acos(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_ACOS, acosf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_ASIN, std::asin(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_ASIN, asinf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_ATAN, std::atan(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_ATAN, atanf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_CBRT, std::pow(*A, 1.0 / 3.0));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_CBRT, powf(*A, 1.0f / 3.0f));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_CEIL, std::ceil(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_CEIL, ceilf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_COS, std::cos(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_COS, cosf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_EXP, std::exp(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_EXP, expf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_EXP2, std::pow(2.0, *A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_EXP2, powf(2.0f, *A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_EXP10, std::pow(10.0, *A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_EXP10, powf(10.0f, *A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_FLR, std::floor(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_FLR, floorf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_FRAC, (*A) - std::floor(*A));
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_FRAC, (*A) - floorf(*A));
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_LOG, std::log(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_LOG, logf(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_LOG2, std::log(*A) / std::log(2.0)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_LOG2, log2f(*A) / log2f(2.0f)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_LOG10, std::log(*A) / std::log(10.0)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_LOG10, logf(*A) / logf(10.0f)); 


void ShConcreteCTypeOp<SH_OP_NORM, SH_DOUBLE>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  double m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}


void ShConcreteCTypeOp<SH_OP_NORM, SH_FLOAT>::doop(DataPtr dest, DataCPtr a, DataCPtr b, DataCPtr c) 
{
  // assume dest.size == a->size 
  float m = std::sqrt(std::inner_product(a->begin(), a->end(), a->begin(), 0.0f));

  Variant::const_iterator A = a->begin();
  Variant::iterator D = dest->begin();
  for(; A != a->end(); ++D, ++A) (*D) = (*A) / m; 
}

SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_RCP, 1.0 / (*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_RCP, 1.0f / (*A)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_RND, std::floor(*A + 0.5)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_RND, floorf(*A + 0.5f)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_RSQ, 1.0 / std::sqrt(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_RSQ, 1.0f / sqrtf(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT , SH_OP_SGN, ((*A) < 0 ? -1 : (*A) > 0 ? 1 : 0)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_SIN, std::sin(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_SIN, sinf(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_SQRT, std::sqrt(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_SQRT, sqrtf(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_DOUBLE, SH_OP_TAN, std::tan(*A)); 
SHCCTO_UNARY_OP_SPEC(SH_FLOAT,  SH_OP_TAN, tanf(*A)); 

/* Specializations for binary ops */
SHCCTO_BINARY_OP_SPEC(SH_DOUBLE, SH_OP_ATAN2, std::atan2((*A), (*B)));
SHCCTO_BINARY_OP_SPEC(SH_FLOAT,  SH_OP_ATAN2, atan2f((*A), (*B)));


// Binary Ops
SHCCTO_BINARY_OP_SPEC(SH_DOUBLE, SH_OP_MOD, std::fmod((*A), (*B))); 
SHCCTO_BINARY_OP_SPEC(SH_FLOAT,  SH_OP_MOD, fmodf((*A), (*B))); 
SHCCTO_BINARY_OP_SPEC(SH_DOUBLE, SH_OP_POW, std::pow((*A), (*B))); 
SHCCTO_BINARY_OP_SPEC(SH_FLOAT,  SH_OP_POW, powf((*A), (*B))); 

}
