#ifndef SHEVALIMPL_HPP 
#define SHEVALIMPL_HPP

#include "ShEval.hpp"
#include "ShTypeInfo.hpp"
#include "ShDebug.hpp"
#include "ShError.hpp"

namespace SH {


/******************************************************
 ***  Virtual Function Definitions  for ShDataCloak ***
 ******************************************************/
template<typename T> 
void ShDataEval<T>::srcUnaryOp(ShOperation op, ShCloakPtr src) const
{
  CloakCPtr srcCast = shref_dynamic_cast<const ShDataCloak<T> >(src); 
  switch(op) {
    case SH_OP_KIL: KIL(srcCast->data());
    default: SH_DEBUG_ASSERT(0);
  }
}

#define SHDCE_UNARY_OP(op) case SH_OP_ ## op: op(destCast->data(), srcCast->data())
template<typename T> 
void ShDataEval<T>::unaryOp(ShOperation op, ShCloakPtr dest, ShCloakCPtr src) const
{
  CloakPtr destCast = shref_dynamic_cast<ShDataCloak<T> >(dest); 
  CloakCPtr srcCast = shref_dynamic_cast<const ShDataCloak<T> >(src); 
  switch(op) {
    //SHDCE_UNARY_OP(NEG);
    SHDCE_UNARY_OP(ABS);
    SHDCE_UNARY_OP(ACOS);
    SHDCE_UNARY_OP(ASIN);
    SHDCE_UNARY_OP(ATAN);
    SHDCE_UNARY_OP(CEIL);
    SHDCE_UNARY_OP(COS);
    SHDCE_UNARY_OP(DX);
    SHDCE_UNARY_OP(DY);
    SHDCE_UNARY_OP(EXP);
    SHDCE_UNARY_OP(EXP2);
    SHDCE_UNARY_OP(EXP10);
    SHDCE_UNARY_OP(FLR);
    SHDCE_UNARY_OP(FRAC);
    SHDCE_UNARY_OP(LOG);
    SHDCE_UNARY_OP(LOG2);
    SHDCE_UNARY_OP(LOG10);
    SHDCE_UNARY_OP(RCP);
    SHDCE_UNARY_OP(RSQ);
    SHDCE_UNARY_OP(SIN);
    SHDCE_UNARY_OP(SGN);
    SHDCE_UNARY_OP(SQRT);
    SHDCE_UNARY_OP(TAN);
    SHDCE_UNARY_OP(NORM);
    default: SH_DEBUG_ASSERT(0);
  }
}

#define SHDCE_BINARY_OP(op) case SH_OP_ ## op: op(destCast->data(), aCast->data(), bCast->data())
template<typename T> 
void ShDataEval<T>::binaryOp(ShOperation op, ShCloakPtr dest, 
    ShCloakCPtr a, ShCloakCPtr b) const
{
  CloakPtr destCast = shref_dynamic_cast<ShDataCloak<T> >(dest); 
  CloakCPtr aCast = shref_dynamic_cast<const ShDataCloak<T> >(a); 
  CloakCPtr bCast = shref_dynamic_cast<const ShDataCloak<T> >(b); 
  switch(op) {
    SHDCE_BINARY_OP(ADD);
    SHDCE_BINARY_OP(MUL);
    SHDCE_BINARY_OP(DIV);
    SHDCE_BINARY_OP(SLT);
    SHDCE_BINARY_OP(SLE);
    SHDCE_BINARY_OP(SGT);
    SHDCE_BINARY_OP(SGE);
    SHDCE_BINARY_OP(SEQ);
    SHDCE_BINARY_OP(SNE);
    SHDCE_BINARY_OP(ATAN2);
    SHDCE_BINARY_OP(DOT);
    SHDCE_BINARY_OP(MAX);
    SHDCE_BINARY_OP(MIN);
    SHDCE_BINARY_OP(MOD);
    SHDCE_BINARY_OP(POW);
    SHDCE_BINARY_OP(XPD);
    default: SH_DEBUG_ASSERT(0);
  }
}

#define SHDCE_TERNARY_OP(op) case SH_OP_ ## op: op(destCast->data(), aCast->data(), bCast->data(), cCast->data())
template<typename T> 
void ShDataEval<T>::ternaryOp(ShOperation op, ShCloakPtr dest,
    ShCloakCPtr a, ShCloakCPtr b, ShCloakCPtr c) const
{
  CloakPtr destCast = shref_dynamic_cast<ShDataCloak<T> >(dest); 
  CloakCPtr aCast = shref_dynamic_cast<const ShDataCloak<T> >(a); 
  CloakCPtr bCast = shref_dynamic_cast<const ShDataCloak<T> >(b); 
  CloakCPtr cCast = shref_dynamic_cast<const ShDataCloak<T> >(c); 
  switch(op) {
    SHDCE_TERNARY_OP(LRP);
    SHDCE_TERNARY_OP(MAD);
    SHDCE_TERNARY_OP(COND);
    default: SH_DEBUG_ASSERT(0);
  }
}


/**************************************************************
 ***  Static Template Function Definitions  for ShDataCloak ***
 **************************************************************/
#define SHEVAL_CWISE_UNARY_OP(op, opsrc) \
template<typename T>\
void ShDataEval<T>::op(DataVec & dest, const DataVec &src) \
{\
  int so = src.size() > 1;\
\
  typename DataVec::iterator D = dest.begin();\
  typename DataVec::const_iterator S = src.begin();\
  for(; D != dest.end(); S += so, ++D) {\
    (*D) = opsrc;\
  }\
}

#define SHEVAL_CWISE_BINARY_OP(op, opsrc) \
template<typename T>\
void ShDataEval<T>::op(DataVec & dest, const DataVec &a, const DataVec &b) \
{\
  int ao = a.size() > 1;\
  int bo = b.size() > 1;\
\
  typename DataVec::iterator D = dest.begin();\
  typename DataVec::const_iterator A = a.begin();\
  typename DataVec::const_iterator B = b.begin();\
  for(; D != dest.end(); A += ao, B += bo, ++D) {\
    (*D) = opsrc;\
  }\
}

#define SHEVAL_CWISE_TERNARY_OP(op, opsrc) \
template<typename T>\
void ShDataEval<T>::op(DataVec & dest, const DataVec &a, const DataVec &b, const DataVec &c) \
{\
  int ao = a.size() > 1;\
  int bo = b.size() > 1;\
  int co = c.size() > 1;\
\
  typename DataVec::iterator D = dest.begin();\
  typename DataVec::const_iterator A = a.begin();\
  typename DataVec::const_iterator B = b.begin();\
  typename DataVec::const_iterator C = c.begin();\
  for(; D != dest.end(); A += ao, B += bo, C += co, ++D) {\
    (*D) = opsrc;\
  }\
}

SHEVAL_CWISE_UNARY_OP(ASN, (*S));
//SHEVAL_CWISE_UNARY_OP(NEG, -(*S));

SHEVAL_CWISE_BINARY_OP(ADD, (*A) + (*B));
SHEVAL_CWISE_BINARY_OP(MUL, (*A) * (*B));
SHEVAL_CWISE_BINARY_OP(DIV, (*A) / (*B));

template<typename T>
void ShDataEval<T>::SLT(DataVec & dest, const DataVec &a, const DataVec &b) 
{
  int ao = a.size() > 1;
  int bo = b.size() > 1;

  typename DataVec::iterator D = dest.begin();
  typename DataVec::const_iterator A = a.begin();
  typename DataVec::const_iterator B = b.begin();
  for(; D != dest.end(); A += ao, B += bo, ++D) {
    (*D) = ((*A) < (*B) ? 
            ShConcreteTypeInfo<T>::trueValue : 
            ShConcreteTypeInfo<T>::falseValue);
  }
}
//SHEVAL_CWISE_BINARY_OP(SLT, ((*A) < (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));
SHEVAL_CWISE_BINARY_OP(SLE, ((*A) <= (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));
SHEVAL_CWISE_BINARY_OP(SGT, ((*A) > (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));
SHEVAL_CWISE_BINARY_OP(SGE, ((*A) >= (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));
SHEVAL_CWISE_BINARY_OP(SEQ, ((*A) == (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));
SHEVAL_CWISE_BINARY_OP(SNE, ((*A) != (*B) ? ShConcreteTypeInfo<T>::trueValue : ShConcreteTypeInfo<T>::falseValue));

// TODO handle double cases using template specialization
SHEVAL_CWISE_UNARY_OP(ABS, std::fabs(*S));
SHEVAL_CWISE_UNARY_OP(ACOS, std::acos(*S));
SHEVAL_CWISE_UNARY_OP(ASIN, std::asin(*S));
SHEVAL_CWISE_UNARY_OP(ATAN, std::atan(*S));
SHEVAL_CWISE_BINARY_OP(ATAN2, std::atan2((*A), (*B)));
SHEVAL_CWISE_UNARY_OP(CEIL, std::ceil(*S));
SHEVAL_CWISE_UNARY_OP(COS, std::cos(*S));

template<typename T> 
void ShDataEval<T>::DOT(DataVec & dest, const DataVec &a, const DataVec &b)
{
  // dest.size should be 1 and a.size == b.size
  typename DataVec::const_iterator A = a.begin();
  typename DataVec::const_iterator B = b.begin();
  T &result = dest[0];
  result = 0; 
  for(; A != a.end(); ++A, ++B) result += (*A) * (*B); 
}

template<typename T>
void ShDataEval<T>::DX(DataVec & dest, const DataVec &src)
{
  shError(ShScopeException("Cannot take derivatives in immediate mode"));
}

template<typename T>
void ShDataEval<T>::DY(DataVec & dest, const DataVec &src)
{
  shError(ShScopeException("Cannot take derivatives in immediate mode"));
}

SHEVAL_CWISE_UNARY_OP(EXP, std::exp(*S));
SHEVAL_CWISE_UNARY_OP(EXP2, exp2(*S));
SHEVAL_CWISE_UNARY_OP(EXP10,exp10(*S));
SHEVAL_CWISE_UNARY_OP(FLR, std::floor(*S));
SHEVAL_CWISE_UNARY_OP(FRAC, (*S) - std::floor(*S));
SHEVAL_CWISE_UNARY_OP(LOG, std::log(*S)); 
SHEVAL_CWISE_UNARY_OP(LOG2, log2(*S)); 
SHEVAL_CWISE_UNARY_OP(LOG10, log10(*S)); 
SHEVAL_CWISE_TERNARY_OP(LRP, (*A) * (*B) + (1 - (*A)) * (*C)); 
SHEVAL_CWISE_TERNARY_OP(MAD, (*A) * (*B) + (*C)); 
SHEVAL_CWISE_BINARY_OP(MAX, std::max((*A), (*B))); 
SHEVAL_CWISE_BINARY_OP(MIN, std::min((*A), (*B))); 
SHEVAL_CWISE_BINARY_OP(MOD, std::fmod((*A), (*B))); 
SHEVAL_CWISE_BINARY_OP(POW, std::pow((*A), (*B))); 
SHEVAL_CWISE_UNARY_OP(RCP, 1.0 / (*S)); 
SHEVAL_CWISE_UNARY_OP(RSQ, 1.0 / std::sqrt(*S)); 
SHEVAL_CWISE_UNARY_OP(SGN, ((*S) < 0 ? -1 : ((*S) > 0 ? 1 : 0))); 
SHEVAL_CWISE_UNARY_OP(SIN, std::sin(*S)); 
SHEVAL_CWISE_UNARY_OP(SQRT, std::sqrt(*S)); 
SHEVAL_CWISE_UNARY_OP(TAN, std::tan(*S)); 

template<typename T> 
void ShDataEval<T>::NORM(DataVec & dest, const DataVec &src)
{
  // assume dest.size == src.size 
  typename DataVec::const_iterator S = src.begin();
  T m = 0;
  for(; S != src.end(); ++S) m += (*S) * (*S); 
  m = 1.0 / std::sqrt(m);

  typename DataVec::iterator D = dest.begin();
  S = src.begin();
  for(; S != src.end(); ++D, ++S) (*D) = (*S) * m; 
}

template<typename T> 
void ShDataEval<T>::XPD(DataVec & dest, const DataVec &a, const DataVec &b)
{
  dest[0] = a[1] * b[2] - a[2] * b[1];
  dest[1] = -(a[0] * b[2] - a[2] * b[0]);
  dest[2] = a[0] * b[1] - a[1] * b[0];
}

SHEVAL_CWISE_TERNARY_OP(COND, ((*A) > 0 ? (*B) : (*C))); 

template<typename T>
void ShDataEval<T>::KIL(const DataVec& cond)
{
  shError(ShScopeException("Cannot kill in immediate mode"));
}

}

#endif
