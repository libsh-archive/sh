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
#ifndef SHLIB_HPP
#define SHLIB_HPP

#include <cmath>
#include <cassert>
#include "ShVariable.hpp"
#include "ShAttrib.hpp"
#include "ShMatrix.hpp"

/** \file ShLib.hpp
 * \brief The Sh library functions.
 * If you add or remove functions in this file be sure to modify the
 * other ShLib*.hpp (e.g. ShLibVector.hpp) files appropriately. See
 * the bottom of the file for a list.
 */

#define SH_SHLIB_CONST_SCALAR_OP(operation) \
template<typename T> \
ShVariableN<1, T> operation(const ShVariableN<1, T>& left, T right) \
{ \
  return operation(left, ShConstant1f(right)); \
} \
template<typename T> \
ShVariableN<1, T> operation(T left, const ShVariableN<1, T>& right) \
{ \
  return operation(ShConstant1f(left), right); \
} \
template<typename T> \
ShVariableN<1, T> operation(const ShVariableN<1, T>& left, double right) \
{ \
  return operation(left, ShConstant1f(right)); \
} \
template<typename T> \
ShVariableN<1, T> operation(double left, const ShVariableN<1, T>& right) \
{ \
  return operation(ShConstant1f(left), right); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize) \
template<int N, typename T> \
ShVariableN<retsize, T> operation(const ShVariableN<N, T>& left, T right) \
{ \
  return operation(left, ShConstant1f(right)); \
} \
template<int N, typename T> \
ShVariableN<retsize, T> operation(const ShVariableN<N, T>& left, double right) \
{ \
  return operation(left, ShConstant1f(right)); \
}

#define SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize) \
template<int N, typename T> \
ShVariableN<retsize, T> operation(T left, const ShVariableN<N, T>& right) \
{ \
  return operation(ShConstant1f(left), right); \
} \
template<int N, typename T> \
ShVariableN<retsize, T> operation(double left, const ShVariableN<N, T>& right) \
{ \
  return operation(ShConstant1f(left), right); \
}

#define SH_SHLIB_CONST_N_OP_LEFT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, N);

#define SH_SHLIB_CONST_N_OP_RIGHT(operation) \
  SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, N);

#define SH_SHLIB_CONST_N_OP_BOTH(operation) \
SH_SHLIB_CONST_N_OP_LEFT(operation); \
SH_SHLIB_CONST_N_OP_RIGHT(operation);

#define SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(operation, retsize) \
SH_SHLIB_CONST_N_OP_RETSIZE_LEFT(operation, retsize); \
SH_SHLIB_CONST_N_OP_RETSIZE_RIGHT(operation, retsize);

namespace SH {

/** Utility template to check whether dimensions are equal or scalar.
 * If ScalarLHS or ScalarRHS are true, then the left or right
 * arguments respectively are allowed to be 1.
 *
 * This is kind of ugly, but it saves us having to define several
 *operator*, operator/, etc. later on.
 */
template<int N, bool ScalarLHS, int M, bool ScalarRHS>
class ShCheckDims 
{
private:
  ShCheckDims();
};
template<int N> class ShCheckDims<N, false, N, false>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, false, N, true>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, true, N, false>
{public: ShCheckDims() {}};
template<int N> class ShCheckDims<N, true, N, true>
{public: ShCheckDims() {}};
template<int N, bool ScalarRHS> class ShCheckDims<1, true, N, ScalarRHS>
{public: ShCheckDims() {}};
template<int N, bool ScalarLHS> class ShCheckDims<N, ScalarLHS, 1, true>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, false, 1, false>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, false, 1, true>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, true, 1, false>
{public: ShCheckDims() {}};
template<> class ShCheckDims<1, true, 1, true>
{public: ShCheckDims() {}};

/// Addition
template<int N, typename T>
ShVariableN<N, T> operator+(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] + rvals[i];
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_ADD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator+);

/// Subtraction
template<int N, typename T>
ShVariableN<N, T> operator-(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  return left + (-right);
}

SH_SHLIB_CONST_SCALAR_OP(operator-);

/// Componentwise/scalar multiplication
template<int N, int M, typename T>
ShVariableN<N, T> operator*(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, true, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] * rvals[M == 1 ? 0 : i];
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

template<int M, typename T>
ShVariableN<M, T> operator*(const ShVariableN<1, T>& left, const ShVariableN<M, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[1];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[M];
    for (int i = 0; i < M; i++) result[i] = lvals[0] * rvals[i];
    return ShConstant<M, T>(result);
  } else {
    ShAttrib<M, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MUL, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);

    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator*);
SH_SHLIB_CONST_N_OP_BOTH(operator*);

/// Componentwise/scalar division
template<int N, int M, typename T>
ShVariableN<N, T> operator/(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, false, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[M];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = lvals[i] / rvals[M == 1 ? 0 : i];
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_DIV, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(operator/);
SH_SHLIB_CONST_N_OP_LEFT(operator/);

/// Conventional power operation.
template<int N, typename T>
ShVariableN<N, T> pow(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::pow(lvals[i], rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_POW, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(pow);
SH_SHLIB_CONST_N_OP_RIGHT(pow);

template<int N, typename T>
ShVariableN<N, T> operator^(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right) {
  return pow(left, right);
}

SH_SHLIB_CONST_SCALAR_OP(operator^);
SH_SHLIB_CONST_N_OP_RIGHT(operator^);

// "Boolean" operators
#define SH_SHLIB_BOOLEAN_OP(opfunc, op, shop) \
template<int N, int M, typename T>\
ShVariableN<N, T> opfunc(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)\
{\
  ShCheckDims<N, true, M, true>();\
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    assert(right.hasValues());\
    T lvals[N];\
    left.getValues(lvals);\
    T rvals[M];\
    right.getValues(rvals);\
    T result[N];\
    for (int i = 0; i < N; i++) result[i] = (lvals[i] op rvals[(M == 1) ? 0 : i] ? 1.0 : 0.0);\
    return ShConstant<N, T>(result);\
  } else {\
    ShAttrib<N, SH_VAR_TEMP, T, false> t;\
    ShStatement stmt(t, left, shop, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}\
\
template<int M, typename T>\
ShVariableN<M, T> opfunc(const ShVariableN<1, T>& left, const ShVariableN<M, T>& right)\
{\
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    assert(right.hasValues());\
    T lvals[1];\
    left.getValues(lvals);\
    T rvals[M];\
    right.getValues(rvals);\
    T result[M];\
    for (int i = 0; i < M; i++) result[i] = (lvals[0] op rvals[i] ? 1.0 : 0.0);\
    return ShConstant<M, T>(result);\
  } else {\
    ShAttrib<M, SH_VAR_TEMP, T, false> t;\
    ShStatement stmt(t, left, shop, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}\
SH_SHLIB_CONST_SCALAR_OP(opfunc);\
SH_SHLIB_CONST_N_OP_BOTH(opfunc);

SH_SHLIB_BOOLEAN_OP(operator<, <, SH_OP_SLT); 
SH_SHLIB_BOOLEAN_OP(operator<=, <=, SH_OP_SLE); 
SH_SHLIB_BOOLEAN_OP(operator>, >, SH_OP_SGT); 
SH_SHLIB_BOOLEAN_OP(operator>=, >=, SH_OP_SGE); 
SH_SHLIB_BOOLEAN_OP(operator==, ==, SH_OP_SEQ); 
SH_SHLIB_BOOLEAN_OP(operator!=, !=, SH_OP_SNE); 


/** Conditional assignment (COND/CMP)
 *  dest[i] = (src[0][i] > 0.0 ? src[1][i] : src[2][i])
 *  Note: CMP in the ARB_{vertex,fragment}_program spec has
 *  src[0][i] < 0.0, not greater than.
 */
template<int M, int N, typename T>
ShVariableN<N, T> cond(const ShVariableN<M, T>& condition, const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  ShCheckDims<N, true, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(condition.hasValues());
    assert(left.hasValues());
    assert(right.hasValues());
    T cvals[M];
    condition.getValues(cvals);
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (cvals[M == 1 ? 0 : i] > 0.0 ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COND, condition, left, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/** Casting
 * Casts ShAttrib type T2 to type T1.
 * If T1::typesize < T2::typesize, pads remaining components with 0s (on right).
 * Otherwise, discards extra components.
 */
template<typename T1, typename T2> 
T1 cast( const T2 &a ) {
  const int s1 = T1::typesize;
  const int s2 = T2::typesize;
  int copySize = std::min(s1, s2);
  typename T1::TempType result;

  int indices[copySize];
  for(int i = 0; i < copySize; ++i) indices[i] = i;
  if(s1 < s2) {
    result = a.template swiz<s1>(indices);
  } else {
    result.template swiz<s2>(indices) = a;
  }
  return result;
}

// Fragment killing
template<int N, typename T>
void kill(const ShVariableN<N, T>& c)
{
  assert(ShEnvironment::insideShader);
  ShStatement stmt(c, SH_OP_KIL, c);
  ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
}

//--- Mathematics Library

/// Absolute value
template<int N, typename T>
ShVariableN<N, T> abs(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (vals[i] >= 0.0 ? vals[i] : -vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ABS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Arccosine of x. x in [-1, 1], result in [-pi/2, pi/2]
template<int N, typename T>
ShVariableN<N, T> acos(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::acos(vals[i]) - M_PI/2;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ACOS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Arcsine of x. x in [-1, 1]. Result in [0, pi]
template<int N, typename T>
ShVariableN<N, T> asin(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::asin(vals[i]) + M_PI/2;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_ASIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// least integer <= argument 
template<int N, typename T>
ShVariableN<N, T> ceil(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::ceil(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_CEIL, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Cosine of x.
template<int N, typename T>
ShVariableN<N, T> cos(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::cos(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_COS, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

/// Componentwise/scalar dot product
#define SH_LIB_DOTPRODUCT(opfunc) \
template<int M, int N, typename T>\
ShVariableN<1,  T> opfunc(const ShVariableN<M, T>& left, const ShVariableN<N, T>& right)\
{\
  ShCheckDims<M, true, N, true>();\
  if (!ShEnvironment::insideShader) {\
    assert(left.hasValues());\
    T lvals[M];\
    left.getValues(lvals);\
    T rvals[N];\
    right.getValues(rvals);\
    T result = 0.0;\
    for (int i = 0; i < M || i < N; i++) result += \
        ( M == 1 ? lvals[0] : lvals[i] ) * ( N == 1 ? rvals[0] : rvals[i] );\
    return ShConstant<1, T>(result);\
  } else {\
    ShAttrib<1, SH_VAR_TEMP, T, false> t;\
    ShStatement stmt(t, left, SH_OP_DOT, right);\
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);\
    return t;\
  }\
}

SH_LIB_DOTPRODUCT(dot);
SH_LIB_DOTPRODUCT(operator|);

SH_SHLIB_CONST_N_OP_RETSIZE_BOTH(dot, 1);

/// greatest integer >= argument 
template<int N, typename T>
ShVariableN<N, T> floor(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::floor(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_FLR, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Float modulus 
template<int N, int M, typename T>
ShVariableN<N,  T> fmod(const ShVariableN<N, T>& left, const ShVariableN<M, T>& right)
{
  ShCheckDims<N, false, M, true>();
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues()); 
    assert(right.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N]; 
    for (int i = 0; i < N; i++) result[i] = 
      std::fmod( lvals[i], rvals[(M == 1 ? 0 : i)] ); 
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_FMOD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(fmod);
SH_SHLIB_CONST_N_OP_LEFT(fmod);

/// Fractional part.
template<int N, typename T>
ShVariableN<N, T> frac(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = fmodf(vals[i], 1.0f); // TODO: Check that this is correct
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_FRAC, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// linear interpolation.
template<int N, int M, typename T>
ShVariableN<N, T> lerp(const ShVariableN<M, T>& f, const ShVariableN<N, T>& a, 
    const ShVariableN<N,T>& b)
{
  ShCheckDims<M, true, N, false>();
  if (!ShEnvironment::insideShader) {
    assert(a.hasValues()); 
    assert(b.hasValues());
    assert(f.hasValues());
    T avals[N], bvals[N], fvals[M];
    a.getValues(avals);
    b.getValues(bvals);
    f.getValues(fvals);
    T result[N];
    for (int i = 0; i < N; i++) {
        T fi = ( M == 1 ? fvals[0] : fvals[i] ); 
        result[i] = fi * avals[i] + (1 - fi) * bvals[i]; 
    }
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_LRP, f, a, b); 
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T> 
ShVariableN<N, T> lerp(T f, const ShVariableN<N, T>& a, const ShVariableN<N, T>& b)
{ 
  return lerp(ShConstant1f(f), a, b); 
}

template<int N, int M, int P> 
class ShCheckMad {
  private:
    ShCheckMad();
};
template<int N> class ShCheckMad<N, N, N>
{public: ShCheckMad() {}};
template<int N> class ShCheckMad<1, N, N> 
{public: ShCheckMad() {}};
template<int N> class ShCheckMad<N, 1, N>
{public: ShCheckMad() {}};
template<> class ShCheckMad<1, 1, 1>
{public: ShCheckMad() {}};

/// Multiply and add
template<int N, int M, int P, typename T>
ShVariableN<P, T> mad(const ShVariableN<M, T>& m1, const ShVariableN<N, T>& m2, 
    const ShVariableN<P,T>& a)
{
  ShCheckMad<N, M, P>();
  if (!ShEnvironment::insideShader) {
    assert(m1.hasValues()); 
    assert(m2.hasValues());
    assert(a.hasValues());
    //TODO better error message.  can this method be cleaner?
    T m1vals[M], m2vals[N], avals[P];
    m1.getValues(m1vals);
    m2.getValues(m2vals);
    a.getValues(avals);
    T result[P];
    for (int i = 0; i < P; i++) {
        result[i] = ( M == 1 ? m1vals[0] : m1vals[i] ) * ( N == 1 ? m2vals[0] : m2vals[i] ) + avals[i]; 
    }
    return ShConstant<P, T>(result);
  } else {
    ShAttrib<P, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_MAD, m1, m2, a); 
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

/// Use template specialization for the N, 1, N case

template<int N, typename T> 
ShVariableN<N, T> mad(T m1, const ShVariableN<N, T>& m2, const ShVariableN<N, T>& a)
{ 
  return mad(ShConstant1f(m1), m2, a); 
}

template<int N, typename T> 
ShVariableN<N, T> mad(double m1, const ShVariableN<N, T>& m2, const ShVariableN<N, T>& a)
{ 
  return mad(ShConstant1f(m1), m2, a); 
}

template<int N, typename T> 
ShVariableN<N, T> mad(const ShVariableN<N, T>& m1, T m2, const ShVariableN<N, T>& a)
{ 
  return mad(m1, ShConstant1f(m2), a); 
}

template<int N, typename T> 
ShVariableN<N, T> mad(const ShVariableN<N, T>& m1, double m2, const ShVariableN<N, T>& a)
{ 
  return mad(m1, ShConstant1f(m2), a); 
}

/// Componentwise maximum
template<int N, typename T>
ShVariableN<N,  T> max(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] > rvals[i] ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MAX, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}
SH_SHLIB_CONST_SCALAR_OP(max);

/// Positive - make positive  
template<int N, typename T>
ShVariableN<N,  T> pos(const ShVariableN<N, T>& x) 
{
  if (!ShEnvironment::insideShader) {
    assert(x.hasValues());
    T vals[N];
    x.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = vals[i] > 0 ? vals[i] : 0; 
    return ShConstant<N, T>(result);
  } else {
    T vals[N];
    for( int i = 0; i < N; ++i ) vals[i] = 0;
    ShConstant<N, T> zero(vals);
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, x, SH_OP_MAX, zero);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}


/// Componentwise minimum
template<int N, typename T>
ShVariableN<N,  T> min(const ShVariableN<N, T>& left, const ShVariableN<N, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    T lvals[N];
    left.getValues(lvals);
    T rvals[N];
    right.getValues(rvals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = (lvals[i] < rvals[i] ? lvals[i] : rvals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_MIN, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

SH_SHLIB_CONST_SCALAR_OP(min);


/// Sine of x.
template<int N, typename T>
ShVariableN<N, T> sin(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sin(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SIN, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    
    return t;
  }
}

// Square root.
template<int N, typename T>
ShVariableN<N, T> sqrt(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = std::sqrt(vals[i]);
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_SQRT, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Geometrical operations

template<typename T>
ShVariableN<3, T> cross(const ShVariableN<3, T>& left, const ShVariableN<3, T>& right)
{
  if (!ShEnvironment::insideShader) {
    assert(left.hasValues());
    assert(right.hasValues());
    T lvals[3];
    left.getValues(lvals);
    T rvals[3];
    right.getValues(rvals);
    T result[3];
    result[0] = lvals[1] * rvals[2] - lvals[2] * rvals[1];
    result[1] = -(lvals[0] * rvals[2] - lvals[2] * rvals[0]);
    result[2] = lvals[0] * rvals[1] - lvals[1] * rvals[0];
    return ShConstant<3, T>(result);
  } else {
    ShAttrib<3, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, left, SH_OP_XPD, right);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

template<int N, typename T>
ShVariableN<N, T> normalize(const ShVariableN<N, T>& var)
{
  if (!ShEnvironment::insideShader) {
    ShAttrib1f sizeVar = sqrt(dot(var, var));
    T size;
    sizeVar.getValues(&size);
    assert(var.hasValues());
    T vals[N];
    var.getValues(vals);
    T result[N];
    for (int i = 0; i < N; i++) result[i] = vals[i]/size;
    return ShConstant<N, T>(result);
  } else {
    ShAttrib<N, SH_VAR_TEMP, T, false> t;
    ShStatement stmt(t, SH_OP_NORM, var);
    ShEnvironment::shader->tokenizer.blockList()->addStatement(stmt);
    return t;
  }
}

// Operations on matrices

/// Transpose of a matrix
template<int Rows, int Cols, int Kind, typename T>
ShMatrix<Cols, Rows, SH_VAR_TEMP, T>
transpose(const ShMatrix<Rows, Cols, Kind, T>& m)
{
  ShMatrix<Cols, Rows, SH_VAR_TEMP, T> result;
  for (int i = 0; i < Cols; i++) {
    for (int j = 0; j < Rows; j++) {
      result[i][j] = m[j][i];
    }
  }
  return result;
}

/// Matrix multiplication
template<int M, int N, int P, int Kind, int Kind2, typename T>
ShMatrix<M, P, SH_VAR_TEMP, T>
operator|(const ShMatrix<M, N, Kind, T>& a,
          const ShMatrix<N, P, Kind2, T>& b)
{
  ShMatrix<P, N, SH_VAR_TEMP, T> tb = transpose(b);

  ShMatrix<M, P, SH_VAR_TEMP, T> result;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < P; j++) {
      result[i][j] = dot(a[i], tb[j]);
    }
  }
  return result;
}

/// Treat a variable as a column vector and multiply it with a matrix
template<int M, int N, int Kind, typename T>
ShVariableN<M, T> operator|(const ShMatrix<M, N, Kind, T>& a, const ShVariableN<N, T>& b)
{
  ShAttrib<M, SH_VAR_TEMP, T> ret;
  for (int i = 0; i < M; i++) {
    ret[i] = dot(a[i], b);
  }
  return ret;
}

}

#define SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& var) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(var)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int K1, int K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<N, K2, T, S2>& right) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(left), \
                                       static_cast< ShVariableN<N, T> >(right)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int N, int M, int K1, int K2, typename T, bool S1, bool S2> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, const libtype<M, K2, T, S2>& right) \
{ \
  ShVariableN<libretsize, T> t = libop(static_cast< ShVariableN<N, T> >(left), \
                                       static_cast< ShVariableN<M, T> >(right)); \
  return librettype<libretsize, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_UNEQ_BINARY_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

#define SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, librettype) \
template<int M, int K1, int K2, typename T, bool S1, bool S2> \
librettype<M, SH_VAR_TEMP, T, false> \
libop(const libtype<1, K2, T, S2>& left, const libtype<M, K1, T, S1>& right) \
{ \
  ShVariableN<M, T> t = libop(static_cast< ShVariableN<1, T> >(left), \
                              static_cast< ShVariableN<M, T> >(right)); \
  return librettype<M, SH_VAR_TEMP, T, false>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_SCALAR_OPERATION(libtype, libop) \
  SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(libtype, libop, libtype)

#define SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, librettype, libretsize) \
template<int M, int N, int K1, int K2, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> libop(const ShMatrix<M, N, K1, T>& a, \
                                                    const libtype<N, K2, T, S1>& b) \
{ \
  ShVariableN<libretsize, T> t = libop(a, \
                                       static_cast< ShVariableN<N, T> >(b)); \
  return librettype<libretsize, K1, T, S1>(t.node(), t.swizzle(), t.neg()); \
}

#define SH_SHLIB_LEFT_MATRIX_OPERATION(libtype, libop, libretsize) \
  SH_SHLIB_LEFT_MATRIX_RETTYPE_OPERATION(libtype, libop, libtype, libretsize)

// All the scalar constant stuff

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, librettype, libretsize) \
template<int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShConstant1f(right)); \
} \
template<int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(T left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShConstant1f(left), right); \
} \
template<int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<1, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShConstant1f(right)); \
} \
template<int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(double left, const libtype<1, K1, T, S1>& right) \
{ \
  return libop(ShConstant1f(left), right); \
}

#define SH_SHLIB_SPECIAL_CONST_SCALAR_OP(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(libtype, libop, libtype, 1)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, librettype, libretsize) \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, T right) \
{ \
  return libop(left, ShConstant1f(right)); \
} \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(const libtype<N, K1, T, S1>& left, double right) \
{ \
  return libop(left, ShConstant1f(right)); \
}
#define SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, librettype, libretsize) \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(T left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShConstant1f(left), right); \
} \
template<int N, int K1, typename T, bool S1> \
librettype<libretsize, SH_VAR_TEMP, T, false> \
libop(double left, const libtype<N, K1, T, S1>& right) \
{ \
  return libop(ShConstant1f(left), right); \
}
#define SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, libop) \
  SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, libop, libtype, N)

#define SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(libtype, operation, librettype, libretsize) \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(libtype, operation, librettype, libretsize); \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(libtype, operation, librettype, libretsize);

#define SH_SHLIB_SPECIAL_CONST_N_OP_BOTH(libtype, operation) \
SH_SHLIB_SPECIAL_CONST_N_OP_LEFT(libtype, operation); \
SH_SHLIB_SPECIAL_CONST_N_OP_RIGHT(libtype, operation);

// Standard stuff

#define SH_SHLIB_USUAL_OPERATIONS(type) \
  SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, type)

#define SH_SHLIB_USUAL_OPERATIONS_RETTYPE(type, rettype) \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator+, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator+, rettype, 1);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator*, rettype, N); \
SH_SHLIB_LEFT_SCALAR_RETTYPE_OPERATION(type, operator*, rettype);    \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator*, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, operator*, rettype, N);  \
                                                        \
SH_SHLIB_UNEQ_BINARY_RETTYPE_OPERATION(type, operator/, rettype, N); \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator/, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_LEFT(type, operator/, rettype, N);  \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pow, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pow, rettype, 1);        \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, pow, rettype, N);       \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator^, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator^, rettype, 1);  \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_RIGHT(type, operator^, rettype, N); \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator<=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator<=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>, rettype, N);      \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>, rettype, 1);  \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator>=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator>=, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator==, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator==, rettype, 1); \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, operator!=, rettype, N);     \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, operator!=, rettype, 1); \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, abs, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, acos, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, asin, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, cos, rettype, N);             \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, dot, rettype, 1);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_N_OP_BOTH(type, dot, rettype, 1);           \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, fmod, rettype, N);           \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, fmod, rettype, 1);       \
                                                        \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, frac, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sin, rettype, N);             \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, sqrt, rettype, N);            \
SH_SHLIB_UNARY_RETTYPE_OPERATION(type, normalize, rettype, N);       \
                                                        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, min, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, min, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, pos, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, pos, rettype, 1);        \
SH_SHLIB_BINARY_RETTYPE_OPERATION(type, max, rettype, N);            \
SH_SHLIB_SPECIAL_RETTYPE_CONST_SCALAR_OP(type, max, rettype, 1);

// Points have different subtraction, so we don't include them in our "usuals"
#define SH_SHLIB_USUAL_SUBTRACT(type) \
SH_SHLIB_BINARY_OPERATION(type, operator-, N);      \
SH_SHLIB_SPECIAL_CONST_SCALAR_OP(type, operator-);  \

#include "ShLibAttrib.hpp"
#include "ShLibVector.hpp"
#include "ShLibPoint.hpp"
#include "ShLibColor.hpp"
#include "ShLibTexCoord.hpp"
#include "ShLibNormal.hpp"
#include "ShLibPosition.hpp"

#endif
